/**
 * @file   guiNodes.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2016-02-07 (Last edit)
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

typedef struct
{
    float humidity;
    float temperature;
    bool status;
} dht22_data_type;

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

// Multiply with two to get an extra detailed view for each node view
static struct view node_views[MAX_NR_NODE_VIEWS * 2];

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

static void DrawNodeView(uint16_t context);
static void DrawDetailedNodeView(uint16_t context);

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

        sc_assert(view_index + MAX_NR_NODE_VIEWS < MAX_NR_NODE_VIEWS * 2);

        node_views[view_index + MAX_NR_NODE_VIEWS].draw_function =
            DrawDetailedNodeView;
        node_views[view_index + MAX_NR_NODE_VIEWS].context = view_index;
        node_views[view_index + MAX_NR_NODE_VIEWS].child = NULL;
        node_views[view_index + MAX_NR_NODE_VIEWS].prev = NULL;
        node_views[view_index + MAX_NR_NODE_VIEWS].next = NULL;
        node_views[view_index + MAX_NR_NODE_VIEWS].parent = NULL;
        Interface_AddChild(&node_views[view_index],
                           &node_views[view_index + MAX_NR_NODE_VIEWS]);
    }
    return;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static void DrawNodeView(uint16_t context)
{
    packet_frame_type *packet;

    //Add one to node index(context) since end users are more familiar
    //with indexing starting at 1.
    libUI_Print("%u", 0, 0, context + 1);

    packet = Nodes_GetLatestData(context);

    if (packet != NULL)
    {
        float_parts_type parts;

        parts = FloatToParts(((dht22_data_type *)packet->content.data)->temperature);
        libUI_Print("Temperature: %li.%uC", 8, 2, parts.integer, parts.fractional);

        parts = FloatToParts(((dht22_data_type *)packet->content.data)->humidity);
        libUI_Print("Humidity: %li.%u%%", 18, 16, parts.integer, parts.fractional);
    }
    else
    {
        libUI_Print("Temperature: -", 8, 2);
        libUI_Print("Humidity: -", 18, 16);
        INFO("No valid data, context: %u", context);
    }
    return;
}

static void DrawDetailedNodeView(uint16_t context)
{
    packet_frame_type *packet;
    packet = Nodes_GetLatestData(context);

    if (packet != NULL)
    {
        libUI_Print("RSSI: %d", 4, 2, packet->header.rssi);
        libUI_Print("Source: 0x%02X", 4, 16, packet->header.source);
    }
    else
    {
        libUI_Print("RSSI: -", 16, 2);
        libUI_Print("Source: -", 16, 16);
        INFO("No valid data, context: %u", context);
    }
    return;
}