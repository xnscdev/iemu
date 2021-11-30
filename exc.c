/* exc.c -- This file is part of IEMU.
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
#include "exc.h"
#include "task.h"

void
exception (enum exc_code exc)
{
  fprintf (stderr, "Exception %u at 0x%04x:0x%04x\n", exc, CS, EIP_REAL);
  exit (1);
}
