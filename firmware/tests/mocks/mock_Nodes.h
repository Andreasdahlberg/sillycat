/**
 * @file   mock_Nodes.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2020-03-13 (Last edit)
 * @brief  Mock functions for the nodes module.
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

#ifndef WRAP_NODES_H_
#define WRAP_NODES_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include <stdbool.h>
#include "Nodes.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

void __wrap_Nodes_Init(void) __attribute__((weak));
void __wrap_Nodes_Add(struct node_t* node_p) __attribute__((weak));
struct node_t *__wrap_Nodes_GetNodeFromID(uint8_t id) __attribute__((weak));

#endif
