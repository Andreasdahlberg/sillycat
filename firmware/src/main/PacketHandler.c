/**
 * @file   PacketHandler.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-03-04 (Last edit)
 * @brief  Implementation of a node packet handler.
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
#include "PacketHandler.h"
#include "Nodes.h"
#include "Node.h"
#include "Com.h"
#include "ErrorHandler.h"
#include "libDebug.h"
#include <string.h>

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

static void SendAck(uint8_t target);

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

bool PacketHandler_HandleReadingPacket(packet_frame_type *packet_p)
{
    sc_assert(packet_p != NULL);

    struct node_t *node_p = Nodes_GetNodeFromID(packet_p->header.source);

    if (node_p != NULL)
    {
        DEBUG("Reading packet from 0x%02x\r\n", Node_GetID(node_p));

        Node_ReportActivity(node_p);
        Node_SetRSSI(node_p, packet_p->header.rssi);
        Node_Update(node_p, packet_p->content.data, (size_t)packet_p->content.size);
        SendAck(Node_GetID(node_p));

        return true;
    }
    else
    {
        /**
         * TODO: The current implementation only accept predefined node IDs.
         *       Register new nodes here.
         */
        WARNING("Received packet from unknown node: %x", packet_p->header.source);
        return false;
    }
}

bool PacketHandler_HandleBatteryPacket(packet_frame_type *packet_p)
{
    sc_assert(packet_p != NULL);

    struct node_t *node_p = Nodes_GetNodeFromID(packet_p->header.source);

    if (node_p != NULL)
    {
        DEBUG("Battery packet from 0x%02x\r\n", Node_GetID(node_p));

        uint16_t battery_voltage;

        memcpy(&battery_voltage, packet_p->content.data, sizeof(battery_voltage));
        Node_SetBatteryVoltage(node_p, battery_voltage);

        Node_ReportActivity(node_p);
        Node_SetRSSI(node_p, packet_p->header.rssi);

        return true;
    }
    else
    {
        /**
         * TODO: The current implementation only accept predefined node IDs.
         *       Register new nodes here.
         */
        WARNING("Received packet from unknown node: %x", packet_p->header.source);
        return false;
    }
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static void SendAck(uint8_t target)
{
    rtc_time_type timestamp;
    if (RTC_GetCurrentTime(&timestamp))
    {
        Com_Send(target, COM_PACKET_TYPE_TIME, &timestamp,
                 sizeof(timestamp));
    }
    else
    {
        ErrorHandler_LogError(RTC_FAIL, 0);
        ERROR("Failed to get timestamp.");
    }

    return;
}
