/* register.h -- This file is part of IEMU.
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

#ifndef __REGISTER_H
#define __REGISTER_H

struct registers
{
  union
  {
    unsigned int eax;
    unsigned short ax;
    struct
    {
      unsigned char al;
      unsigned char ah;
    };
  };
  union
  {
    unsigned int ecx;
    unsigned short cx;
    struct
    {
      unsigned char cl;
      unsigned char ch;
    };
  };
  union
  {
    unsigned int edx;
    unsigned short dx;
    struct
    {
      unsigned char dl;
      unsigned char dh;
    };
  };
  union
  {
    unsigned int ebx;
    unsigned short bx;
    struct
    {
      unsigned char bl;
      unsigned char bh;
    };
  };
  union
  {
    unsigned int esp;
    unsigned short sp;
  };
  union
  {
    unsigned int ebp;
    unsigned short bp;
  };
  union
  {
    unsigned int esi;
    unsigned short si;
  };
  union
  {
    unsigned int edi;
    unsigned short di;
  };
  unsigned int eip;
  unsigned int eflags;
  unsigned short cs;
  unsigned short ds;
  unsigned short es;
  unsigned short fs;
  unsigned short gs;
  unsigned short ss;
};

extern struct registers *registers;

#endif
