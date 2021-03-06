/**
 * @file   test_Com.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2021-04-24 (Last edit)
 * @brief  Test suite for the Communications module.
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
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include <stdbool.h>

#include "ErrorHandler.h"
#include "Com.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

/**
 * A packet type number larger then the number of packet types is guaranteed
 * to be invalid.
 */
#define INVALID_PACKET_TYPE COM_PACKET_NR_TYPES

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//INTERUPT SERVICE ROUTINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static int Setup(void **state)
{
    Com_Init();

    return 0;
}

static bool FakePacketHandlerOne(const packet_frame_type *packet_p)
{
    assert_non_null(packet_p);
    check_expected(packet_p->content.type);
}

static bool FakePacketHandlerTwo(const packet_frame_type *packet_p)
{
    assert_non_null(packet_p);
    check_expected(packet_p->content.type);
}

static void ReceiveMockPacket(packet_frame_type *packet_p, uint8_t type)
{
    packet_p->content.type = type;

    will_return(__wrap_Transceiver_ReceivePacket, packet_p);
    will_return(__wrap_Transceiver_ReceivePacket, true);
}

static void AssertTimestampEqual(struct time_t *a_p, struct time_t *b_p)
{
    assert_int_equal(a_p->year, b_p->year);
    assert_int_equal(a_p->month, b_p->month);
    assert_int_equal(a_p->date, b_p->date);
    assert_int_equal(a_p->hour, b_p->hour);
    assert_int_equal(a_p->minute, b_p->minute);
    assert_int_equal(a_p->second, b_p->second);
}

//////////////////////////////////////////////////////////////////////////
//TESTS
//////////////////////////////////////////////////////////////////////////

static void test_Com_Init(void **state)
{
    /* Skip this test since their is no way to verify if this works. */
    skip();

    Com_Init();
}

static void test_Com_Update_NoPacket(void **state)
{
    will_return(__wrap_Transceiver_ReceivePacket, NULL);
    will_return(__wrap_Transceiver_ReceivePacket, false);
    Com_Update();
}

static void test_Com_Update_ReceivePacket(void **state)
{
    const uint8_t packet_type = 0;
    packet_frame_type mock_packet;

    /* Receive packet when no packet handler is set. */
    ReceiveMockPacket(&mock_packet, packet_type);
    Com_Update();

    /* Receive packet when a packet handler is set. */
    expect_value(FakePacketHandlerOne, packet_p->content.type, packet_type);
    ReceiveMockPacket(&mock_packet, packet_type);
    Com_SetPacketHandler(FakePacketHandlerOne, packet_type);
    Com_Update();
}

static void test_Com_Update_ReceiveInvalidPacket(void **state)
{
    /* Skip this test since their is no way to verify if this works. */
    skip();

    packet_frame_type mock_packet;

    /* Receive packet with an invalid packet type. */
    ReceiveMockPacket(&mock_packet, INVALID_PACKET_TYPE);
    Com_Update();
}

static void test_Com_SetPacketHandler_InvalidType(void **state)
{
    expect_assert_failure(Com_SetPacketHandler(FakePacketHandlerOne,
                          INVALID_PACKET_TYPE));
}

static void test_Com_SetPacketHandler(void **state)
{
    const uint8_t packet_type_one = 0;
    const uint8_t packet_type_two = COM_PACKET_NR_TYPES - 1;
    packet_frame_type mock_packet;

    Com_SetPacketHandler(FakePacketHandlerOne, packet_type_one);
    Com_SetPacketHandler(FakePacketHandlerTwo, packet_type_two);

    expect_value(FakePacketHandlerOne, packet_p->content.type, packet_type_one);
    ReceiveMockPacket(&mock_packet, packet_type_one);
    Com_Update();

    expect_value(FakePacketHandlerTwo, packet_p->content.type, packet_type_two);
    ReceiveMockPacket(&mock_packet, packet_type_two);
    Com_Update();
}

