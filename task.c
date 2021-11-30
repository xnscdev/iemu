/* task.c -- This file is part of IEMU.
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

#include <stdlib.h>
#include "task.h"

struct task *curr_task;
unsigned char *memory;

struct task *
create_task (unsigned char *buffer)
{
  struct task *task = calloc (1, sizeof (struct task));
  if (!task)
    return NULL;
  task->buffer = buffer;
  task->mode = mode_16;
  return task;
}

void
free_task (struct task *task)
{
  free (task->buffer);
  free (task);
}

void
set_task_current (struct task *task)
{
  curr_task = task;
  memory = task->buffer;
  registers = &task->registers;
}

void
execute (void)
{
  init_opcodes ();
  while (1)
    exec_inst ();
}
