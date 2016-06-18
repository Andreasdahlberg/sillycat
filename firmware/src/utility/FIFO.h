/**
 * @file   FIFO.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2016-05-16 (Last edit)
 * @brief  Header of FIFO
 *
 * Detailed description of file.
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


#ifndef FIFO_H_
#define FIFO_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stdbool.h>

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define FIFO_New(data) (fifo_type){(uint8_t *)data, 0, 0, (sizeof(data) / sizeof(*data)), sizeof(*data)}

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

typedef struct
{
    uint8_t *data;
    uint8_t head;
    uint8_t tail;
    uint8_t size;
    uint8_t item_size;
} fifo_type;

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

bool FIFO_Push(fifo_type *fifo, void *item);
bool FIFO_Pop(fifo_type *fifo, void *item);
bool FIFO_Peek(fifo_type *fifo, void *item);
bool FIFO_IsFull(fifo_type *fifo);
bool FIFO_IsEmpty(fifo_type *fifo);
#endif /* FIFO_H_ */