/**
 * @file   Transceiver.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2015-12-13 (Last edit)
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
#include "Sensor.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

typedef enum
{
    TR_STATE_NO_INIT = 0,
    TR_STATE_IDLE,
    TR_STATE_LISTENING,
    TR_STATE_SENDING,
    TR_STATE_SLEEPING
} transceiver_state_type;

typedef enum
{
    TR_STATE_SENDING_INIT = 0,
    TR_STATE_SENDING_WRITING,
    TR_STATE_SENDING_TRANSMITTING,
    TR_STATE_SENDING_WAIT_FOR_ACK,
    TR_STATE_SENDING_DONE,
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

//static char aes_key[17] = "1DUMMYKEYFOOBAR1";
static uint8_t net_id[6] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

static transceiver_state_type transceiver_state = TR_STATE_NO_INIT;
static packet_frame_type packet_frame;
static transceiver_packet_handler_type packet_handlers[TR_PACKET_NR_TYPES];

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

static transceiver_state_type IdleStateMachine(void);
static transceiver_state_type SendingStateMachine(void);
static transceiver_state_type ListeningStateMachine(void);
static bool IsPacketTypeValid(packet_type_type packet_type);
static bool IsActive(void);
static bool ReadyToSend(void);
static bool PacketToSend(void);

#ifdef DEBUG_ENABLE
static void DumpPacketHeader(packet_header_type *header);
static void DumpPacketContent(packet_content_type *content);
#endif

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void Transceiver_Init(void)
{
    libRFM69_Init();
    //IMPORTANT: Only disable encryption during development!
    libRFM69_EnableEncryption(FALSE);
    libRFM69_EnableSequencer(TRUE);
    libRFM69_EnableListenMode(FALSE);

    libRFM69_SetMode(RFM_STANDBY);
    libRFM69_WaitForModeReady();

    libRFM69_SetBitRate(4800);
    libRFM69_SetDataMode(RFM_PACKET_DATA);
    libRFM69_SetModulationType(RFM_FSK);
    libRFM69_SetModulationShaping(0x00);
    libRFM69_SetFrequencyDeviation(5000);
    libRFM69_SetCarrierFrequency(868000000);
    libRFM69_EnableSyncWordGeneration(TRUE);
    libRFM69_SetFIFOFillCondition(RFM_FIFO_FILL_AUTO);

    //TODO: Decide how network id is set, only set this if master?
    libRFM69_SetSyncWordSize(5);
    libRFM69_SetSyncWord(net_id, 6);

    libRFM69_SetTXStartCondition(RFM_TX_START_NOT_EMPTY);
    libRFM69_SetNodeAddress(NODE_ADDRESS);
    libRFM69_ClearFIFOOverrun();
    libRFM69_SetPacketFormat(RFM_PACKET_VARIABLE_LEN);

    libRFM69_EnableOCP(FALSE);
    libRFM69_SetPowerAmplifierMode(0x04);

    libRFM69_SetMode(RFM_STANDBY);
    libRFM69_WaitForModeReady();

    memset(&packet_frame, 0, sizeof(packet_frame_type));
    transceiver_state = TR_STATE_IDLE;

    INFO("Transceiver initiated");
}

void Transceiver_Update(void)
{
    switch (transceiver_state)
    {
        case TR_STATE_NO_INIT:
            WARNING("Transceiver not initialized");
            Transceiver_Init();
            break;

        case TR_STATE_IDLE:
            transceiver_state = IdleStateMachine();
            break;

        case TR_STATE_LISTENING:
            transceiver_state = ListeningStateMachine();
            break;

        case TR_STATE_SENDING:
            transceiver_state = SendingStateMachine();
            break;

        case TR_STATE_SLEEPING:
            if (!IsActive() && libRFM69_IsModeReady() && libRFM69_GetMode() != RFM_SLEEP)
            {
                libRFM69_SetMode(RFM_SLEEP);
            }
            break;

        default:
            sc_assert_fail();
            break;
    }
    return;
}

bool Transceiver_SendPacket(uint8_t target, bool request_ack,
                            packet_content_type *content,
                            transceiver_callback_type callback)
{
    sc_assert(content != NULL);

    bool status = TRUE;
    packet_frame_type *frame_ptr;

    frame_ptr = &packet_frame;

    if (content->size > CONTENT_DATA_SIZE)
    {
        ERROR("Size of packet content is to large");
        status = FALSE;
    }

    DEBUG("Ready to send: %u\r\n", (uint8_t)ReadyToSend());

    if (ReadyToSend() && status)
    {
        frame_ptr->header.target = target;
        frame_ptr->header.source = NODE_ADDRESS;
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

bool Transceiver_SetPacketHandler(transceiver_packet_handler_type
                                  packet_handler,
                                  packet_type_type packet_type)
{
    sc_assert(packet_handler != NULL);
    bool status = FALSE;

    if (IsPacketTypeValid(packet_type))
    {
        packet_handlers[packet_type] = packet_handler;
        status = TRUE;
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
            while(IsActive() == TRUE)
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
            transceiver_state == TR_STATE_SENDING);
}

static bool IsPacketTypeValid(packet_type_type packet_type)
{
    return (packet_type < TR_PACKET_NR_TYPES);
}

//TODO: Make this function public?
static bool ReadyToSend(void)
{
    return (transceiver_state == TR_STATE_IDLE && libRFM69_GetRSSI() < -90);
}

static bool PacketToSend(void)
{
    return (packet_frame.header.target > 0);
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
        return FALSE;
    }

    libRFM69_ReadFromFIFO(&data_buffer[1], length);

    memcpy(&packet_frame.header, data_buffer, sizeof(packet_header_type));
    memcpy(&packet_frame.content, &data_buffer[sizeof(packet_header_type)],
           sizeof(packet_content_type));

#ifdef DEBUG_ENABLE
    DumpPacketHeader(&packet_frame.header);
    DumpPacketContent(&packet_frame.content);
#endif

    if (IsPacketTypeValid(packet_frame.content.type) &&
            packet_handlers[packet_frame.content.type] != NULL)
    {
        return packet_handlers[packet_frame.content.type](&packet_frame);
    }
    else
    {
        INFO("No packet handler for packet type %u");
        return FALSE;
    }
}

static transceiver_state_type IdleStateMachine(void)
{
    transceiver_state_type next_state = TR_STATE_IDLE;

    if (PacketToSend())
    {
        INFO("Sending packet from buffer");
        next_state = TR_STATE_SENDING;
    }

    return next_state;
}

static transceiver_state_type ListeningStateMachine(void)
{
    static transceiver_listening_state_type state = TR_STATE_LISTENING_INIT;

    switch (state)
    {
        case TR_STATE_LISTENING_INIT:
            libRFM69_SetMode(RFM_RECEIVER);
            libRFM69_EnableHighPowerSetting(FALSE);
            state = TR_STATE_LISTENING_WAITING;
            break;

        case TR_STATE_LISTENING_WAITING:
            if (libRFM69_IsPayloadReady())
            {
                DEBUG("RSSI: %i\r\n", libRFM69_GetRSSI());
                if (HandlePayload() == TRUE)
                {
                    libRFM69_SetMode(RFM_TRANSMITTER);
                    libRFM69_EnableHighPowerSetting(TRUE);
                    state = TR_STATE_LISTENING_SEND_ACK;
                }
            }
            break;

        case TR_STATE_LISTENING_SEND_ACK:
            if (libRFM69_IsModeReady())
            {
                //Transceiver_SendPacket() //Send ACK-packet, async?
                state = TR_STATE_LISTENING_INIT;
            }
            break;

        default:
            sc_assert_fail();
            break;
    }
    return TR_STATE_LISTENING;
}

static transceiver_state_type SendingStateMachine(void)
{
    static transceiver_sending_state_type state = TR_STATE_SENDING_INIT;
    transceiver_state_type next_state = TR_STATE_SENDING;

    switch (state)
    {
        case TR_STATE_SENDING_INIT:
            libRFM69_SetMode(RFM_STANDBY);
            state = TR_STATE_SENDING_WRITING;
            break;

        case TR_STATE_SENDING_WRITING:
            if (libRFM69_IsModeReady())
            {
                DEBUG("Write packet to FIFO\r\n");
                libRFM69_WriteToFIFO((uint8_t *)&packet_frame.header,
                                     sizeof(packet_header_type));
                libRFM69_WriteToFIFO((uint8_t *)&packet_frame.content,
                                     packet_frame.content.size + 8);
                libRFM69_SetMode(RFM_TRANSMITTER);
                libRFM69_EnableHighPowerSetting(TRUE);
                state = TR_STATE_SENDING_TRANSMITTING;
            }
            break;

        case TR_STATE_SENDING_TRANSMITTING:
            if (libRFM69_IsPacketSent())
            {
                DEBUG("Packet sent\r\n");
                if (packet_frame.header.ack)
                {
                    DEBUG("Waiting for ACK\r\n");
                    libRFM69_SetMode(RFM_RECEIVER);
                    libRFM69_EnableHighPowerSetting(FALSE);
                    state = TR_STATE_SENDING_WAIT_FOR_ACK;
                }
                else
                {
                    DEBUG("No ACK needed\r\n");
                    libRFM69_SetMode(RFM_STANDBY);
                    if (packet_frame.callback != NULL)
                    {
                        packet_frame.callback(TRUE);
                    }
                    memset(&packet_frame, 0, sizeof(packet_frame_type));
                    state = TR_STATE_SENDING_INIT;
                    next_state = TR_STATE_IDLE;
                }
            }
            break;

        case TR_STATE_SENDING_WAIT_FOR_ACK:
            if (libRFM69_IsPayloadReady())
            {
                if (HandlePayload())
                {

                }
                libRFM69_SetMode(RFM_STANDBY);
                memset(&packet_frame, 0, sizeof(packet_frame_type));
                state = TR_STATE_SENDING_INIT;
                next_state = TR_STATE_IDLE;

            }
            break;

        case TR_STATE_SENDING_DONE:
            break;

        default:
            sc_assert_fail();
            break;
    }
    return next_state;
}

#ifdef DEBUG_ENABLE
void DumpPacketHeader(packet_header_type *header)
{
    DEBUG("******Packet header******\r\n");
    DEBUG("Target: %u\r\n", header->target);
    DEBUG("Source: %u\r\n", header->source);
    DEBUG("Ack: %u\r\n", (uint8_t)header->ack);
    DEBUG("RSSI: %d\r\n", header->rssi);
    DEBUG("Total size: %u\r\n", header->total_size);
}

void DumpPacketContent(packet_content_type *content)
{
    uint8_t idx;

    DEBUG("******Packet content******\r\n");
    DEBUG("Timestamp: 20%02u-%02u-%02u %02u:%02u:%02u\r\n",
          content->timestamp.year,
          content->timestamp.month,
          content->timestamp.date,
          content->timestamp.hour,
          content->timestamp.minute,
          content->timestamp.second);
    DEBUG("Type: %u\r\n", (uint8_t)content->type);
    DEBUG("Size: %u\r\n", (uint8_t)content->size);
    DEBUG("Data: 0x");


    for (idx = 0; idx < content->size; ++idx)
    {
        DEBUG("%02X", content->data[idx]);
    }
    DEBUG("\r\n");
}
#endif