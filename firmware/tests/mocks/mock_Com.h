/**
 * @file   mock_Com.h
 * @Author Andreas Dahlberg
 * @date   2020-03-14
 * @brief  Mock functions for the Communications module.
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

#ifndef WRAP_NODE_H_
#define WRAP_NODE_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include "Com.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

void __wrap_Com_Init(void) __attribute__((weak));
void __wrap_Com_Update(void) __attribute__((weak));
void __wrap_Com_SetPacketHandler(com_packet_handler_t packet_handler, com_packet_type_t packet_type) __attribute__((weak));
void __wrap_Com_Send(uint8_t target, uint8_t packet_type, const void* data_p, size_t size) __attribute__((weak));

#endif
