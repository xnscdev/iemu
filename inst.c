/* inst.c -- This file is part of IEMU.
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

#include "task.h"

#define SIZE_HALF_LIMIT(size) (((unsigned long) 1 << (size * 8 - 1)) - 1)
#define SIZE_LIMIT(size)      (((unsigned long) 1 << (size * 8)) - 1)

#define I_ARITH(op, size, dest, src)				\
  do								\
    {								\
      unsigned long x = dest op src;				\
      if (x > SIZE_LIMIT (size))				\
	EFLAGS |= CF;						\
      else							\
	EFLAGS &= ~CF;						\
      if ((x & SIZE_LIMIT (size)) > SIZE_HALF_LIMIT (size))	\
	EFLAGS |= OF;						\
      else							\
	EFLAGS &= ~OF;						\
      if ((dest & 0xf) op (src & 0xf) > 0xf)			\
	EFLAGS |= AF;						\
      else							\
	EFLAGS &= ~AF;						\
      if (!x)							\
	EFLAGS |= ZF;						\
      else							\
	EFLAGS &= ~ZF;						\
      if (x & (SIZE_HALF_LIMIT (size) + 1))			\
	EFLAGS |= SF;						\
      else							\
	EFLAGS &= ~SF;						\
      dest = x;							\
      x ^= x >> 16;						\
      x ^= x >> 8;						\
      x ^= x >> 4;						\
      x ^= x >> 2;						\
      x ^= x >> 1;						\
      if (x & 1)						\
	EFLAGS |= PF;						\
      else							\
	EFLAGS &= ~PF;						\
    }								\
  while (0)

void
i_add (enum opmode size, unsigned char *dest, unsigned char *src)
{
  switch (size)
    {
    case op_8:
      I_ARITH (+, 1, *dest, *src);
      break;
    case op_16:
      I_ARITH (+, 2, OP16 (dest), OP16 (src));
      break;
    case op_32:
      I_ARITH (+, 4, OP32 (dest), OP32 (src));
      break;
    }
}
