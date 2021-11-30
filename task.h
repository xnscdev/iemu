/* task.h -- This file is part of IEMU.
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

#ifndef __TASK_H
#define __TASK_H

#include "opcode.h"
#include "register.h"

#define CURRENT_INST (memory[(unsigned int) CS * 16 + EIP])

enum opmode
{
  mode_16,
  mode_32,
  mode_64
};

enum opsize
{
  size_8,
  size_16,
  size_32,
  size_64
};

struct task
{
  unsigned char *buffer;
  enum opmode mode;
  struct registers registers;
};

extern struct task *curr_task;
extern unsigned char *memory;

struct task *create_task (unsigned char *buffer);
void free_task (struct task *task);
void set_task_current (struct task *task);
void execute (void);

#endif
