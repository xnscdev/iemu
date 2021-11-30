/* inst.h -- This file is part of IEMU.
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

#ifndef __INST_H
#define __INST_H

enum opmode
{
  op_8      = 1,
  op_16     = 2,
  op_32     = 4
};

void i_add (enum opmode size, unsigned char *dest, unsigned char *src);
void i_or (enum opmode size, unsigned char *dest, unsigned char *src);
void i_adc (enum opmode size, unsigned char *dest, unsigned char *src);
void i_sbb (enum opmode size, unsigned char *dest, unsigned char *src);
void i_and (enum opmode size, unsigned char *dest, unsigned char *src);
void i_sub (enum opmode size, unsigned char *dest, unsigned char *src);
void i_xor (enum opmode size, unsigned char *dest, unsigned char *src);
void i_cmp (enum opmode size, unsigned char *dest, unsigned char *src);
void i_push (enum opmode size, unsigned char *value);
void i_pop (enum opmode size, unsigned char *value);
void i_daa (void);
void i_das (void);
void i_aaa (void);
void i_aas (void);
void i_inc (enum opmode size, unsigned char *value);
void i_dec (enum opmode size, unsigned char *value);
void i_pusha (enum opmode size);
void i_popa (enum opmode size);
void i_bound (enum opmode size, unsigned char *value, unsigned char *bounds);

#endif
