/**
 * @file   List.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2020-03-11 (Last edit)
 * @brief  Implementation of a single linked list.
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

#ifndef LIST_H_
#define LIST_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stdio.h>

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define List_New(type_id) (struct list_t){type_id, NULL};
#define List_NewNode(data_ptr) (struct list_node_t){data_ptr, NULL};

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

struct list_node_t
{
    void *data;
    struct list_node_t *next;
};

struct list_t
{
    uint32_t type_id;
    struct list_node_t *root;
};

typedef void (*list_fp)(struct list_node_t *node, uint32_t index);

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

/**
 * Append a node to the list.
 *
 * @param self_p Pointer to list.
 * @param node_p Pointer to node.
 */
void List_Append(struct list_t *self_p, struct list_node_t *node_p);

/**
 * Remove a node from the list.
 *
 * @param self_p Pointer to list.
 * @param node_p Pointer to node.
 */
void List_Remove(struct list_t *self_p, const struct list_node_t *node_p);

/**
 * Get length of the list
 *
 * @param self_p Pointer to list.
 *
 * @return Length of the list.
 */
uint32_t List_GetLength(const struct list_t *self_p);

/**
 * Iterate over all nodes and call a function on each one.
 *
 * @param self_p     Pointer to list.
 * @param function_p Pointer to function with pointer to node and index as
 *                   arguments.
 */
void List_Iterate(const struct list_t *self_p, list_fp function_p);

#endif
