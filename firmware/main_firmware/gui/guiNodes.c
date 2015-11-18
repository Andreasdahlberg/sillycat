/**
 * @file   guiNodes.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2015-11-18 (Last edit)
 * @brief  Implementation of guiNodes
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

//NOTE: Include before all other headers
#include "common.h"

#include <string.h>
#include <stdio.h>

#include "libDebug.h"
#include "libUI.h"

#include "Interface.h"
#include "Nodes.h"

#include "guiNodes.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define MAX_NR_NODE_VIEWS 3

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static struct view node_views[MAX_NR_NODE_VIEWS];

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

static void DrawNodeView(uint16_t context);

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void guiNodes_Init(void)
{
    uint8_t view_index;

    for (view_index = 0; view_index < MAX_NR_NODE_VIEWS; ++view_index)
    {
        node_views[view_index].draw_function = DrawNodeView;
        node_views[view_index].context = view_index;
        node_views[view_index].child = NULL;
        node_views[view_index].prev = NULL;
        node_views[view_index].next = NULL;
        node_views[view_index].parent = NULL;
        Interface_AddView(&node_views[view_index]);
    }

    return;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static void DrawNodeView(uint16_t context)
{
    //Add one to node index(context) since end users are more familiar
    //with indexing starting at 1.
    libUI_Print("%u", 0, 0, context + 1);

    if (Nodes_GetLatestData(context) != NULL)
    {
        libUI_Print("Temperature: 23", 20, 2);
        libUI_Print("Humidity: 34", 28, 16);
    }
    else
    {
        libUI_Print("Temperature: -", 20, 2);
        libUI_Print("Humidity: -", 28, 16);
        INFO("No valid data, context: %u", context);
    }
    return;
}