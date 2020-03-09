/**
 * @file   FIFO.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2020-03-09 (Last edit)
 * @brief  FIFO-module
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

/**
 * Push an item to the FIFO
 *
 * @param fifo Pointer to FIFO.
 * @param item Pointer to item to push.
 *
 * @return Status of push. False if FIFO is full, otherwise true.
 */
bool FIFO_Push(fifo_type *fifo, const void *item);

/**
 * Get and remove the first item in the FIFO.
 *
 * @param fifo Pointer to FIFO.
 * @param item Pointer to item where the new item will be stored.
 *
 * @return Status of pop. False if FIFO is empty, otherwise true.
 */
bool FIFO_Pop(fifo_type *fifo, void *item);

/**
 * Get the first item in the FIFO without removing it.
 *
 * @param fifo Pointer to FIFO.
 * @param item Pointer to item where the new item will be stored.
 *
 * @return Status of peek. False if FIFO is empty, otherwise true.
 */
bool FIFO_Peek(const fifo_type *fifo, void *item);

/**
 * Check if FIFO is full.
 *
 * @param fifo Pointer to FIFO.
 *
 * @return True if FIFO is full, otherwise false.
 */
bool FIFO_IsFull(const fifo_type *fifo);

/**
 * Check if FIFO is empty.
 *
 * @param fifo Pointer to FIFO.
 *
 * @return True if FIFO is empty, otherwise false.
 */
bool FIFO_IsEmpty(const fifo_type *fifo);

/**
 * Reset FIFO.
 *
 * @param fifo Pointer to FIFO.
 */
void FIFO_Clear(fifo_type *fifo);

#endif
