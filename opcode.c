/* opcode.c -- This file is part of IEMU.
   Copyright (C) 2021 XNSC

   IEMU is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   IEMU is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with IEMU. If not, see <https://www.gnu.org/licenses/>. */

#include <stddef.h>
#include "exc.h"
#include "task.h"

static unsigned char sib_scales[4] = {1, 2, 4, 8};
static unsigned char *reg_map_8[8];
static unsigned int *reg_map_32[8];

static void
decode_rm_16 (unsigned char byte, enum opmode size, unsigned char **rm,
	      char *ss)
{
  switch (byte >> 6)
    {
    case 0:
      switch (byte & 7)
	{
	case 0:
	  *rm = memory + BX + SI;
	  break;
	case 1:
	  *rm = memory + BX + DI;
	  break;
	case 2:
	  *rm = memory + BP + SI;
	  *ss = 1;
	  break;
	case 3:
	  *rm = memory + BP + DI;
	  *ss = 1;
	  break;
	case 4:
	  *rm = memory + SI;
	  break;
	case 5:
	  *rm = memory + DI;
	  break;
	case 6:
	  *rm = memory + *((unsigned short *) &CURRENT_INST);
	  EIP += 2;
	  break;
	case 7:
	  *rm = memory + BX;
	  break;
	}
      break;
    case 1:
      switch (byte & 7)
	{
	case 0:
	  *rm = memory + BX + SI + CURRENT_INST;
	  EIP++;
	  break;
	case 1:
	  *rm = memory + BX + DI + CURRENT_INST;
	  EIP++;
	  break;
	case 2:
	  *rm = memory + BP + SI + CURRENT_INST;
	  *ss = 1;
	  EIP++;
	  break;
	case 3:
	  *rm = memory + BP + DI + CURRENT_INST;
	  *ss = 1;
	  EIP++;
	  break;
	case 4:
	  *rm = memory + SI + CURRENT_INST;
	  EIP++;
	  break;
	case 5:
	  *rm = memory + DI + CURRENT_INST;
	  EIP++;
	  break;
	case 6:
	  *rm = memory + BP + CURRENT_INST;
	  *ss = 1;
	  EIP++;
	  break;
	case 7:
	  *rm = memory + BX + CURRENT_INST;
	  EIP++;
	  break;
	}
      break;
    case 2:
      switch (byte & 7)
	{
	case 0:
	  *rm = memory + BX + SI + *((unsigned short *) &CURRENT_INST);
	  EIP += 2;
	  break;
	case 1:
	  *rm = memory + BX + DI + *((unsigned short *) &CURRENT_INST);
	  EIP += 2;
	  break;
	case 2:
	  *rm = memory + BP + SI + *((unsigned short *) &CURRENT_INST);
	  *ss = 1;
	  EIP += 2;
	  break;
	case 3:
	  *rm = memory + BP + DI + *((unsigned short *) &CURRENT_INST);
	  *ss = 1;
	  EIP += 2;
	  break;
	case 4:
	  *rm = memory + SI + *((unsigned short *) &CURRENT_INST);
	  EIP += 2;
	  break;
	case 5:
	  *rm = memory + DI + *((unsigned short *) &CURRENT_INST);
	  EIP += 2;
	  break;
	case 6:
	  *rm = memory + BP + *((unsigned short *) &CURRENT_INST);
	  *ss = 1;
	  EIP += 2;
	  break;
	case 7:
	  *rm = memory + BX + *((unsigned short *) &CURRENT_INST);
	  EIP += 2;
	  break;
	}
      break;
    case 3:
      if (size == op_8)
	*rm = reg_map_8[byte & 7];
      else
	{
	  *rm = (unsigned char *) reg_map_32[byte & 7];
	  if ((byte & 7) == 4 || (byte & 7) == 5)
	    *ss = 1;
	}
      break;
    }
}

static void
decode_sib (enum opmode size, unsigned char **rm, char *ss)
{
  unsigned char byte = CURRENT_INST;
  EIP++;
  if ((byte & 7) == 4)
    exception (exc_UD);
  if (((byte >> 3) & 7) == 5)
    *ss = 1;
  *rm = memory + *reg_map_32[byte & 7] * sib_scales[byte >> 6] +
    *reg_map_32[(byte >> 3) & 7];
}

