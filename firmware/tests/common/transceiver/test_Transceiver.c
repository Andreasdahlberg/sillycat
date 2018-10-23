/**
 * @file   test_Transceiver.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-10-23 (Last edit)
 * @brief  Test suite for the Transceiver interface.
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

#include "libRFM69.h"
#include "Transceiver.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

static uint8_t network_id[6];
static char aes_key[17];

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static void PrepareTransceiverInitMocks(void)
{
    expect_any(__wrap_libRFM69_SetMode, mode);
    will_return(__wrap_Config_GetNetworkId, &network_id);
    will_return(__wrap_Config_GetAddress, 1);
    will_return(__wrap_Config_GetBroadcastAddress, 255);
    will_return(__wrap_Config_GetAESKey, &aes_key);
}

static void PrepareSendingState(void)
{
    expect_any(__wrap_libRFM69_SetMode, mode);
    Transceiver_Update();

    will_return(__wrap_libRFM69_IsPayloadReady, false);
    will_return(__wrap_libRFM69_IsRxTimeoutFlagSet, false);
    will_return(__wrap_FIFO_IsEmpty, false);
    Transceiver_Update();
}

static int Setup(void **state)
{
    PrepareTransceiverInitMocks();
    Transceiver_Init();

    return 0;
}

static int SetupSending(void **state)
{
    Setup(state);
    PrepareSendingState();

    return 0;
}

//////////////////////////////////////////////////////////////////////////
//TESTS
//////////////////////////////////////////////////////////////////////////

static void test_Transceiver_Init(void **state)
{
    PrepareTransceiverInitMocks();
    Transceiver_Init();
}

static void test_Transceiver_ReceivePacket_NULL(void **state)
{
    expect_assert_failure(Transceiver_ReceivePacket(NULL));
}

static void test_Transceiver_ReceivePacket_NoPacket(void **state)
{
    will_return(__wrap_FIFO_Pop, false);

    packet_frame_type packet;
    assert_false(Transceiver_ReceivePacket(&packet));
}

static void test_Transceiver_ReceivePacket(void **state)
{
    will_return(__wrap_FIFO_Pop, true);

    packet_frame_type packet;
    assert_true(Transceiver_ReceivePacket(&packet));
}

static void test_Transceiver_SendPacket_InvalidArguments(void **state)
{
    const packet_content_type packet;

    expect_assert_failure(Transceiver_SendPacket(0, NULL));
    expect_assert_failure(Transceiver_SendPacket(1, NULL));
    expect_assert_failure(Transceiver_SendPacket(0, &packet));
}

static void test_Transceiver_SendPacket_InvalidSize(void **state)
{
    const uint8_t target = 1;
    const packet_content_type packet = {.size = sizeof(packet.data) + 1};

    will_return_maybe(__wrap_FIFO_IsFull, false);
    assert_false(Transceiver_SendPacket(target, &packet));
}

static void test_Transceiver_SendPacket_FullFIFO(void **state)
{
    const uint8_t target = 1;
    const packet_content_type packet = {0};

    will_return(__wrap_FIFO_IsFull, true);
    assert_false(Transceiver_SendPacket(target, &packet));
}

static void test_Transceiver_SendPacket(void **state)
{
    const uint8_t target = 1;
    const uint8_t address = 2;
    const packet_content_type packet = {0};

    will_return(__wrap_FIFO_IsFull, false);
    will_return(__wrap_Config_GetAddress, address);
    will_return(__wrap_FIFO_Push, true);

    assert_true(Transceiver_SendPacket(target, &packet));
}

static void test_Transceiver_EventHandler_NULL(void **state)
{
    expect_assert_failure(Transceiver_EventHandler(NULL));
}

static void test_Transceiver_EventHandler_Sleep(void **state)
{
    const event_t dummy_event;

    will_return(__wrap_Event_GetId, EVENT_SLEEP);
    will_return_maybe(__wrap_libRFM69_IsPayloadReady, false);
    will_return_maybe(__wrap_FIFO_IsEmpty, true);
    expect_value(__wrap_libRFM69_SetMode, mode, RFM_SLEEP);
    Transceiver_EventHandler(&dummy_event);
}

static void test_Transceiver_EventHandler_WakeUp(void **state)
{
    const event_t dummy_event;

    will_return(__wrap_Event_GetId, EVENT_WAKEUP);
    expect_value(__wrap_libRFM69_SetMode, mode, RFM_STANDBY);
    Transceiver_EventHandler(&dummy_event);
}

static void test_Transceiver_EventHandler_Unknown(void **state)
{
    const event_t dummy_event;

    will_return(__wrap_Event_GetId, UINT8_MAX);
    Transceiver_EventHandler(&dummy_event);
}

static void test_Transceiver_Update_Listening(void **state)
{
    expect_value(__wrap_libRFM69_SetMode, mode, RFM_RECEIVER);
    Transceiver_Update();

    will_return(__wrap_libRFM69_IsPayloadReady, false);
    will_return(__wrap_libRFM69_IsRxTimeoutFlagSet, false);
    will_return(__wrap_FIFO_IsEmpty, true);
    Transceiver_Update();
}

static void test_Transceiver_Update_PayloadReady(void **state)
{
    expect_value(__wrap_libRFM69_SetMode, mode, RFM_RECEIVER);
    Transceiver_Update();

    will_return(__wrap_libRFM69_IsPayloadReady, true);
    expect_value(__wrap_libRFM69_SetMode, mode, RFM_STANDBY);

    /* This return value is not used right now. */
    will_return_always(__wrap_libRFM69_ReadFromFIFO, 0);
    will_return(__wrap_libRFM69_GetRSSI, -10);
    will_return(__wrap_FIFO_Push, true);

    Transceiver_Update();
}

