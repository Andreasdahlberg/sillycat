/**
 * @file   libRFM69.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2015-09-10 (Last edit)
 * @brief  Header of RFM69HW-library.
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
//////////////////////////////////////////////////////////////////////////

//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

#ifndef LIBRFM69_H_
#define LIBRFM69_H_

#define READ_REG	0x7F
#define WRITE_REG	0x80

typedef enum
{
	RFM_SLEEP = 0,
	RFM_STANDBY,
	RFM_SYNTHESIZER,
	RFM_TRANSMITTER,
	RFM_RECEIVER
}libRFM69_mode_type;

typedef enum
{
	RFM_PACKET_DATA,
	RFM_RESERVED,	//Do not use
	RFM_CONT_SYNC_DATA,
	RFM_CONT_DATA
}libRFM69_data_mode_type;

typedef enum
{
	RFM_PACKET_FIXED_LEN,
	RFM_PACKET_VARIABLE_LEN	
	
}libRFM69_packet_format_type;

typedef enum
{
	RFM_FSK,
	RFM_OOK
}libRFM69_modulation_type_type;

#ifndef NODE_ADDRESS
	#define NODE_ADDRESS 0xAA
#endif

#ifndef BROADCAST_ADDRESS
	#define BROADCAST_ADDRESS 0x00
#endif

#ifdef	NETWORK_MASTER
	#define NETWORK_ID NODE_ADDRESS
#endif

#define RFM_PWR_1	0x04 //PA0 output on pin RFIO 
#define RFM_PWR_2	0x02 //PA1 enabled on pin PA_BOOST
#define RFM_PWR_3_4	0x03//PA1 and PA2 combined on pin PA_BOOST /PA1+PA2 on PA_BOOST with high output power +20dBm

#define MAX_SYNC_WORD_SIZE 7

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

void libRFM69_Init();
void libRFM69_Update();
void libRFM69_Test();
bool libRFM69_SetMode(libRFM69_mode_type mode);
void libRFM69_EnableEncryption(bool enable);
bool libRFM69_IsModeReady();
void libRFM69_WaitForModeReady();
void libRFM69_SetCarrierFrequency(uint32_t frequency);
void libRFM69_EnableSequencer(bool enable);
void libRFM69_GetTemperature(uint8_t *temperature);
bool libRFM69_SetBitRate(uint32_t bit_rate);
bool libRFM69_SetDataMode(libRFM69_data_mode_type data_mode);
bool libRFM69_SetModulationType(libRFM69_modulation_type_type modulation_type);
bool libRFM69_SetModulationShaping(uint8_t modulation_shaping);
bool libRFM69_SetFrequencyDeviation(uint16_t frequency_deviation);
bool libRFM69_SetPowerAmplifierMode(uint8_t mode);
bool libRFM69_SetPreambleLength(uint16_t length);
bool libRFM69_SetSyncWordSize(uint8_t size);
bool libRFM69_SetSyncWord(uint8_t *sync_word, uint8_t length);
bool libRFM69_SetPacketFormat(libRFM69_packet_format_type packet_format);
bool libRFM69_EnableSyncWordGeneration(bool enabled);
bool libRFM69_CalibrateRCOscillator(void);
uint32_t libRFM69_GetBitrate(void);
uint8_t libRFM69_GetChipVersion(void);
uint8_t libRFM69_GetPowerAmplifierMode(void);
int8_t libRFM69_GetOutputPower(void);
int8_t libRFM69_GetRSSI(void);
uint16_t libRFM69_GetPreambleLength(void);
uint8_t libRFM69_GetSyncWordSize(void);
bool libRFM69_GetSyncWord(uint8_t *sync_word, uint8_t length);

bool libRFM69_EnableOCP(bool enabled);
bool libRFM69_ClearFIFOOverrun(void);

bool libRFM69_IsHighPowerEnabled(void);

void libRFM69_DumpRegisterValues(void);

#endif /* LIBRFM69_H_ */