static void test_Com_Send_InvalidArguments(void **state)
{
    const uint8_t target = 1;
    const uint8_t packet_type = 0;
    const uint8_t fake_data;

    /* Invalid target. */
    expect_assert_failure(Com_Send(0,
                                   packet_type,
                                   &fake_data,
                                   sizeof(fake_data)
                                  ));

    /* Invalid packet type. */
    expect_assert_failure(Com_Send(target,
                                   INVALID_PACKET_TYPE,
                                   &fake_data,
                                   sizeof(fake_data)
                                  ));

    /* Invalid data pointer. */
    expect_assert_failure(Com_Send(target,
                                   packet_type,
                                   NULL,
                                   sizeof(fake_data)
                                  ));

    /* Invalid data size. */
    expect_assert_failure(Com_Send(target,
                                   packet_type,
                                   &fake_data,
                                   CONTENT_DATA_SIZE + 1
                                  ));
}

static void test_Com_Send_RTCFailure(void **state)
{
    const uint8_t target = 1;
    const uint8_t packet_type = 0;
    const uint8_t fake_data;
    packet_content_type packet_content;

    will_return(__wrap_RTC_GetCurrentTime, false);
    will_return(__wrap_Transceiver_SendPacket, &packet_content);
    will_return(__wrap_Transceiver_SendPacket, true);
    expect_value(__wrap_Transceiver_SendPacket, target, target);
    expect_value(__wrap_ErrorHandler_LogError, code, RTC_FAIL);

    Com_Send(target, packet_type, &fake_data, sizeof(fake_data));

    /**
     * All fields in the timestamp should be zero to indicate that the
     * timestamp is invalid.
     */
    struct time_t timestamp = {0};
    AssertTimestampEqual(&packet_content.timestamp, &timestamp);
}

static void test_Com_Send_SendFailure(void **state)
{
    /* Skip this test since their is no way to verify if this works. */
    skip();

    const uint8_t target = 1;
    const uint8_t packet_type = 0;
    const uint8_t fake_data;
    packet_content_type packet_content;

    will_return(__wrap_RTC_GetCurrentTime, true);
    will_return(__wrap_Transceiver_SendPacket, &packet_content);
    will_return(__wrap_Transceiver_SendPacket, false);
    expect_value(__wrap_Transceiver_SendPacket, target, target);

    Com_Send(target, packet_type, &fake_data, sizeof(fake_data));
}

static void test_Com_Send(void **state)
{
    const uint8_t target = 2;
    const uint8_t packet_type = 3;
    const uint8_t fake_data[4] = {0xAA, 0xFF, 0x00, 0xBB};
    packet_content_type packet_content;

    will_return(__wrap_RTC_GetCurrentTime, true);
    will_return(__wrap_Transceiver_SendPacket, &packet_content);
    will_return(__wrap_Transceiver_SendPacket, true);
    expect_value(__wrap_Transceiver_SendPacket, target, target);

    Com_Send(target, packet_type, fake_data, sizeof(fake_data));

    assert_int_equal(packet_content.type, packet_type);
    assert_int_equal(packet_content.size, sizeof(fake_data));
    assert_memory_equal(packet_content.data, fake_data, sizeof(fake_data));

    /* TODO: Verify timestamp. */
}

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test(test_Com_Init),
        cmocka_unit_test_setup(test_Com_Update_NoPacket, Setup),
        cmocka_unit_test_setup(test_Com_Update_ReceivePacket, Setup),
        cmocka_unit_test_setup(test_Com_Update_ReceiveInvalidPacket, Setup),
        cmocka_unit_test_setup(test_Com_SetPacketHandler_InvalidType, Setup),
        cmocka_unit_test_setup(test_Com_SetPacketHandler, Setup),
        cmocka_unit_test_setup(test_Com_Send_InvalidArguments, Setup),
        cmocka_unit_test_setup(test_Com_Send_RTCFailure, Setup),
        cmocka_unit_test_setup(test_Com_Send_SendFailure, Setup),
        cmocka_unit_test_setup(test_Com_Send, Setup),
    };

    if (argc >= 2)
    {
        cmocka_set_test_filter(argv[1]);
    }

    return cmocka_run_group_tests(tests, NULL, NULL);
}
