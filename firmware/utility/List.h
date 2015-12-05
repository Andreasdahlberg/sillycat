/**
 * @file   List.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2015-12-5 (Last edit)
 * @brief  Header of List
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


#ifndef LIST_H_
#define LIST_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stdbool.h>

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define List_New(type_id) (list_type){type_id, NULL};
#define List_NewNode(data_ptr) (list_node_type){data_ptr, NULL};

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

typedef struct list_node_type
{
	void *data;
    struct list_node_type *next;
}list_node_type;

typedef struct list_type
{
	uint32_t type_id;
	struct list_node_type *root;
}list_type;

typedef void (*list_fp)(list_node_type *node, uint32_t index);

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

bool List_Append(list_type *list, list_node_type *item);
bool List_Remove(list_type *list, list_node_type *item);
uint32_t List_GetLength(list_type *list);

#endif /* LIST_H_ */