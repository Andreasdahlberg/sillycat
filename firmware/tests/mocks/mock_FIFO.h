/**
 * @file   mock_FIFO.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-10-21 (Last edit)
 * @brief  Mock functions for the FIFO module.
 */

/*
This file is part of SillyCat firmware.

SillyCat firmware is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

SillyCat firmware is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with SillyCat firmware.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef WRAP_EVENT_H_
#define WRAP_EVENT_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include <stdbool.h>
#include "FIFO.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

bool __wrap_FIFO_IsEmpty(fifo_type *fifo);
void __wrap_FIFO_Clear(fifo_type *fifo);
bool __wrap_FIFO_Pop(fifo_type *fifo, void *item);
bool __wrap_FIFO_Peek(fifo_type *fifo, void *item);
bool __wrap_FIFO_IsFull(fifo_type *fifo);
bool __wrap_FIFO_Push(fifo_type *fifo, void *item);

#endif
