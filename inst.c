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

#include <string.h>
#include "task.h"

#define SIZE_HALF_LIMIT(size) (((unsigned long) 1 << (size * 8 - 1)) - 1)
#define SIZE_LIMIT(size)      (((unsigned long) 1 << (size * 8)) - 1)

#define __I_ARITH(op, size, dest, src, real)			\
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
      if (((dest & 0xf) op (src & 0xf)) > 0xf)			\
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
      if (real)							\
	dest = x;						\
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

#define I_ARITH(op, size, dest, src) __I_ARITH (op, size, dest, src, 1)

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

void
i_or (enum opmode size, unsigned char *dest, unsigned char *src)
{
  switch (size)
    {
    case op_8:
      I_ARITH (|, 1, *dest, *src);
      break;
    case op_16:
      I_ARITH (|, 2, OP16 (dest), OP16 (src));
      break;
    case op_32:
      I_ARITH (|, 4, OP32 (dest), OP32 (src));
      break;
    }
}

void
i_adc (enum opmode size, unsigned char *dest, unsigned char *src)
{
  unsigned char cf = !!(EFLAGS & CF);
  switch (size)
    {
    case op_8:
      I_ARITH (+ cf +, 1, *dest, *src);
      break;
    case op_16:
      I_ARITH (+ cf +, 2, OP16 (dest), OP16 (src));
      break;
    case op_32:
      I_ARITH (+ cf +, 4, OP32 (dest), OP32 (src));
      break;
    }
}

void
i_sbb (enum opmode size, unsigned char *dest, unsigned char *src)
{
  unsigned char cf = !!(EFLAGS & CF);
  switch (size)
    {
    case op_8:
      I_ARITH (- cf -, 1, *dest, *src);
      break;
    case op_16:
      I_ARITH (- cf -, 2, OP16 (dest), OP16 (src));
      break;
    case op_32:
      I_ARITH (- cf -, 4, OP32 (dest), OP32 (src));
      break;
    }
}

void
i_and (enum opmode size, unsigned char *dest, unsigned char *src)
{
  switch (size)
    {
    case op_8:
      I_ARITH (&, 1, *dest, *src);
      break;
    case op_16:
      I_ARITH (&, 2, OP16 (dest), OP16 (src));
      break;
    case op_32:
      I_ARITH (&, 4, OP32 (dest), OP32 (src));
      break;
    }
}

void
i_sub (enum opmode size, unsigned char *dest, unsigned char *src)
{
  switch (size)
    {
    case op_8:
      I_ARITH (-, 1, *dest, *src);
      break;
    case op_16:
      I_ARITH (-, 2, OP16 (dest), OP16 (src));
      break;
    case op_32:
      I_ARITH (-, 4, OP32 (dest), OP32 (src));
      break;
    }
}

void
i_xor (enum opmode size, unsigned char *dest, unsigned char *src)
{
  switch (size)
    {
    case op_8:
      I_ARITH (^, 1, *dest, *src);
      break;
    case op_16:
      I_ARITH (^, 2, OP16 (dest), OP16 (src));
      break;
    case op_32:
      I_ARITH (^, 4, OP32 (dest), OP32 (src));
      break;
    }
}

void
i_cmp (enum opmode size, unsigned char *dest, unsigned char *src)
{
  switch (size)
    {
    case op_8:
      __I_ARITH (-, 1, *dest, *src, 0);
      break;
    case op_16:
      __I_ARITH (-, 2, OP16 (dest), OP16 (src), 0);
      break;
    case op_32:
      __I_ARITH (-, 4, OP32 (dest), OP32 (src), 0);
      break;
    }
}

void
i_push (enum opmode size, unsigned char *value)
{
  memcpy (memory + ESP - size, value, size);
  ESP -= size;
}

void
i_pop (enum opmode size, unsigned char *value)
{
  ESP += size;
  memcpy (value, memory + ESP - size, size);
}

void
i_daa (void)
{
  unsigned char cf = EFLAGS & CF;
  unsigned char al = AL;
  EFLAGS &= ~CF;
  if ((AL & 0xf) > 9 || (EFLAGS & AF))
    {
      if (cf || AL > 0xf9)
	EFLAGS |= CF;
      AL += 6;
      EFLAGS |= AF;
    }
  else
    EFLAGS &= ~AF;
  if (al > 0x99 || cf)
    {
      AL += 0x60;
      EFLAGS |= CF;
    }
  else
    EFLAGS &= ~CF;
}

void
i_das (void)
{
  unsigned char cf = EFLAGS & CF;
  unsigned char al = AL;
  EFLAGS &= ~CF;
  if ((AL & 0xf) > 9 || (EFLAGS & AF))
    {
      if (cf || AL < 6)
	EFLAGS |= CF;
      AL -= 6;
      EFLAGS |= AF;
    }
  else
    EFLAGS &= ~AF;
  if (al > 0x99 || cf)
    {
      AL -= 0x60;
      EFLAGS |= CF;
    }
}

void
i_aaa (void)
{
  if ((AL & 0xf) > 9 || (EFLAGS & AF))
    {
      AX += 0x106;
      EFLAGS |= AF | CF;
    }
  else
    EFLAGS &= ~(AF | CF);
  AL &= 0xf;
}

void
i_aas (void)
{
  if ((AL & 0xf) > 9 || (EFLAGS & AF))
    {
      AX -= 6;
      AH--;
      EFLAGS |= AF | CF;
    }
  else
    EFLAGS &= ~(AF | CF);
  AL &= 0xf;
}

void
i_inc (enum opmode size, unsigned char *value)
{
  unsigned char cf = EFLAGS & CF;
  switch (size)
    {
    case op_8:
      I_ARITH (+, 1, *value, 1);
      break;
    case op_16:
      I_ARITH (+, 2, OP16 (value), 1);
      break;
    case op_32:
      I_ARITH (+, 4, OP32 (value), 1);
      break;
    }
  EFLAGS &= ~CF;
  EFLAGS |= cf;
}

void
i_dec (enum opmode size, unsigned char *value)
{
  unsigned char cf = EFLAGS & CF;
  switch (size)
    {
    case op_8:
      I_ARITH (-, 1, *value, 1);
      break;
    case op_16:
      I_ARITH (-, 2, OP16 (value), 1);
      break;
    case op_32:
      I_ARITH (-, 4, OP32 (value), 1);
      break;
    }
  EFLAGS &= ~CF;
  EFLAGS |= cf;
}
