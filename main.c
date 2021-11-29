/* main.c -- This file is part of IEMU.
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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "task.h"

#define MEMORY_SIZE 0x100000

static unsigned char *
buffer_from_file (const char *path)
{
  FILE *file = fopen (path, "rb");
  unsigned char *buffer;
  size_t len;
  if (!file)
    return NULL;
  fseek (file, 0, SEEK_END);
  len = ftell (file);
  rewind (file);
  buffer = calloc (1, MEMORY_SIZE);
  if (!buffer)
    return NULL;
  fread (buffer, 1, len, file);
  fclose (file);
  return buffer;
}

int
main (int argc, char **argv)
{
  struct task *task;
  unsigned char *buffer;
  if (argc != 2)
    {
      fprintf (stderr, "Usage: iemu FILE\n");
      exit (1);
    }
  buffer = buffer_from_file (argv[1]);
  if (!buffer)
    {
      fprintf (stderr, "%s: %s\n", argv[1], strerror (errno));
      exit (1);
    }
  task = create_task (buffer);
  set_task_current (task);
  execute ();
  free_task (task);
  return 0;
}
