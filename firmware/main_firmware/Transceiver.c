/**
 * @file   Transceiver.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2015-10-14 (Last edit)
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

#define F_CPU 8000000UL // 8 MHz

#include "common.h"
#include "libDebug.h"
#include "libRFM69.h"
#include "libTimer.h"

#include <util/delay.h>

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

typedef struct 
{
    uint8_t length;
    uint8_t address;
    uint8_t data[66];
}transceiver_packet_type;

typedef enum
{
    TR_STATE_NO_INIT = 0,
    TR_STATE_IDLE,
    TR_STATE_LISTENING,
    TR_STATE_SENDING,
}transceiver_state_type;

typedef enum
{
    TR_STATE_SENDING_INIT = 0,
    TR_STATE_SENDING_WRITING,
    TR_STATE_SENDING_TRANSMITTING,
    TR_STATE_SENDING_DONE,
}transceiver_sending_state_type;

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

//static char aes_key[17] = "1DUMMYKEYFOOBAR1";
static uint8_t net_id[6] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
   
static transceiver_state_type transceiver_state = TR_STATE_NO_INIT;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

transceiver_state_type IdleStateMachine(void);
transceiver_state_type SendingStateMachine(void);

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
    
    
    uint32_t start = libTimer_GetMilliseconds();
    libRFM69_SetMode(RFM_SLEEP);
    libRFM69_WaitForModeReady();
    
    DEBUG("Waited for: %u ms\r\n", libTimer_TimeDifference(start));
	    
    transceiver_state = TR_STATE_IDLE;
    INFO("Transceiver initiated");
}

void Transceiver_Update(void)
{
    switch (transceiver_state)
    {
        case TR_STATE_IDLE:
            transceiver_state = IdleStateMachine();
            break;

        case TR_STATE_LISTENING:
            break;
            
        case TR_STATE_SENDING:
            transceiver_state = SendingStateMachine();
            break;
            
        default:
            break;
    }
    return;
}

bool Transceiver_Send(void)
{
   
    
    return FALSE;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

transceiver_state_type IdleStateMachine(void)
{
   static uint32_t start = 0;

   if (libTimer_TimeDifference(start) > 500)
   {
       start = libTimer_GetMilliseconds();
       //DEBUG("Idle...\r\n");
   }       
   

   return TR_STATE_IDLE;
}

transceiver_state_type SendingStateMachine(void)
{
    static transceiver_sending_state_type state = TR_STATE_SENDING_INIT;
    
    switch (state)
    {
        case TR_STATE_SENDING_INIT:
            libRFM69_SetMode(RFM_STANDBY);
            if (libRFM69_IsModeReady())
            {
                state = TR_STATE_SENDING_WRITING;
            }                
            break;
            
        case TR_STATE_SENDING_WRITING:
            break;
            
        case TR_STATE_SENDING_TRANSMITTING:
            break;
            
        case TR_STATE_SENDING_DONE:
            break;
            
        default:
            ERROR("Unknown state, reset transceiver");
            state = TR_STATE_SENDING_INIT;
            Transceiver_Init();
            break;
    }
    return TR_STATE_SENDING;
}