/**
 * @file   Transceiver.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-10-22 (Last edit)
 * @brief  Implementation of the Transceiver interface.
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

#include "common.h"
#include <string.h>
#include "libDebug.h"
#include "libRFM69.h"
#include "Timer.h"
#include "Config.h"
#include "ErrorHandler.h"
#include "FIFO.h"
#include "Transceiver.h"

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

struct module_t
{
    struct
    {
        transceiver_state_type transceiver;
        transceiver_sending_state_type sending;
        transceiver_listening_state_type listening;
    } state;
};

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static struct module_t module;

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
static void DumpPacket(const packet_frame_type *packet_p);
#endif

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void Transceiver_Init(void)
{
    module = (struct module_t) { {.transceiver = TR_STATE_LISTENING}};

    libRFM69_Init();
    libRFM69_EnableEncryption(false);
    libRFM69_EnableSequencer(true);
    libRFM69_EnableListenMode(false);
    libRFM69_SetPacketRxDelay(1);
    libRFM69_SetMode(RFM_STANDBY);
    libRFM69_WaitForModeReady();
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
    libRFM69_SetRSSIThreshold(-85);
    libRFM69_SetSyncWordSize(6);
    libRFM69_SetSyncWord(Config_GetNetworkId(), 6);
    libRFM69_SetTXStartCondition(RFM_TX_START_NOT_EMPTY);
    libRFM69_SetNodeAddress(Config_GetAddress());
    libRFM69_SetBroadcastAddress(Config_GetBroadcastAddress());
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
    libRFM69_EnableHighPowerSetting(false);
    libRFM69_SetPowerLevel(28);
    libRFM69_SetAESKey((uint8_t *)Config_GetAESKey());

    tx_packet_fifo = FIFO_New(tx_packet_buffer);
    rx_packet_fifo = FIFO_New(rx_packet_buffer);

    INFO("Transceiver initiated");
}

void Transceiver_Update(void)
{
    switch (module.state.transceiver)
    {
        case TR_STATE_LISTENING:
            module.state.transceiver = ListeningStateMachine();
            break;

        case TR_STATE_SENDING:
            module.state.transceiver = SendingStateMachine();
            break;

        default:
            sc_assert_fail();
            break;
    }
}

bool Transceiver_ReceivePacket(packet_frame_type *packet_p)
{
    sc_assert(packet_p != NULL);

    bool status;
    status = FIFO_Pop(&rx_packet_fifo, packet_p);

    return status;
}

bool Transceiver_SendPacket(uint8_t target, const packet_content_type *content_p)
{
    sc_assert(content_p != NULL);
    sc_assert(target != 0);

    bool status = false;
    if (content_p->size <= CONTENT_DATA_SIZE && !FIFO_IsFull(&tx_packet_fifo))
    {
        packet_frame_type packet;

        packet.header.target = target;
        packet.header.source = Config_GetAddress();
        packet.header.rssi = 0;
        packet.header.total_size = sizeof(packet_header_type) + 8 +
                                   content_p->size; //TODO: sizeof or define!

        packet.content.timestamp = content_p->timestamp;
        packet.content.type = content_p->type;
        packet.content.size = content_p->size;
        memcpy(packet.content.data, content_p->data, content_p->size);

        status = FIFO_Push(&tx_packet_fifo, &packet);
    }

    return status;
}

void Transceiver_EventHandler(const event_t *event_p)
{
    sc_assert(event_p != NULL);

    switch (Event_GetId(event_p))
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
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static bool IsActive(void)
{
    return (module.state.transceiver == TR_STATE_SENDING ||
            libRFM69_IsPayloadReady() || PacketToSend());
}

static bool PacketToSend(void)
{
    return (!FIFO_IsEmpty(&tx_packet_fifo));
}

static bool HandlePayload(void)
{
    packet_frame_type packet;
    uint8_t *packet_p = (uint8_t *)&packet;

    // Read the first byte containing the payload length.
    libRFM69_ReadFromFIFO(packet_p, 1);
    ++packet_p;

    if (packet.header.total_size > RFM_FIFO_SIZE - 1)
    {
        ERROR("Size of packet is larger then the RFM69 FIFO");

        libRFM69_ClearFIFO();
        return false;
    }

    // Read the payload.
    libRFM69_ReadFromFIFO(packet_p, packet.header.total_size);

    packet.header.rssi = libRFM69_GetRSSI();

#ifdef DEBUG_ENABLE
    DumpPacket(&packet);
#endif

    return FIFO_Push(&rx_packet_fifo, &packet);
}

static transceiver_state_type ListeningStateMachine(void)
{
    transceiver_state_type next_state = TR_STATE_LISTENING;

    switch (module.state.listening)
    {
        case TR_STATE_LISTENING_INIT:
            libRFM69_SetMode(RFM_RECEIVER);
            libRFM69_WaitForModeReady();
            INFO("Next state: TR_STATE_LISTENING_WAITING");
            module.state.listening = TR_STATE_LISTENING_WAITING;
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
                module.state.listening = TR_STATE_LISTENING_INIT;

            }
            else if (libRFM69_IsRxTimeoutFlagSet())
            {
                WARNING("Rx timeout!");
                libRFM69_RestartRx();
            }
            else if (PacketToSend())
            {
                INFO("Next state: SENDING");
                module.state.listening = TR_STATE_LISTENING_INIT;
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
    transceiver_state_type next_state = TR_STATE_SENDING;

    switch (module.state.sending)
    {
        case TR_STATE_SENDING_INIT:
            // Change to standby mode before starting to write to the FIFO.
            libRFM69_SetMode(RFM_STANDBY);
            module.state.sending = TR_STATE_SENDING_WRITING;
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
                    module.state.sending = TR_STATE_SENDING_TRANSMITTING;
                }
                else
                {
                    WARNING("No packets available, aborting TX sequence.");
                    module.state.sending = TR_STATE_SENDING_INIT;
                    next_state = TR_STATE_LISTENING;
                }
            }
            break;

        case TR_STATE_SENDING_TRANSMITTING:
            if (libRFM69_IsPacketSent())
            {
                module.state.sending = TR_STATE_SENDING_INIT;
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
static void DumpPacket(const packet_frame_type *packet_p)
{
    DEBUG("<PCK>");
    DEBUG("%u,", packet_p->header.target);
    DEBUG("%u,", packet_p->header.source);
    DEBUG("%d,", packet_p->header.rssi);
    DEBUG("%u,", packet_p->header.total_size);
    DEBUG("20%02u-%02u-%02u %02u:%02u:%02u,",
          packet_p->content.timestamp.year,
          packet_p->content.timestamp.month,
          packet_p->content.timestamp.date,
          packet_p->content.timestamp.hour,
          packet_p->content.timestamp.minute,
          packet_p->content.timestamp.second);
    DEBUG("%u,", packet_p->content.type);
    DEBUG("%u\r\n", packet_p->content.size);
}
#endif
