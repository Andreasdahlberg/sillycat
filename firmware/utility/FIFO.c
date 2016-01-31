/**
 * @file   FIFO.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2016-01-31 (Last edit)
 * @brief  Implementation of a FIFO
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

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include <string.h>
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

uint8_t NextPosition(fifo_type *fifo, uint8_t current_position);

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

///
/// @brief Push an item to the FIFO
///
/// @param  *fifo Pointer to FIFO.
/// @param  *item Pointer to item to push.
/// @return Status of push. false if FIFO is full, otherwise true.
///
bool FIFO_Push(fifo_type *fifo, void *item)
{
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

///
/// @brief Get and remove the first item in the FIFO.
///
/// @param  *fifo Pointer to FIFO.
/// @param  *item Pointer to item where the new item will be stored.
/// @return Status of pop. false if FIFO is empty, otherwise true
///
bool FIFO_Pop(fifo_type *fifo, void *item)
{
    uint8_t *ptr;

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

///
/// @brief Get the first item in the FIFO without removing it.
///
/// @param  *fifo Pointer to FIFO.
/// @param  *item Pointer to item where the new item will be stored.
/// @return Status of pop. false if FIFO is empty, otherwise true
///
bool FIFO_Peek(fifo_type *fifo, void *item)
{
    uint8_t *ptr;

    if (FIFO_IsEmpty(fifo) == true)
    {
        return false;
    }
    ptr = fifo->data + NextPosition(fifo, fifo->tail);

    memcpy(item, ptr, fifo->item_size);
    return true;
}

///
/// @brief Check if FIFO is full.
///
/// @param  *fifo Pointer to FIFO.
/// @return true if FIFO is full, otherwise false.
///
bool FIFO_IsFull(fifo_type *fifo)
{
    return NextPosition(fifo, fifo->head) == fifo->tail;
}

///
/// @brief Check if FIFO is empty.
///
/// @param  *fifo Pointer to FIFO.
/// @return true if FIFO is empty, otherwise false.
///
bool FIFO_IsEmpty(fifo_type *fifo)
{
    return fifo->head == fifo->tail;
}

///
/// @brief Reset FIFO.
///
/// @param  *fifo Pointer to FIFO.
/// @return None
///
void FIFO_Clear(fifo_type *fifo)
{
    fifo->head = 0;
    fifo->tail = 0;
    return;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

//TODO: Check typecasts, overflow?
uint8_t NextPosition(fifo_type *fifo, uint8_t current_position)
{
    return (uint8_t)((uint16_t)(current_position + fifo->item_size) %
                     (uint16_t)(fifo->size * fifo->item_size));
}