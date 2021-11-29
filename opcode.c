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

void
decode_modrm (unsigned char **dest, unsigned char **src)
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
    }
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
