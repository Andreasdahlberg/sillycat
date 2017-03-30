/**
 * @file   Interface.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2017-03-30 (Last edit)
 * @brief  Header of Interface
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

#ifndef INTERFACE_H_
#define INTERFACE_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include <stdint.h>

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

typedef void (*interface_fp)(uint16_t context);

struct view
{
    interface_fp draw_function;
    uint16_t context;
    struct view *parent;
    struct view *child;
    struct view *next;
    struct view *prev;
};

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

void Interface_Init(void);
void Interface_Update(void);
void Interface_Refresh(void);
void Interface_AddView(struct view *new_view);
void Interface_AddChild(struct view *parent_view, struct view *child_view);
void Interface_AddSibling(struct view *sibling_view, struct view *new_view);
void Interface_RemoveView(struct view *view);
void Interface_NextView(void);
void Interface_PreviousView(void);
void Interface_ActivateView(void);
struct view *Interface_GetRootView(void);
struct view *Interface_GetActiveView(void);

#endif /* INTERFACE_H_ */
