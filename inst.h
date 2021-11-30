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
void i_push (enum opmode size, unsigned char *value);
void i_pop (enum opmode size, unsigned char *value);

#endif
