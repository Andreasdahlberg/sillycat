/**
 * @file   test_PacketHandler.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2020-03-14 (Last edit)
 * @brief  Test suite for the Packet handler module.
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
#include <stdbool.h>

#include "PacketHandler.h"
#include "Node.h"
#include "ErrorHandler.h"
#include "Com.h"

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

void FillPacket(packet_frame_type *packet_p, uint8_t source, int8_t rssi);

//////////////////////////////////////////////////////////////////////////
//INTERUPT SERVICE ROUTINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void FillPacket(packet_frame_type *packet_p, uint8_t source, int8_t rssi)
{
    *packet_p = (__typeof__(*packet_p)) {0};

    packet_p->header.source = source;
    packet_p->header.rssi = rssi;
}

//////////////////////////////////////////////////////////////////////////
//TESTS
//////////////////////////////////////////////////////////////////////////

static void test_PacketHandler_HandleReadingPacket_NULL(void **state)
{
    expect_assert_failure(PacketHandler_HandleReadingPacket(NULL));
}


static void test_PacketHandler_HandleReadingPacket_UnknownSourceNode(void **state)
{
    packet_frame_type packet;

    expect_any(__wrap_Nodes_GetNodeFromID, id);
    will_return_always(__wrap_Nodes_GetNodeFromID, NULL);

    assert_false(PacketHandler_HandleReadingPacket(&packet));
}

static void test_PacketHandler_HandleReadingPacket_RTCFailure(void **state)
{
    packet_frame_type packet;
    struct node_t node;

    expect_any(__wrap_Nodes_GetNodeFromID, id);
    will_return_always(__wrap_Nodes_GetNodeFromID, &node);

    const uint8_t source_id = 1;
    will_return_always(__wrap_Node_GetID, source_id);

    expect_function_call(__wrap_Node_ReportActivity);
    expect_any(__wrap_Node_SetRSSI, rssi);
    expect_function_call(__wrap_Node_Update);

    will_return_always(__wrap_RTC_GetCurrentTime, false);
    expect_value(__wrap_ErrorHandler_LogError, code, RTC_FAIL);

    assert_true(PacketHandler_HandleReadingPacket(&packet));
}

static void test_PacketHandler_HandleReadingPacket(void **state)
{
    packet_frame_type packet;
    struct node_t node;

    expect_any(__wrap_Nodes_GetNodeFromID, id);
    will_return_always(__wrap_Nodes_GetNodeFromID, &node);

    const uint8_t source_id = 1;
    will_return_always(__wrap_Node_GetID, source_id);

    expect_function_call(__wrap_Node_ReportActivity);
    expect_any(__wrap_Node_SetRSSI, rssi);
    expect_function_call(__wrap_Node_Update);

    will_return_always(__wrap_RTC_GetCurrentTime, true);
    expect_value(__wrap_Com_Send, target, source_id);
    expect_value(__wrap_Com_Send, packet_type, COM_PACKET_TYPE_TIME);

    assert_true(PacketHandler_HandleReadingPacket(&packet));
}

int main(void)
{
    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test(test_PacketHandler_HandleReadingPacket_NULL),
        cmocka_unit_test(test_PacketHandler_HandleReadingPacket_UnknownSourceNode),
        cmocka_unit_test(test_PacketHandler_HandleReadingPacket_RTCFailure),
        cmocka_unit_test(test_PacketHandler_HandleReadingPacket)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
