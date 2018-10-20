/**
 * @file   Transceiver.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-10-20 (Last edit)
 * @brief  Header for Transceiver interface.
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

#ifndef TRANSCEIVER_H_
#define TRANSCEIVER_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include "libRFM69.h"
#include "RTC.h"
#include "Event.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define CONTENT_DATA_SIZE 20

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
    rtc_time_type timestamp;
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

void Transceiver_Init(void);
void Transceiver_Update(void);
bool Transceiver_ReceivePacket(packet_frame_type *packet);
bool Transceiver_SendPacket(uint8_t target, packet_content_type *content);
void Transceiver_EventHandler(const event_t *event);

#endif /* TRANSCEIVER_H_ */
