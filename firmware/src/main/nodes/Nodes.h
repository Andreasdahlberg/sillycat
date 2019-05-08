/**
 * @file   Nodes.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-03-04 (Last edit)
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

#ifndef NODES_H_
#define NODES_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include "Node.h"

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

/**
 * Initialize the node collection.
 */
void Nodes_Init(void);

/**
 * Add a node to the collection.
 *
 * @param node_p Pointer to node struct.
 */
void Nodes_Add(struct node_t *node_p);

/**
 * Get a node matching the supplied ID.
 *
 * @param id Node ID.
 *
 * @return Pointer to node struct if ID is valid, otherwise NULL.
 */
struct node_t *Nodes_GetNodeFromID(uint8_t id);

#endif
