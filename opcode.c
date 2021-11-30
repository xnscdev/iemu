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

#include <stdio.h>
#include <stdlib.h>
#include "task.h"

static unsigned char sib_scales[4] = {1, 2, 4, 8};
static unsigned char *reg_map_8[8];
static unsigned int *reg_map_32[8];

static void
invalid_opcode (void)
{
  fprintf (stderr, "Invalid opcode at 0x%04x:0x%04x\n", CS, EIP_REAL);
  exit (1);
}

static void
decode_rm_16 (unsigned char byte, enum opmode size, unsigned char **rm)
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
	  break;
	case 3:
	  *rm = memory + BP + DI;
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
	  EIP++;
	  break;
	case 3:
	  *rm = memory + BP + DI + CURRENT_INST;
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
	  EIP += 2;
	  break;
	case 3:
	  *rm = memory + BP + DI + *((unsigned short *) &CURRENT_INST);
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
	*rm = (unsigned char *) reg_map_32[byte & 7];
      break;
    }
}

static void
decode_sib (enum opmode size, unsigned char **rm)
{
  unsigned char byte = CURRENT_INST;
  EIP++;
  if ((byte & 7) == 4)
    invalid_opcode ();
  *rm = memory + *reg_map_32[byte & 7] * sib_scales[byte >> 6] +
    *reg_map_32[(byte >> 3) & 7];
}

static int
decode_rm_32 (unsigned char byte, enum opmode size, unsigned char **rm)
{
  switch (byte >> 6)
    {
    case 0:
      switch (byte & 7)
	{
	case 4:
	  decode_sib (size, rm);
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
	  decode_sib (size, rm);
	  *rm += CURRENT_INST;
	  EIP++;
	  return 1;
	}
      else
	{
	  *rm = memory + *reg_map_32[byte & 7] +
	    *((unsigned int *) &CURRENT_INST);
	  EIP += 4;
	}
      break;
    case 2:
      if ((byte & 7) == 4)
	{
	  decode_sib (size, rm);
	  *rm += *((unsigned int *) &CURRENT_INST);
	  EIP += 4;
	  return 1;
	}
      else
	{
	  *rm = memory + *reg_map_32[byte & 7] +
	    *((unsigned int *) &CURRENT_INST);
	  EIP += 4;
	}
      break;
    case 3:
      if (size == op_8)
	*rm = reg_map_8[byte & 7];
      else
	*rm = (unsigned char *) reg_map_32[byte & 7];
      break;
    }
  return 0;
}

static void
decode_modrm (enum opmode size, unsigned char **rm, unsigned char **r)
{
  unsigned char byte = CURRENT_INST;
  EIP++;
  if (curr_task->mode == op_16)
    decode_rm_16 (byte, size, rm);
  else if (decode_rm_32 (byte, size, rm))
    return;
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
  invalid_opcode ();
}

void
exec_inst (void)
{
  unsigned char opcode = CURRENT_INST;
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
      decode_modrm (size, &rm, &r);
      i_add (size, rm, r);
      break;
    case 0x02: /* ADD r8, r/m8 */
      size = op_8;
    case 0x03: /* ADD r16/32, r/m16/32 */
      decode_modrm (size, &rm, &r);
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
      decode_modrm (size, &rm, &r);
      i_or (size, rm, r);
      break;
    case 0x0a: /* OR r8, r/m8 */
      size = op_8;
    case 0x0b: /* OR r16/32, r/m16/32 */
      decode_modrm (size, &rm, &r);
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
      decode_modrm (size, &rm, &r);
      i_adc (size, rm, r);
      break;
    case 0x12: /* ADC r8, r/m8 */
      size = op_8;
    case 0x13: /* ADC r16/32, r/m16/32 */
      decode_modrm (size, &rm, &r);
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
      decode_modrm (size, &rm, &r);
      i_sbb (size, rm, r);
      break;
    case 0x1a: /* SBB r8, r/m8 */
      size = op_8;
    case 0x1b: /* SBB r16/32, r/m16/32 */
      decode_modrm (size, &rm, &r);
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
    case 0x66: /* Operand size override prefix */
      if (size == op_16)
	size = op_32;
      else
	size = op_16;
      opcode = CURRENT_INST;
      EIP++;
      goto read;
    default:
      invalid_opcode ();
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
