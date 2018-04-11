/**
 * @file   Com.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2017-06-06 (Last edit)
 * @brief  Header for the Communications module.
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

#ifndef COM_H_
#define COM_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include "Transceiver.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

typedef enum
{
    COM_PACKET_TYPE_ACK = 0,
    COM_PACKET_TYPE_DATA,
    COM_PACKET_TYPE_READING,
    COM_PACKET_TYPE_TIME,
    COM_PACKET_NR_TYPES
} com_packet_type_t;

typedef bool (*com_packet_handler_t)(packet_frame_type *packet);

typedef void (*com_sent_callback_t)(bool status);

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

void Com_Init(void);
void Com_Update(void);
void Com_SetPacketHandler(com_packet_handler_t packet_handler,
                          com_packet_type_t packet_type);
void Com_Send(uint8_t target, uint8_t packet_type, void *data,
              size_t size);

#endif /* COM_H_ */
