/**
 * @file   FIFO.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2020-03-09 (Last edit)
 * @brief  FIFO-module.
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

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include <string.h>
#include "sc_assert.h"
#include "FIFO.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

uint8_t NextPosition(const fifo_type *fifo, uint8_t current_position);

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

bool FIFO_Push(fifo_type *fifo, const void *item)
{
    sc_assert(fifo != NULL);
    sc_assert(item != NULL);

    uint8_t *ptr;

    if (FIFO_IsFull(fifo) == true)
    {
        return false;
    }

    //Move head one item forward
    fifo->head = NextPosition(fifo, fifo->head);
    ptr = fifo->data + fifo->head;

    memcpy(ptr, item, fifo->item_size);
    return true;
}

bool FIFO_Pop(fifo_type *fifo, void *item)
{
    sc_assert(fifo != NULL);
    sc_assert(item != NULL);

    const uint8_t *ptr;

    if (FIFO_IsEmpty(fifo) == true)
    {
        return false;
    }

    //Move tail one item forward
    fifo->tail = NextPosition(fifo, fifo->tail);
    ptr = fifo->data + fifo->tail;

    memcpy(item, ptr, fifo->item_size);
    return true;
}

bool FIFO_Peek(const fifo_type *fifo, void *item)
{
    sc_assert(fifo != NULL);
    sc_assert(item != NULL);

    const uint8_t *ptr;

    if (FIFO_IsEmpty(fifo) == true)
    {
        return false;
    }
    ptr = fifo->data + NextPosition(fifo, fifo->tail);

    memcpy(item, ptr, fifo->item_size);
    return true;
}

bool FIFO_IsFull(const fifo_type *fifo)
{
    sc_assert(fifo != NULL);

    return NextPosition(fifo, fifo->head) == fifo->tail;
}

bool FIFO_IsEmpty(const fifo_type *fifo)
{
    sc_assert(fifo != NULL);

    return fifo->head == fifo->tail;
}

void FIFO_Clear(fifo_type *fifo)
{
    sc_assert(fifo != NULL);

    fifo->head = 0;
    fifo->tail = 0;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

//TODO: Check typecasts, overflow?
uint8_t NextPosition(const fifo_type *fifo, uint8_t current_position)
{
    return (uint8_t)((uint16_t)(current_position + fifo->item_size) %
                     (uint16_t)(fifo->size * fifo->item_size));
}
