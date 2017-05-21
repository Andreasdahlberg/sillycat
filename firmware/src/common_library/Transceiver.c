/**
 * @file   Transceiver.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2017-06-06 (Last edit)
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
#include "FIFO.h"

#include "RFM69Registers.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define TX_PACKET_FIFO_SIZE 3
#define RX_PACKET_FIFO_SIZE 4

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
} transceiver_sending_state_type;

typedef enum
{
    TR_STATE_LISTENING_INIT = 0,
    TR_STATE_LISTENING_WAITING,
    TR_STATE_LISTENING_RECEIVING,
    TR_STATE_LISTENING_DONE,
} transceiver_listening_state_type;

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static transceiver_state_type transceiver_state = TR_STATE_NO_INIT;

static packet_frame_type tx_packet_buffer[TX_PACKET_FIFO_SIZE];
static packet_frame_type rx_packet_buffer[RX_PACKET_FIFO_SIZE];

static fifo_type tx_packet_fifo;
static fifo_type rx_packet_fifo;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

static transceiver_state_type SendingStateMachine(void);
static transceiver_state_type ListeningStateMachine(void);
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
    libRFM69_SetRSSIThreshold(-35);
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

    tx_packet_fifo = FIFO_New(tx_packet_buffer);
    rx_packet_fifo = FIFO_New(rx_packet_buffer);

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
/// @brief Receive a packet
///
/// @param  *packet Pointer to packet.
/// @return True if a packet was available, otherwise False.
///
bool Transceiver_ReceivePacket(packet_frame_type *packet)
{
    sc_assert(packet != NULL);

    bool status;
    status = FIFO_Pop(&rx_packet_fifo, packet);

    return status;
}

///
/// @brief Send a packet.
///
/// The packet is added to the send queue and is sent when the transceiver is ready.
///
/// @param  target Address of target.
/// @param  *content Pointer to packet content. A complete packet with a header
///                  will be created from this content.
/// @return None
///
bool Transceiver_SendPacket(uint8_t target, packet_content_type *content)
{
    sc_assert(content != NULL);
    sc_assert(target != 0);

    bool status = false;
    if (content->size <= CONTENT_DATA_SIZE && !FIFO_IsFull(&tx_packet_fifo))
    {
        packet_frame_type packet;

        packet.header.target = target;
        packet.header.source = Config_GetNodeId();
        packet.header.rssi = 0;
        packet.header.total_size = sizeof(packet_header_type) + 8 +
                                   content->size; //TODO: sizeof or define!

        packet.content.timestamp = content->timestamp;
        packet.content.type = content->type;
        packet.content.size = content->size;
        memcpy(packet.content.data, content->data, content->size);

        status = FIFO_Push(&tx_packet_fifo, &packet);
    }

    return status;
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

static bool PacketToSend(void)
{
    return (!FIFO_IsEmpty(&tx_packet_fifo));
}

static bool HandlePayload(void)
{
    packet_frame_type packet;
    uint8_t *packet_ptr = (uint8_t *)&packet;

    // Read the first byte containing the payload length.
    libRFM69_ReadFromFIFO(packet_ptr, 1);
    ++packet_ptr;

    if (packet.header.total_size > RFM_FIFO_SIZE - 1)
    {
        ERROR("Size of packet is larger then the RFM69 FIFO");

        libRFM69_ClearFIFO();
        return false;
    }

    // Read the payload.
    libRFM69_ReadFromFIFO(packet_ptr, packet.header.total_size);

    packet.header.rssi = libRFM69_GetRSSI();

#ifdef DEBUG_ENABLE
    DumpPacket(&packet);
#endif

    return FIFO_Push(&rx_packet_fifo, &packet);
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

                if (!HandlePayload())
                {
                    WARNING("Failed to handle packet");
                }

                INFO("Next state: TR_STATE_LISTENING_INIT");
                state = TR_STATE_LISTENING_INIT;

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
            // Change to standby mode before starting to write to the FIFO.
            libRFM69_SetMode(RFM_STANDBY);
            state = TR_STATE_SENDING_WRITING;
            break;

        case TR_STATE_SENDING_WRITING:
            if (libRFM69_IsModeReady())
            {
                packet_frame_type packet;
                if (FIFO_Pop(&tx_packet_fifo, &packet))
                {
                    libRFM69_WriteToFIFO((uint8_t *)&packet.header,
                                         sizeof(packet_header_type));
                    libRFM69_WriteToFIFO((uint8_t *)&packet.content,
                                         packet.content.size + 8);

                    libRFM69_SetMode(RFM_TRANSMITTER);
                    libRFM69_EnableHighPowerSetting(true);
                    state = TR_STATE_SENDING_TRANSMITTING;
                }
                else
                {
                    WARNING("No packets available, aborting TX sequence.");
                    state = TR_STATE_SENDING_INIT;
                    next_state = TR_STATE_LISTENING;
                }
            }
            break;

        case TR_STATE_SENDING_TRANSMITTING:
            if (libRFM69_IsPacketSent())
            {
                libRFM69_EnableHighPowerSetting(false);

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
