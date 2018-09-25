/**
 * @file   List.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-09-25 (Last edit)
 * @brief  Implementation of a single linked list
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

#include <stdio.h>
#include "List.h"
#include "sc_assert.h"

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

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

///
/// @brief Append a node to the list
///
/// @param  *list Pointer to list.
/// @param  *node Pointer to node.
/// @return None.
///
void List_Append(list_type *list, list_node_type *node)
{
    sc_assert(list != NULL);
    sc_assert(node != NULL);

    if (list->root == NULL)
    {
        list->root = node;
    }
    else
    {
        list_node_type *node_ptr;
        node_ptr = list->root;

        while (node_ptr->next != NULL)
        {
            node_ptr = node_ptr->next;
        }

        node_ptr->next = node;
    }
    node->next = NULL;
}

///
/// @brief Remove a node from the list
///
/// @param  *list Pointer to list.
/// @param  *node Pointer to node.
/// @return Status of remove.
///
void List_Remove(list_type *list, list_node_type *node)
{
    sc_assert(list != NULL);
    sc_assert(node != NULL);

    list_node_type *curr_node;
    list_node_type *prev_node;

    curr_node = list->root;
    prev_node = NULL;

    while (curr_node != NULL )
    {
        if (curr_node == node && prev_node != NULL)
        {
            prev_node->next = curr_node->next;
            return;
        }
        else if (curr_node == node && prev_node == NULL)
        {
            list->root = curr_node->next;
            return;
        }

        prev_node = curr_node;
        curr_node = curr_node->next;
    }
    return;
}

///
/// @brief Get length of the list
///
/// @param  *list Pointer to list.
/// @return Length of list
///
uint32_t List_GetLength(list_type *list)
{
    sc_assert(list != NULL);

    uint32_t node_count = 0;
    list_node_type *node_ptr;
    node_ptr = list->root;

    while (node_ptr != NULL)
    {
        node_ptr = node_ptr->next;
        ++node_count;
    }

    return node_count;
}

///
/// @brief Iterate over all nodes and call a function on each one.
///
/// @param  *list Pointer to list.
/// @param  *list Pointer to function with pointer to node and index as arguments.
/// @return None
///
void List_Iterate(list_type *list, list_fp function)
{
    sc_assert(list != NULL);
    sc_assert(function != NULL);

    uint32_t node_count = 0;
    list_node_type *node_ptr;
    node_ptr = list->root;

    while (node_ptr != NULL)
    {
        function(node_ptr, node_count);
        node_ptr = node_ptr->next;
        ++node_count;
    }
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////
