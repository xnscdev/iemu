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
exec_inst (void)
{
  unsigned char opcode = memory[registers->cs * 16 + registers->eip];
  switch (opcode)
    {
    case 0x00:
      registers->eip++;
      break;
    default:
      fprintf (stderr, "Invalid opcode 0x%02x at 0x%04x:0x%04x\n", opcode,
	       registers->cs, registers->eip);
      exit (1);
    }
}