static void test_Transceiver_Update_PayloadReadyInvalidSize(void **state)
{
    skip();
}

static void test_Transceiver_Update_PayloadReadyFullFIFO(void **state)
{
    skip();
}

static void test_Transceiver_Update_RxTimeout(void **state)
{
    expect_value(__wrap_libRFM69_SetMode, mode, RFM_RECEIVER);
    Transceiver_Update();

    will_return(__wrap_libRFM69_IsPayloadReady, false);
    will_return(__wrap_libRFM69_IsRxTimeoutFlagSet, true);
    expect_function_call(__wrap_libRFM69_RestartRx);
    Transceiver_Update();
}

static void test_Transceiver_Update_PacketToSend(void **state)
{
    expect_value(__wrap_libRFM69_SetMode, mode, RFM_RECEIVER);
    Transceiver_Update();

    will_return(__wrap_libRFM69_IsPayloadReady, false);
    will_return(__wrap_libRFM69_IsRxTimeoutFlagSet, false);
    will_return(__wrap_FIFO_IsEmpty, false);
    Transceiver_Update();

    /* Start sending state machine. */
    expect_value(__wrap_libRFM69_SetMode, mode, RFM_STANDBY);
    Transceiver_Update();
}

static void test_Transceiver_Update_SendingInit(void **state)
{
    expect_value(__wrap_libRFM69_SetMode, mode, RFM_STANDBY);
    Transceiver_Update();
}

static void test_Transceiver_Update_SendingModeNotReady(void **state)
{
    /* Change to writing state. */
    expect_any(__wrap_libRFM69_SetMode, mode);
    Transceiver_Update();

    will_return(__wrap_libRFM69_IsModeReady, false);
    Transceiver_Update();

    /* Call update again to make sure that the state has not changed. */
    will_return(__wrap_libRFM69_IsModeReady, false);
    Transceiver_Update();
}

