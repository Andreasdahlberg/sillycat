/**
 * @file   mock_Transceiver.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-10-20 (Last edit)
 * @brief  Mock functions for the Transceiver module.
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

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include <stdint.h>
#include "mock_Transceiver.h"

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

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

bool __wrap_Transceiver_ReceivePacket(packet_frame_type *packet)
{
    packet_frame_type *mock_packet_p;
    mock_packet_p = mock_ptr_type(packet_frame_type *);

    if (mock_packet_p != NULL)
    {
        *packet = *mock_packet_p;
    }
    mock_type(bool);
}

bool __wrap_Transceiver_SendPacket(uint8_t target, packet_content_type *content)
{
    check_expected(target);

    packet_content_type *mock_packet_content_p;
    mock_packet_content_p = mock_ptr_type(packet_content_type *);

    if (mock_packet_content_p != NULL)
    {
        *mock_packet_content_p = *content;
    }

    mock_type(bool);
}

void __wrap_Transceiver_EventHandler(const event_type *event)
{
}

void __wrap_Transceiver_Init(void)
{
}

void __wrap_Transceiver_Update(void)
{
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

