/**
 * @file   List.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-09-25 (Last edit)
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

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

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

void List_Append(struct list_t *self_p, struct list_node_t *node_p)
{
    sc_assert(self_p != NULL);
    sc_assert(node_p != NULL);

    if (self_p->root == NULL)
    {
        self_p->root = node_p;
    }
    else
    {
        struct list_node_t *current_p;
        current_p = self_p->root;

        while (current_p->next != NULL)
        {
            current_p = current_p->next;
        }

        current_p->next = node_p;
    }
    node_p->next = NULL;
}

void List_Remove(struct list_t *self_p, struct list_node_t *node_p)
{
    sc_assert(self_p != NULL);
    sc_assert(node_p != NULL);

    struct list_node_t *curr_node_p;
    struct list_node_t *prev_node_p;

    curr_node_p = self_p->root;
    prev_node_p = NULL;

    while (curr_node_p != NULL )
    {
        if (curr_node_p == node_p && prev_node_p != NULL)
        {
            prev_node_p->next = curr_node_p->next;
            return;
        }
        else if (curr_node_p == node_p && prev_node_p == NULL)
        {
            self_p->root = curr_node_p->next;
            return;
        }

        prev_node_p = curr_node_p;
        curr_node_p = curr_node_p->next;
    }
}

uint32_t List_GetLength(struct list_t *self_p)
{
    sc_assert(self_p != NULL);

    uint32_t node_count = 0;
    struct list_node_t *current_p;
    current_p = self_p->root;

    while (current_p != NULL)
    {
        current_p = current_p->next;
        ++node_count;
    }

    return node_count;
}

void List_Iterate(struct list_t *self_p, list_fp function_p)
{
    sc_assert(self_p != NULL);
    sc_assert(function_p != NULL);

    uint32_t node_count = 0;
    struct list_node_t *current_p;
    current_p = self_p->root;

    while (current_p != NULL)
    {
        function_p(current_p, node_count);
        current_p = current_p->next;
        ++node_count;
    }
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////
