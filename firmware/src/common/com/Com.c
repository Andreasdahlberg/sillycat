/**
 * @file   Com.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-10-31 (Last edit)
 * @brief  Implementation of the Communications module.
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
#include <string.h>
#include "libDebug.h"
#include "ErrorHandler.h"
#include "Timer.h"
#include "Time.h"
#include "RTC.h"
#include "Com.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

struct module_t
{
    struct
    {
        uint32_t sent;
        uint32_t received;
        uint32_t lost;
        uint32_t invalid;
    } statistics;
    com_packet_handler_t packet_handlers[COM_PACKET_NR_TYPES];
};

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static struct module_t module;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

static bool HandlePacket(packet_frame_type *packet);

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void Com_Init(void)
{
    module = (struct module_t) { {0}, {0}};
}

void Com_Update(void)
{
    packet_frame_type rx_packet;

    if (Transceiver_ReceivePacket(&rx_packet))
    {
        // We are not using the status here since ACKs are not implemented yet.
        HandlePacket(&rx_packet);
    }
}

void Com_SetPacketHandler(com_packet_handler_t packet_handler,
                          com_packet_type_t packet_type)
{
    sc_assert(packet_type < ElementsIn(module.packet_handlers));

    module.packet_handlers[packet_type] = packet_handler;
}

void Com_Send(uint8_t target, uint8_t packet_type, const void *data_p, size_t size)

{
    sc_assert(target != 0);
    sc_assert(packet_type < ElementsIn(module.packet_handlers));
    sc_assert(data_p != NULL);

    struct time_t timestamp;
    if (!RTC_GetCurrentTime(&timestamp))
    {
        // Log error and continue, an invalid timestamp is not critical.
        ErrorHandler_LogError(RTC_FAIL, 0);
        ERROR("Failed to get timestamp.");

        // Set all fields to zero so the receiver can detect the invalid
        // timestamp.
        timestamp = (struct time_t) {0};
    }

    // Construct the package.
    packet_content_type packet;
    packet.type = packet_type;
    packet.size = size;
    packet.timestamp = timestamp;
    memcpy(packet.data, data_p, packet.size);

    if (Transceiver_SendPacket(target, &packet))
    {
        DEBUG("Packet sent to 0x%02X. \r\n", target);
        ++module.statistics.sent;
    }
    else
    {
        ERROR("Failed to send packet.");
        ++module.statistics.lost;
    }
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static bool HandlePacket(packet_frame_type *packet)
{
    sc_assert(packet != NULL);

    bool status;
    if (packet->content.type >= ElementsIn(module.packet_handlers))
    {
        WARNING("Invalid packet type [%u:%u]", packet->header.source,
                packet->content.type);
        status = false;
        ++module.statistics.invalid;
    }
    else if (NULL == module.packet_handlers[packet->content.type])
    {
        INFO("No packet handler set [%u:%u]", packet->header.source,
             packet->content.type);
        status = false;
        ++module.statistics.received;
    }
    else
    {
        status = module.packet_handlers[packet->content.type](packet);
        ++module.statistics.received;
    }

    return status;
}
