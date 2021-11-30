/* exc.h -- This file is part of IEMU.
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

#ifndef __EXC_H
#define __EXC_H

enum exc_code
{
  exc_DE = 0,
  exc_DB,
  exc_BP = 3,
  exc_OF,
  exc_BR,
  exc_UD,
  exc_NM,
  exc_DF,
  exc_TS = 10,
  exc_NP,
  exc_SS,
  exc_GP,
  exc_PF,
  exc_MF = 16,
  exc_AC,
  exc_MC,
  exc_XM,
  exc_VE,
  exc_CP,
  exc_HV = 28,
  exc_VC,
  exc_SX
};

void exception (enum exc_code exc);

#endif