static int
decode_rm_32 (unsigned char byte, enum opmode size, unsigned char **rm,
	      char *ss)
{
  switch (byte >> 6)
    {
    case 0:
      switch (byte & 7)
	{
	case 4:
	  decode_sib (size, rm, ss);
	  return 1;
	case 5:
	  *rm = memory + *((unsigned int *) &CURRENT_INST);
	  EIP += 4;
	  break;
	default:
	  *rm = memory + *reg_map_32[byte & 7];
	}
      break;
    case 1:
      if ((byte & 7) == 4)
	{
	  decode_sib (size, rm, ss);
	  *rm += CURRENT_INST;
	  EIP++;
	  return 1;
	}
      else
	{
	  if ((byte & 7) == 5)
	    *ss = 1;
	  *rm = memory + *reg_map_32[byte & 7] +
	    *((unsigned int *) &CURRENT_INST);
	  EIP += 4;
	}
      break;
    case 2:
      if ((byte & 7) == 4)
	{
	  decode_sib (size, rm, ss);
	  *rm += *((unsigned int *) &CURRENT_INST);
	  EIP += 4;
	  return 1;
	}
      else
	{
	  if ((byte & 7) == 5)
	    *ss = 1;
	  *rm = memory + *reg_map_32[byte & 7] +
	    *((unsigned int *) &CURRENT_INST);
	  EIP += 4;
	}
      break;
    case 3:
      if (size == op_8)
	*rm = reg_map_8[byte & 7];
      else
	{
	  *rm = (unsigned char *) reg_map_32[byte & 7];
	  if ((byte & 7) == 4 || (byte & 7) == 5)
	    *ss = 1;
	}
      break;
    }
  return 0;
}

static void
decode_modrm (enum opmode size, unsigned short *segment, unsigned char **rm,
	      unsigned char **r)
{
  unsigned char byte = CURRENT_INST;
  char ss = 0;
  EIP++;
  if (curr_task->mode == op_16)
    decode_rm_16 (byte, size, rm, &ss);
  else if (decode_rm_32 (byte, size, rm, &ss))
    {
      if (segment)
	*rm += *segment * 16;
      else if (ss)
	*rm += SS * 16;
      else
	*rm += DS * 16;
      return;
    }
  if (segment)
    *rm += *segment * 16;
  else if (ss)
    *rm += SS * 16;
  else
    *rm += DS * 16;
  if (size == op_8)
    *r = reg_map_8[(byte >> 3) & 7];
  else
    *r = (unsigned char *) reg_map_32[(byte >> 3) & 7];
}

static void
exec_inst_2b (void)
{
  unsigned char opcode = CURRENT_INST;
  EIP++;
  exception (exc_UD);
}

