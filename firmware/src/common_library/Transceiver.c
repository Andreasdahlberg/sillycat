/**
 * @file   Transceiver.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2016-11-07 (Last edit)
 * @brief  Implementation of Transceiver interface.
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

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

//NOTE: Include before all other headers
#include "common.h"

#include <util/delay.h>
#include <string.h>

#include "libDebug.h"
#include "libRFM69.h"

#include "Timer.h"
#include "Transceiver.h"
#include "Config.h"
#include "ErrorHandler.h"

#include "RFM69Registers.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define ResetPacketFrame(packet_frame) packet_frame.header.target = 0

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

typedef enum
{
    TR_STATE_NO_INIT = 0,
    TR_STATE_LISTENING,
    TR_STATE_SENDING
} transceiver_state_type;

typedef enum
{
    TR_STATE_SENDING_INIT = 0,
    TR_STATE_SENDING_WRITING,
    TR_STATE_SENDING_TRANSMITTING,
    TR_STATE_SENDING_WAIT_FOR_ACK
} transceiver_sending_state_type;

typedef enum
{
    TR_STATE_LISTENING_INIT = 0,
    TR_STATE_LISTENING_WAITING,
    TR_STATE_LISTENING_RECEIVING,
    TR_STATE_LISTENING_SEND_ACK,
    TR_STATE_LISTENING_DONE,
} transceiver_listening_state_type;

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static transceiver_state_type transceiver_state = TR_STATE_NO_INIT;
static packet_frame_type packet_frame_tx;
static packet_frame_type packet_frame_rx;
static transceiver_packet_handler_type packet_handlers[TR_PACKET_NR_TYPES];


//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

static transceiver_state_type SendingStateMachine(void);
static transceiver_state_type ListeningStateMachine(void);
static bool IsPacketTypeValid(packet_type_type packet_type);
static bool IsActive(void);
static bool PacketToSend(void);

#ifdef DEBUG_ENABLE
static void DumpPacket(packet_frame_type *packet);
#endif

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

///
/// @brief Set the transceiver to an known state.
///
/// @param  None
/// @return None
///
void Transceiver_Init(void)
{
    libRFM69_Init();
    libRFM69_EnableEncryption(false);
    libRFM69_EnableSequencer(true);
    libRFM69_EnableListenMode(false);
    libRFM69_SetPacketRxDelay(1);
    libRFM69_SetMode(RFM_STANDBY);
    libRFM69_WaitForModeReady();
    libRFM69_SetAddressFiltering(RFM_ADDRESS_FILTER_ADDRESS_BROADCAST);
    libRFM69_SetPreambleLength(8);
    libRFM69_SetLNAGain(RFM_LNA_GAIN_AUTO);
    libRFM69_SetLNAInputImpedance(RFM_LNA_ZIN_50OHM);
    libRFM69_EnableAFCLowBeta(false);
    libRFM69_EnableContinuousDAGC(false);
    libRFM69_SetBitRate(4800);
    libRFM69_SetDataMode(RFM_PACKET_DATA);
    libRFM69_SetModulationType(RFM_FSK);
    libRFM69_SetModulationShaping(0x00);
    libRFM69_SetFrequencyDeviation(5000);
    libRFM69_SetCarrierFrequency(868000000);
    libRFM69_EnableSyncWordGeneration(true);
    libRFM69_SetFIFOFillCondition(RFM_FIFO_FILL_AUTO);
    libRFM69_SetRSSIThreshold(-25);
    libRFM69_SetSyncWordSize(6);
    libRFM69_SetSyncWord(Config_GetNetworkId(), 6);
    libRFM69_SetTXStartCondition(RFM_TX_START_NOT_EMPTY);
    libRFM69_SetNodeAddress(Config_GetNodeId());
    libRFM69_SetBroadcastAddress(BROADCAST_ADDRESS);
    libRFM69_SetAddressFiltering(RFM_ADDRESS_FILTER_ADDRESS_BROADCAST);
    libRFM69_ClearFIFO();
    libRFM69_SetPacketFormat(RFM_PACKET_VARIABLE_LEN);
    libRFM69_EnableCRC(true);
    libRFM69_EnableCRCAutoClear(true);
    libRFM69_SetRxTimeout(0);
    libRFM69_SetRSSIThresholdTimeout(850);
    libRFM69_SetClockOutFrequency(RFM_CLKOUT_OFF);
    libRFM69_EnableOCP(false);
    libRFM69_SetPowerAmplifierMode(RFM_PWR_3_4);
    libRFM69_SetPowerLevel(31);
    libRFM69_SetAESKey((uint8_t *)Config_GetAESKey());

    ResetPacketFrame(packet_frame_tx);
    ResetPacketFrame(packet_frame_rx);

    transceiver_state = TR_STATE_LISTENING;

    INFO("Transceiver initiated");
    return;
}

///
/// @brief Update the internal transceiver state machine.
///
/// @param  None
/// @return None
///
void Transceiver_Update(void)
{
    switch (transceiver_state)
    {
        case TR_STATE_LISTENING:
            transceiver_state = ListeningStateMachine();
            break;

        case TR_STATE_SENDING:
            transceiver_state = SendingStateMachine();
            break;

        default:
            sc_assert_fail();
            break;
    }
    return;
}

///
/// @brief Send packet.
///
/// The packet is added to the send queue and is sent when the transceiver is ready.
///
/// @param  target Address of target.
/// @param  request_ack True if ack from target is requested, otherwise false.
/// @param  *content Pointer to packet content. A complete packet with a header
///                 will be created from this content.
/// @param  *callback Pointer to callback function. This function will be called
///                  after the packet. If a ack is requested the callback will be
///                  called when the ack is received.
/// @return None
///
bool Transceiver_SendPacket(uint8_t target, bool request_ack,
                            packet_content_type *content,
                            transceiver_callback_type callback)
{
    sc_assert(content != NULL);
    sc_assert(target != 0);

    bool status = false;
    packet_frame_type *frame_ptr;
    frame_ptr = &packet_frame_tx;

    if (content->size <= CONTENT_DATA_SIZE)
    {
        status = true;

        frame_ptr->header.target = target;
        frame_ptr->header.source = Config_GetNodeId();
        frame_ptr->header.ack = request_ack;
        frame_ptr->header.rssi = libRFM69_GetRSSI();
        frame_ptr->header.total_size = sizeof(packet_header_type) + 8 +
                                       content->size; //TODO: sizeof or define!

        frame_ptr->content.timestamp = content->timestamp;
        frame_ptr->content.type = content->type;
        frame_ptr->content.size = content->size;
        memcpy(frame_ptr->content.data, content->data, content->size);
        frame_ptr->callback = callback;
    }

    return status;
}

///
/// @brief Register a packet handler for a specific packet type.
///
/// @param  *packet_handler Pointer to packet handler function. This function will
///                         be called when the corresponding packet type is received.
/// @param  packet_type Packet type.
/// @return None
///
void Transceiver_SetPacketHandler(transceiver_packet_handler_type packet_handler,
                                  packet_type_type packet_type)
{
    sc_assert(packet_type < TR_PACKET_NR_TYPES);

    packet_handlers[packet_type] = packet_handler;
    return;
}

///
/// @brief Handle events
///
/// @param  *event Pointer to triggered event
/// @return None
///
void Transceiver_EventHandler(const event_type *event)
{
    sc_assert(event != NULL);

    switch (event->id)
    {
        case EVENT_SLEEP:
            INFO("Entering sleep");

            //TODO: Measure worst case time waiting here
            while(IsActive() == true)
            {
                Transceiver_Update();
            }

            libRFM69_SetMode(RFM_SLEEP);
            libRFM69_WaitForModeReady();
            break;

        case EVENT_WAKEUP:
            INFO("Exiting sleep");
            libRFM69_SetMode(RFM_STANDBY);
            libRFM69_WaitForModeReady();
            break;

        default:
            //Do nothing if an unknown event is received
            break;
    }
    return;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static bool IsActive(void)
{
    return (transceiver_state == TR_STATE_SENDING ||
            libRFM69_IsPayloadReady() || PacketToSend());
}

static bool IsPacketTypeValid(packet_type_type packet_type)
{
    return (packet_type < TR_PACKET_NR_TYPES);
}

static bool PacketToSend(void)
{
    return (packet_frame_tx.header.target > 0);
}

static bool HandlePayload(void)
{
    uint8_t length;
    uint8_t data_buffer[RFM_FIFO_SIZE - 1];

    libRFM69_ReadFromFIFO(data_buffer, 1);
    length = data_buffer[0];

    if (length > RFM_FIFO_SIZE - 1)
    {
        ERROR("Size of data packet is larger then the FIFO");
        return false;
    }

    libRFM69_ReadFromFIFO(&data_buffer[1], length);
    memcpy(&packet_frame_rx.header, data_buffer, sizeof(packet_header_type));
    memcpy(&packet_frame_rx.content, &data_buffer[sizeof(packet_header_type)],
           sizeof(packet_content_type));

    //TODO: Fix this!
    //packet_frame_rx.header.rssi = libRFM69_GetRSSI();

#ifdef DEBUG_ENABLE
    DumpPacket(&packet_frame_rx);
#endif

    if (IsPacketTypeValid(packet_frame_rx.content.type) &&
            packet_handlers[packet_frame_rx.content.type] != NULL)
    {
        return packet_handlers[packet_frame_rx.content.type](&packet_frame_rx);
    }
    else
    {
        INFO("No packet handler for packet type %u");
        return false;
    }
}

static transceiver_state_type ListeningStateMachine(void)
{
    static transceiver_listening_state_type state = TR_STATE_LISTENING_INIT;
    transceiver_state_type next_state = TR_STATE_LISTENING;

    switch (state)
    {
        case TR_STATE_LISTENING_INIT:
            libRFM69_SetMode(RFM_RECEIVER);
            libRFM69_WaitForModeReady();
            INFO("Next state: TR_STATE_LISTENING_WAITING");
            state = TR_STATE_LISTENING_WAITING;
            break;

        case TR_STATE_LISTENING_WAITING:
            if (libRFM69_IsPayloadReady())
            {
                libRFM69_SetMode(RFM_STANDBY);
                libRFM69_WaitForModeReady();

                HandlePayload();
                INFO("Next state: TR_STATE_LISTENING_INIT");
                state = TR_STATE_LISTENING_INIT;

                INFO("PacketToSend %u", PacketToSend());
            }
            else if (libRFM69_IsRxTimeoutFlagSet())
            {
                WARNING("Rx timeout!");
                libRFM69_RestartRx();
            }

            else if (PacketToSend())
            {
                INFO("Next state: SENDING");
                state = TR_STATE_LISTENING_INIT;
                next_state = TR_STATE_SENDING;
            }
            break;

        default:
            sc_assert_fail();
            break;
    }
    return next_state;
}

static transceiver_state_type SendingStateMachine(void)
{
    static transceiver_sending_state_type state = TR_STATE_SENDING_INIT;
    transceiver_state_type next_state = TR_STATE_SENDING;

    switch (state)
    {
        case TR_STATE_SENDING_INIT:
            DEBUG("FIFO not empty 0: %u\r\n", libRFM69_IsFIFONotEmpty());
            libRFM69_SetMode(RFM_STANDBY);
            state = TR_STATE_SENDING_WRITING;
            break;

        case TR_STATE_SENDING_WRITING:
            if (libRFM69_IsModeReady())
            {
                DEBUG("Write packet to FIFO\r\n");
                libRFM69_WriteToFIFO((uint8_t *)&packet_frame_tx.header,
                                     sizeof(packet_header_type));
                libRFM69_WriteToFIFO((uint8_t *)&packet_frame_tx.content,
                                     packet_frame_tx.content.size + 8);
                libRFM69_SetMode(RFM_TRANSMITTER);
                libRFM69_EnableHighPowerSetting(true);

                DEBUG("TX ready: %u\r\n", libRFM69_IsTxReady());
                DEBUG("FIFO not empty 1: %u\r\n", libRFM69_IsFIFONotEmpty());
                state = TR_STATE_SENDING_TRANSMITTING;
            }
            break;

        case TR_STATE_SENDING_TRANSMITTING:
            if (libRFM69_IsPacketSent())
            {
                DEBUG("Packet sent\r\n");
                if (packet_frame_tx.header.ack)
                {
                    DEBUG("Waiting for ACK\r\n");
                    libRFM69_SetMode(RFM_RECEIVER);
                    libRFM69_EnableHighPowerSetting(false);
                    state = TR_STATE_SENDING_WAIT_FOR_ACK;
                }
                else
                {
                    DEBUG("No ACK needed\r\n");

                    if (packet_frame_tx.callback != NULL)
                    {
                        packet_frame_tx.callback(true);
                    }
                    memset(&packet_frame_tx, 0, sizeof(packet_frame_type));

                    state = TR_STATE_SENDING_INIT;
                    next_state = TR_STATE_LISTENING;
                }
            }
            break;

        case TR_STATE_SENDING_WAIT_FOR_ACK:
            if (libRFM69_IsPayloadReady())
            {
                if (HandlePayload())
                {
                    //TODO: Implement this
                }
                libRFM69_SetMode(RFM_STANDBY);
                memset(&packet_frame_tx, 0, sizeof(packet_frame_type));
                state = TR_STATE_SENDING_INIT;
                next_state = TR_STATE_LISTENING;
            }
            break;

        default:
            sc_assert_fail();
            break;
    }
    return next_state;
}

#ifdef DEBUG_ENABLE
static void DumpPacket(packet_frame_type *packet)
{
    DEBUG("<PCK>");
    DEBUG("%u,", packet->header.target);
    DEBUG("%u,", packet->header.source);
    DEBUG("%u,", packet->header.ack);
    DEBUG("%d,", packet->header.rssi);
    DEBUG("%u,", packet->header.total_size);
    DEBUG("20%02u-%02u-%02u %02u:%02u:%02u,",
          packet->content.timestamp.year,
          packet->content.timestamp.month,
          packet->content.timestamp.date,
          packet->content.timestamp.hour,
          packet->content.timestamp.minute,
          packet->content.timestamp.second);
    DEBUG("%u,", packet->content.type);
    DEBUG("%u\r\n", packet->content.size);
}
#endif