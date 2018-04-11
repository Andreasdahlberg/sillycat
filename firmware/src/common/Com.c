/**
 * @file   Com.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2017-06-06 (Last edit)
 * @brief  Implementation of the Communications module.
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

//NOTE: Include common.h before all other headers
#include "common.h"

#include <string.h>

#include "libDebug.h"

#include "Com.h"
#include "ErrorHandler.h"
#include "Timer.h"
#include "RTC.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

struct packet_statistics_t
{
    uint32_t sent;
    uint32_t received;
    uint32_t lost;
};

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static com_packet_handler_t packet_handlers[COM_PACKET_NR_TYPES];
static struct packet_statistics_t packet_stats;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

static void ClearPacketHandlers(void);
static bool HandlePacket(packet_frame_type *packet);

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

///
/// @brief Initialize the Communications module.
///
/// @param  None
///
/// @return None
///
void Com_Init(void)
{
    ClearPacketHandlers();

    // Reset packet statistics.
    packet_stats = (struct packet_statistics_t) {0};

    return;
}

///
/// @brief Update the internal module state.
///
/// @param  None
///
/// @return None
///
void Com_Update(void)
{
    packet_frame_type rx_packet;

    if (Transceiver_ReceivePacket(&rx_packet))
    {
        // We are not using the status here since ACKs are not implemented yet.
        HandlePacket(&rx_packet);

        ++packet_stats.received;
    }

    return;
}

///
/// @brief Register a packet handler for a specific packet type.
///
/// @param  *packet_handler Pointer to packet handler function. This function will
///                         be called when the corresponding packet type is received.
///                         NULL can be used to remove a packet handler.
/// @param  packet_type Packet type.
///
/// @return None
///
void Com_SetPacketHandler(com_packet_handler_t packet_handler,
                          com_packet_type_t packet_type)
{
    sc_assert(packet_type < ElementsIn(packet_handlers));

    packet_handlers[packet_type] = packet_handler;
    return;
}


///
/// @brief Send data to a another node.
///
/// @param  target Address of the target node.
/// @param  packet_type Packet type.
/// @param  *data Pointer to data to send.
/// @param  size Size of the data.
///
/// @return None
///
void Com_Send(uint8_t target, uint8_t packet_type, void *data,
              size_t size)

{
    sc_assert(target != 0);
    sc_assert(data != NULL);

    rtc_time_type timestamp;
    if (!RTC_GetCurrentTime(&timestamp))
    {
        // Log error and continue, an invalid timestamp is not critical.
        ErrorHandler_LogError(RTC_FAIL, 0);
        ERROR("Failed to get timestamp.");

        // Set all fields to zero so the receiver can detect the invalid
        // timestamp.
        timestamp = (rtc_time_type) {0};
    }

    // Construct the package.
    packet_content_type packet;
    packet.type = packet_type;
    packet.size = size;
    packet.timestamp = timestamp;
    memcpy(packet.data, data, packet.size);

    if (Transceiver_SendPacket(target, &packet))
    {
        DEBUG("Packet sent to 0x%02X. \r\n", target);
        ++packet_stats.sent;
    }
    else
    {
        ERROR("Failed to send packet.");
        ++packet_stats.lost;
    }

    return;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static void ClearPacketHandlers(void)
{
    for (size_t index = 0; index < ElementsIn(packet_handlers); ++index)
    {
        packet_handlers[index] = NULL;
    }

    return;
}

static bool HandlePacket(packet_frame_type *packet)
{
    sc_assert(packet != NULL);

    bool status;
    if (packet->content.type >= ElementsIn(packet_handlers))
    {
        WARNING("Invalid packet type [%u:%u]", packet->header.source,
                packet->content.type);
        status = false;
    }
    else if (NULL == packet_handlers[packet->content.type])
    {
        INFO("No packet handler set [%u:%u]", packet->header.source,
             packet->content.type);
        status = false;
    }
    else
    {
        status = packet_handlers[packet->content.type](packet);
    }

    return status;
}