static void test_Transceiver_Update_SendingNoPacket(void **state)
{
    /* Change to writing state. */
    expect_any(__wrap_libRFM69_SetMode, mode);
    Transceiver_Update();

    will_return(__wrap_libRFM69_IsModeReady, true);
    will_return(__wrap_FIFO_Pop, false);
    Transceiver_Update();

    /**
     * Call update so that we can determine if the transmit sequence was
     * aborted by checking if the transceiver is set to receive mode.
     */
    expect_value(__wrap_libRFM69_SetMode, mode, RFM_RECEIVER);
    Transceiver_Update();
}

static void test_Transceiver_Update_SendingPacket(void **state)
{
    /* Change to writing state. */
    expect_any(__wrap_libRFM69_SetMode, mode);
    Transceiver_Update();

    will_return(__wrap_libRFM69_IsModeReady, true);
    will_return(__wrap_FIFO_Pop, true);
    /* These return values are not used right now. */
    will_return_always(__wrap_libRFM69_WriteToFIFO, 0);
    expect_value(__wrap_libRFM69_SetMode, mode, RFM_TRANSMITTER);
    Transceiver_Update();

    /**
     * Call twice to make sure that the state is unchanged if the packet is
     * not sent.
     */
    will_return(__wrap_libRFM69_IsPacketSent, false);
    Transceiver_Update();
    will_return(__wrap_libRFM69_IsPacketSent, true);
    Transceiver_Update();

    /**
     * Call update so that we can determine if the transmit sequence is done
     * and the listening sequence is started by checking if the transceiver is
     * set to receive mode.
     */
    expect_value(__wrap_libRFM69_SetMode, mode, RFM_RECEIVER);
    Transceiver_Update();
}

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test(test_Transceiver_Init),
        cmocka_unit_test_setup(test_Transceiver_ReceivePacket_NULL, Setup),
        cmocka_unit_test_setup(test_Transceiver_ReceivePacket_NoPacket, Setup),
        cmocka_unit_test_setup(test_Transceiver_ReceivePacket, Setup),
        cmocka_unit_test_setup(test_Transceiver_SendPacket_InvalidArguments, Setup),
        cmocka_unit_test_setup(test_Transceiver_SendPacket_InvalidSize, Setup),
        cmocka_unit_test_setup(test_Transceiver_SendPacket_FullFIFO, Setup),
        cmocka_unit_test_setup(test_Transceiver_SendPacket, Setup),
        cmocka_unit_test_setup(test_Transceiver_EventHandler_NULL, Setup),
        cmocka_unit_test_setup(test_Transceiver_EventHandler_Sleep, Setup),
        cmocka_unit_test_setup(test_Transceiver_EventHandler_WakeUp, Setup),
        cmocka_unit_test_setup(test_Transceiver_EventHandler_Unknown, Setup),
        cmocka_unit_test_setup(test_Transceiver_Update_Listening, Setup),
        cmocka_unit_test_setup(test_Transceiver_Update_PayloadReady, Setup),
        cmocka_unit_test_setup(test_Transceiver_Update_PayloadReadyInvalidSize, Setup),
        cmocka_unit_test_setup(test_Transceiver_Update_PayloadReadyFullFIFO, Setup),
        cmocka_unit_test_setup(test_Transceiver_Update_RxTimeout, Setup),
        cmocka_unit_test_setup(test_Transceiver_Update_PacketToSend, Setup),
        cmocka_unit_test_setup(test_Transceiver_Update_SendingInit, SetupSending),
        cmocka_unit_test_setup(test_Transceiver_Update_SendingModeNotReady, SetupSending),
        cmocka_unit_test_setup(test_Transceiver_Update_SendingNoPacket, SetupSending),
        cmocka_unit_test_setup(test_Transceiver_Update_SendingPacket, SetupSending)
    };

    if (argc >= 2)
    {
        cmocka_set_test_filter(argv[1]);
    }

    return cmocka_run_group_tests(tests, NULL, NULL);
}
