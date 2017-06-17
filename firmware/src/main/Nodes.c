/**
 * @file   Nodes.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2017-06-17 (Last edit)
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
#include "Timer.h"
#include "Nodes.h"
#include "Com.h"
#include "ErrorHandler.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define MAX_NR_NODES 3
#define ADDRESS_OFFSET 128
#define DATA_OUTDATED_LIMIT_MS 20000

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static packet_frame_type packets[MAX_NR_NODES];
static uint32_t last_update[MAX_NR_NODES];

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

static bool ReadingPacketHandler(packet_frame_type *packet);

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

///
/// @brief Init the Nodes module, resets all buffers and registers a packet
///        handler function.
///
/// @param  None
/// @return None
///
void Nodes_Init(void)
{
    memset(packets, 0, sizeof(packets));
    memset(last_update, 0, sizeof(last_update));

    Com_SetPacketHandler(ReadingPacketHandler, COM_PACKET_TYPE_READING);

    return;
}

///
/// @brief Get the latest data packet from a specific node.
///
/// @param  index Index of node to get data from.
/// @return packet_frame_type Pointer to the latest data packet, NULL if no data
///                           exists.
///
packet_frame_type *Nodes_GetLatestData(uint8_t index)
{
    sc_assert(index < MAX_NR_NODES);

    //NOTE: Using total_size to determine if packet contains any data
    if (packets[index].header.total_size > 0 &&
            Timer_TimeDifference(last_update[index]) < DATA_OUTDATED_LIMIT_MS)
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
    sc_assert(packet != NULL);

    if (packet->header.source >= ADDRESS_OFFSET)
    {
        uint8_t index = packet->header.source - ADDRESS_OFFSET;
        sc_assert(index < MAX_NR_NODES);

        memcpy(&packets[index], packet, sizeof(packet_frame_type));

        last_update[index] = Timer_GetMilliseconds();
    }

    rtc_time_type timestamp;
    if (RTC_GetCurrentTime(&timestamp))
    {
        Com_Send(packet->header.source, COM_PACKET_TYPE_TIME, &timestamp,
                 sizeof(timestamp));
    }
    else
    {
        ErrorHandler_LogError(RTC_FAIL, 0);
        ERROR("Failed to get timestamp.");
    }

    DEBUG("Packet handled\r\n");
    return false;
}
