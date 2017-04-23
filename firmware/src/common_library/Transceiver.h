/**
 * @file   Transceiver.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2017-04-23 (Last edit)
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

typedef void (*transceiver_callback_type)(bool status);

typedef enum
{
    TR_PACKET_TYPE_ACK = 0,
    TR_PACKET_TYPE_NACK,
    TR_PACKET_TYPE_READING,
    TR_PACKET_TYPE_TIME,
    TR_PACKET_NR_TYPES
} packet_type_type;

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
    packet_type_type type;
    uint8_t size;
    uint8_t data[CONTENT_DATA_SIZE];
} packet_content_type;

typedef struct
{
    packet_header_type header;
    packet_content_type content;
    transceiver_callback_type callback;
} packet_frame_type;

typedef bool (*transceiver_packet_handler_type)(packet_frame_type *packet);

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

void Transceiver_Init(void);
void Transceiver_Update(void);
bool Transceiver_Send(void);
bool Transceiver_SendPacket(uint8_t target,
                            packet_content_type *content,
                            transceiver_callback_type callback);
void Transceiver_SetPacketHandler(transceiver_packet_handler_type
                                  packet_handler,
                                  packet_type_type packet_type);
void Transceiver_EventHandler(const event_type *event);

#endif /* TRANSCEIVER_H_ */
