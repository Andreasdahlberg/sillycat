/**
 * @file   Nodes.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2015-11-17 (Last edit)
 * @brief  Implementation of Nodes
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

#include "libDebug.h"
#include "Nodes.h"
#include "guiNodes.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define MAX_NR_NODES 3

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

static packet_frame_type packets[MAX_NR_NODES];

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

static bool ReadingPacketHandler(packet_frame_type *packet);

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void Nodes_Init(void)
{
    memset(packets, 0, sizeof(packets));

    if (Transceiver_SetPacketHandler(ReadingPacketHandler,
                                     TR_PACKET_TYPE_READING) == TRUE)
    {
        guiNodes_Init();
    }
    return;
}

packet_frame_type *Nodes_GetLatestData(uint8_t index)
{
    //NOTE: Using total_size to determine if packet contains any data
    if (index < MAX_NR_NODES && packets[index].header.total_size > 0)
    {
        return &packets[index];
    }

    return NULL;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static bool ReadingPacketHandler(packet_frame_type *packet)
{
    dht22_data_type reading;

    DEBUG("Packet handled\r\n");

    //TODO: Use source address to determine index in packets
    memcpy(&packets[0], packet, sizeof(packet_frame_type));

    memcpy(&reading, &packet->content.data, sizeof(dht22_data_type));


    //DEBUG("Temp: %u, ", (uint32_t)reading.temperature);
    //DEBUG("Hum: %u\r\n", (uint32_t)reading.humidity);
    //NOTE: Return false so no ACK is sent
    return FALSE;
}