void
exec_inst (void)
{
  unsigned char opcode = CURRENT_INST;
  unsigned short *segment = NULL;
  unsigned char *rm;
  unsigned char *r;
  enum opmode size = curr_task->mode;
  EIP++;

 read:
  switch (opcode)
    {
    case 0x00: /* ADD r/m8, r8 */
      size = op_8;
    case 0x01: /* ADD r/m16/32, r16/32 */
      decode_modrm (size, segment, &rm, &r);
      i_add (size, rm, r);
      break;
    case 0x02: /* ADD r8, r/m8 */
      size = op_8;
    case 0x03: /* ADD r16/32, r/m16/32 */
      decode_modrm (size, segment, &rm, &r);
      i_add (size, r, rm);
      break;
    case 0x04: /* ADD AL, imm8 */
      size = op_8;
    case 0x05: /* ADD eAX, imm16/32 */
      i_add (size, &AL, &CURRENT_INST);
      EIP += size;
      break;
    case 0x06: /* PUSH ES */
      i_push (curr_task->mode, (unsigned char *) &ES);
      break;
    case 0x07: /* POP ES */
      i_pop (curr_task->mode, (unsigned char *) &ES);
      break;
    case 0x08: /* OR r/m8, r8 */
      size = op_8;
    case 0x09: /* OR r/m16/32, r16/32 */
      decode_modrm (size, segment, &rm, &r);
      i_or (size, rm, r);
      break;
    case 0x0a: /* OR r8, r/m8 */
      size = op_8;
    case 0x0b: /* OR r16/32, r/m16/32 */
      decode_modrm (size, segment, &rm, &r);
      i_or (size, r, rm);
      break;
    case 0x0c: /* OR AL, imm8 */
      size = op_8;
    case 0x0d: /* OR eAX, imm16/32 */
      i_or (size, &AL, &CURRENT_INST);
      EIP += size;
      break;
    case 0x0e: /* PUSH CS */
      i_push (curr_task->mode, (unsigned char *) &CS);
      break;
    case 0x0f: /* Two-byte instructions */
      exec_inst_2b ();
      break;
    case 0x10: /* ADC r/m8, r8 */
      size = op_8;
    case 0x11: /* ADC r/m16/32, r16/32 */
      decode_modrm (size, segment, &rm, &r);
      i_adc (size, rm, r);
      break;
    case 0x12: /* ADC r8, r/m8 */
      size = op_8;
    case 0x13: /* ADC r16/32, r/m16/32 */
      decode_modrm (size, segment, &rm, &r);
      i_adc (size, r, rm);
      break;
    case 0x14: /* ADC AL, imm8 */
      size = op_8;
    case 0x15: /* ADC eAX, imm16/32 */
      i_adc (size, &AL, &CURRENT_INST);
      EIP += size;
      break;
    case 0x16: /* PUSH SS */
      i_push (curr_task->mode, (unsigned char *) &SS);
      break;
    case 0x17: /* POP SS */
      i_pop (curr_task->mode, (unsigned char *) &SS);
      break;
    case 0x18: /* SBB r/m8, r8 */
      size = op_8;
    case 0x19: /* SBB r/m16/32, r16/32 */
      decode_modrm (size, segment, &rm, &r);
      i_sbb (size, rm, r);
      break;
    case 0x1a: /* SBB r8, r/m8 */
      size = op_8;
    case 0x1b: /* SBB r16/32, r/m16/32 */
      decode_modrm (size, segment, &rm, &r);
      i_sbb (size, r, rm);
      break;
    case 0x1c: /* SBB AL, imm8 */
      size = op_8;
    case 0x1d: /* SBB eAX, imm16/32 */
      i_sbb (size, &AL, &CURRENT_INST);
      EIP += size;
      break;
    case 0x1e: /* PUSH DS */
      i_push (curr_task->mode, (unsigned char *) &DS);
      break;
    case 0x1f: /* POP DS */
      i_pop (curr_task->mode, (unsigned char *) &DS);
      break;
    case 0x20: /* AND r/m8, r8 */
      size = op_8;
    case 0x21: /* AND r/m16/32, r16/32 */
      decode_modrm (size, segment, &rm, &r);
      i_and (size, rm, r);
      break;
    case 0x22: /* AND r8, r/m8 */
      size = op_8;
    case 0x23: /* AND r16/32, r/m16/32 */
      decode_modrm (size, segment, &rm, &r);
      i_and (size, r, rm);
      break;
    case 0x24: /* AND AL, imm8 */
      size = op_8;
    case 0x25: /* AND eAX, imm16/32 */
      i_and (size, &AL, &CURRENT_INST);
      EIP += size;
      break;
    case 0x26: /* ES segment override prefix */
      segment = &ES;
      opcode = CURRENT_INST;
      EIP++;
      goto read;
    case 0x27: /* DAA */
      i_daa ();
      break;
    case 0x28: /* SUB r/m8, r8 */
      size = op_8;
    case 0x29: /* SUB r/m16/32, r16/32 */
      decode_modrm (size, segment, &rm, &r);
      i_sub (size, rm, r);
      break;
    case 0x2a: /* SUB r8, r/m8 */
      size = op_8;
    case 0x2b: /* SUB r16/32, r/m16/32 */
      decode_modrm (size, segment, &rm, &r);
      i_sub (size, r, rm);
      break;
    case 0x2c: /* SUB AL, imm8 */
      size = op_8;
    case 0x2d: /* SUB eAX, imm16/32 */
      i_sub (size, &AL, &CURRENT_INST);
      EIP += size;
      break;
    case 0x2e: /* CS segment override prefix */
      segment = &CS;
      opcode = CURRENT_INST;
      EIP++;
      goto read;
    case 0x2f: /* DAS */
      i_das ();
      break;
    case 0x30: /* XOR r/m8, r8 */
      size = op_8;
    case 0x31: /* XOR r/m16/32, r16/32 */
      decode_modrm (size, segment, &rm, &r);
      i_xor (size, rm, r);
      break;
    case 0x32: /* XOR r8, r/m8 */
      size = op_8;
    case 0x33: /* XOR r16/32, r/m16/32 */
      decode_modrm (size, segment, &rm, &r);
      i_xor (size, r, rm);
      break;
    case 0x34: /* XOR AL, imm8 */
      size = op_8;
    case 0x35: /* XOR eAX, imm16/32 */
      i_xor (size, &AL, &CURRENT_INST);
      EIP += size;
      break;
    case 0x36: /* SS segment override prefix */
      segment = &SS;
      opcode = CURRENT_INST;
      EIP++;
      goto read;
    case 0x37: /* AAA */
      i_aaa ();
      break;
    case 0x38: /* CMP r/m8, r8 */
      size = op_8;
    case 0x39: /* CMP r/m16/32, r16/32 */
      decode_modrm (size, segment, &rm, &r);
      i_cmp (size, rm, r);
      break;
    case 0x3a: /* CMP r8, r/m8 */
      size = op_8;
    case 0x3b: /* CMP r16/32, r/m16/32 */
      decode_modrm (size, segment, &rm, &r);
      i_cmp (size, r, rm);
      break;
    case 0x3c: /* CMP AL, imm8 */
      size = op_8;
    case 0x3d: /* CMP eAX, imm16/32 */
      i_cmp (size, &AL, &CURRENT_INST);
      EIP += size;
      break;
    case 0x3e: /* DS segment override prefix */
      segment = &DS;
      opcode = CURRENT_INST;
      EIP++;
      goto read;
    case 0x3f: /* AAS */
      i_aas ();
      break;
    case 0x40: /* INC r16/32 */
    case 0x41:
    case 0x42:
    case 0x43:
    case 0x44:
    case 0x45:
    case 0x46:
    case 0x47:
      i_inc (size, (unsigned char *) reg_map_32[opcode - 0x40]);
      break;
    case 0x48: /* DEC r16/32 */
    case 0x49:
    case 0x4a:
    case 0x4b:
    case 0x4c:
    case 0x4d:
    case 0x4e:
    case 0x4f:
      i_dec (size, (unsigned char *) reg_map_32[opcode - 0x48]);
      break;
    case 0x50: /* PUSH r16/32 */
    case 0x51:
    case 0x52:
    case 0x53:
    case 0x54:
    case 0x55:
    case 0x56:
    case 0x57:
      i_push (size, (unsigned char *) reg_map_32[opcode - 0x50]);
      break;
    case 0x58: /* POP r16/32 */
    case 0x59:
    case 0x5a:
    case 0x5b:
    case 0x5c:
    case 0x5d:
    case 0x5e:
    case 0x5f:
      i_pop (size, (unsigned char *) reg_map_32[opcode - 0x58]);
      break;
    case 0x60: /* PUSHA */
      i_pusha (size);
      break;
    case 0x61: /* POPA */
      i_popa (size);
      break;
    case 0x62: /* BOUND r16/32, m16/32&16/32 */
      decode_modrm (size, segment, &rm, &r);
      i_bound (size, r, rm);
      break;
    case 0x66: /* Operand size override prefix */
      if (size == op_16)
	size = op_32;
      else
	size = op_16;
      opcode = CURRENT_INST;
      EIP++;
      goto read;
    default:
      exception (exc_UD);
    }

  EIP_REAL = EIP;
}

void
init_opcodes (void)
{
  reg_map_8[0] = &AL;
  reg_map_8[1] = &CL;
  reg_map_8[2] = &DL;
  reg_map_8[3] = &BL;
  reg_map_8[4] = &AH;
  reg_map_8[5] = &CH;
  reg_map_8[6] = &DH;
  reg_map_8[7] = &BH;
  reg_map_32[0] = &EAX;
  reg_map_32[1] = &ECX;
  reg_map_32[2] = &EDX;
  reg_map_32[3] = &EBX;
  reg_map_32[4] = &ESP;
  reg_map_32[5] = &EBP;
  reg_map_32[6] = &ESI;
  reg_map_32[7] = &EDI;
}
