/**
 * @file   Transceiver.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2021-01-24 (Last edit)
 * @brief  Implementation of the Transceiver interface.
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

#ifndef TRANSCEIVER_H_
#define TRANSCEIVER_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include "Time.h"
#include "Event.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define CONTENT_DATA_SIZE 20
_Static_assert(CONTENT_DATA_SIZE <= UINT8_MAX, "Invalid packet data size!");

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

typedef struct
{
    uint8_t total_size;
    uint8_t target;
    uint8_t source;
    int8_t rssi;
} packet_header_type;

typedef struct
{
    struct time_t timestamp;
    uint8_t type;
    uint8_t size;
    uint8_t data[CONTENT_DATA_SIZE];
} packet_content_type;

typedef struct
{
    packet_header_type header;
    packet_content_type content;
} packet_frame_type;

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

/**
 * Initialize the transceiver.
 *
 * The transceiver hardware is configured with device specific parameters.
 */
void Transceiver_Init(void);

/**
 * Update the internal transceiver state machine.
 */
void Transceiver_Update(void);

/**
 * Receive a packet.
 *
 * @param packet_p Pointer to location where the packet will be stored.
 *
 * @return True if a packet was received, otherwise false.
 */
bool Transceiver_ReceivePacket(packet_frame_type *packet_p);

/**
 * Send a packet.
 *
 * The packet is added to the send queue and is sent when the transceiver
 * is ready.
 *
 * @param target    Address of target.
 * @param content_p Pointer to packet content. A complete packet with a header
 *                  will be created from this content.
 *
 * @return [description]
 */
bool Transceiver_SendPacket(uint8_t target, const packet_content_type *content_p);

/**
 * Handle events.
 *
 * The function currently handles the sleep and wakeup events. All other events
 * are silently ignored.
 *
 * @param event_p Pointer to triggered event.
 */
void Transceiver_EventHandler(const event_t *event_p);

#endif
