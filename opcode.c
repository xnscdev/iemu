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

static void
decode_modrm_16 (unsigned char **dest, unsigned char **src)
{
  unsigned char byte = CURRENT_INST;
  registers->eip++;
  switch (byte >> 6)
    {
    case 0:
      switch ((byte >> 3) & 7)
	{
	case 0:
	  *dest = memory + registers->bx + registers->si;
	  break;
	case 1:
	  *dest = memory + registers->bx + registers->di;
	  break;
	case 2:
	  *dest = memory + registers->bp + registers->si;
	  break;
	case 3:
	  *dest = memory + registers->bp + registers->di;
	  break;
	case 4:
	  *dest = memory + registers->si;
	  break;
	case 5:
	  *dest = memory + registers->di;
	  break;
	case 6:
	  *dest = memory + *((unsigned short *) &CURRENT_INST);
	  registers->eip += 2;
	  break;
	case 7:
	  *dest = memory + registers->bx;
	  break;
	}
      break;
    case 1:
      switch ((byte >> 3) & 7)
	{
	case 0:
	  *dest = memory + registers->bx + registers->si + CURRENT_INST;
	  registers->eip++;
	  break;
	case 1:
	  *dest = memory + registers->bx + registers->di + CURRENT_INST;
	  registers->eip++;
	  break;
	case 2:
	  *dest = memory + registers->bp + registers->si + CURRENT_INST;
	  registers->eip++;
	  break;
	case 3:
	  *dest = memory + registers->bp + registers->di + CURRENT_INST;
	  registers->eip++;
	  break;
	case 4:
	  *dest = memory + registers->si + CURRENT_INST;
	  registers->eip++;
	  break;
	case 5:
	  *dest = memory + registers->di + CURRENT_INST;
	  registers->eip++;
	  break;
	case 6:
	  *dest = memory + registers->bp + CURRENT_INST;
	  registers->eip++;
	  break;
	case 7:
	  *dest = memory + registers->bx + CURRENT_INST;
	  registers->eip++;
	  break;
	}
      break;
    case 2:
      switch ((byte >> 3) & 7)
	{
	case 0:
	  *dest = memory + registers->bx + registers->si + CURRENT_INST;
	  registers->eip++;
	  break;
	case 1:
	  *dest = memory + registers->bx + registers->di +
	    *((unsigned short *) &CURRENT_INST);
	  registers->eip += 2;
	  break;
	case 2:
	  *dest = memory + registers->bp + registers->si +
	    *((unsigned short *) &CURRENT_INST);
	  registers->eip += 2;
	  break;
	case 3:
	  *dest = memory + registers->bp + registers->di +
	    *((unsigned short *) &CURRENT_INST);
	  registers->eip += 2;
	  break;
	case 4:
	  *dest = memory + registers->si + *((unsigned short *) &CURRENT_INST);
	  registers->eip += 2;
	  break;
	case 5:
	  *dest = memory + registers->di + *((unsigned short *) &CURRENT_INST);
	  registers->eip += 2;
	  break;
	case 6:
	  *dest = memory + registers->bp + *((unsigned short *) &CURRENT_INST);
	  registers->eip += 2;
	  break;
	case 7:
	  *dest = memory + registers->bx + *((unsigned short *) &CURRENT_INST);
	  registers->eip += 2;
	  break;
	}
      break;
    }
}

static void
decode_modrm_32 (unsigned char **dest, unsigned char **src)
{
}

void
decode_modrm (unsigned char **dest, unsigned char **src)
{
  if (curr_task->mode == op_16)
    decode_modrm_16 (dest, src);
  else
    decode_modrm_32 (dest, src);
}

void
exec_inst (void)
{
  unsigned char opcode = CURRENT_INST;
  unsigned char *dest;
  unsigned char *src;
  switch (opcode)
    {
    case 0x00:
      registers->eip++;
      decode_modrm (&dest, &src);
      *dest += *src;
      break;
    default:
      fprintf (stderr, "Invalid opcode 0x%02x at 0x%04x:0x%04x\n", opcode,
	       registers->cs, registers->eip);
      exit (1);
    }
}
