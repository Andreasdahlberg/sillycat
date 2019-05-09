/**
 * @file   Nodes.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2019-05-09 (Last edit)
 * @brief  Implementation of an interface for handling a collection of nodes.
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

#include "common.h"
#include "Nodes.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define MAX_NUMBER_OF_NODES 3

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

struct module_t
{
    struct node_t *nodes[MAX_NUMBER_OF_NODES];
    size_t number_of_nodes;
};

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static struct module_t module;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void Nodes_Init(void)
{
    module.number_of_nodes = 0;
}

void Nodes_Add(struct node_t *node_p)
{
    sc_assert(node_p != NULL);
    sc_assert(module.number_of_nodes < ElementsIn(module.nodes));

    module.nodes[module.number_of_nodes] = node_p;
    ++module.number_of_nodes;
}

struct node_t *Nodes_GetNodeFromID(uint8_t id)
{
    sc_assert(module.number_of_nodes <= ElementsIn(module.nodes));

    for (size_t i = 0; i < module.number_of_nodes; ++i)
    {
        if (Node_GetID(module.nodes[i]) == id)
        {
            return module.nodes[i];
        }
    }

    return NULL;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////
