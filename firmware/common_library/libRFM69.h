/**
 * @file   libRFM69.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2016-02-14 (Last edit)
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

#define READ_REG    0x7F
#define WRITE_REG   0x80

typedef enum
{
    RFM_SLEEP = 0,
    RFM_STANDBY,
    RFM_SYNTHESIZER,
    RFM_TRANSMITTER,
    RFM_RECEIVER
} libRFM69_mode_type;

typedef enum
{
    RFM_PACKET_DATA,
    RFM_RESERVED,   //Do not use
    RFM_CONT_SYNC_DATA,
    RFM_CONT_DATA
} libRFM69_data_mode_type;

typedef enum
{
    RFM_PACKET_FIXED_LEN,
    RFM_PACKET_VARIABLE_LEN

} libRFM69_packet_format_type;

typedef enum
{
    RFM_FSK,
    RFM_OOK
} libRFM69_modulation_type_type;

typedef enum
{
    RFM_FIFO_FILL_AUTO = 0,
    RFM_FIFO_FILL_MAN
} libRFM69_fifo_fill_condition_type;

typedef enum
{
    RFM_TX_START_LEVEL = 0,
    RFM_TX_START_NOT_EMPTY
} libRFM69_tx_start_condition_type;

typedef enum
{
    RFM_ADDRESS_FILTER_NONE = 0,
    RFM_ADDRESS_FILTER_ADDRESS,
    RFM_ADDRESS_FILTER_ADDRESS_BROADCAST,
} libRFM69_address_filtering_type;

#ifndef NODE_ADDRESS
#define NODE_ADDRESS 0xAA
#endif

#ifndef BROADCAST_ADDRESS
#define BROADCAST_ADDRESS 0xFF
#endif

#ifdef  NETWORK_MASTER
#define NETWORK_ID NODE_ADDRESS
#endif

#define RFM_PWR_1   0x04 //PA0 output on pin RFIO
#define RFM_PWR_2   0x02 //PA1 enabled on pin PA_BOOST
#define RFM_PWR_3_4 0x03//PA1 and PA2 combined on pin PA_BOOST /PA1+PA2 on PA_BOOST with high output power +20dBm

#define MAX_SYNC_WORD_SIZE 8

#define RFM_FIFO_SIZE 66

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

void libRFM69_Init(void);
void libRFM69_HWInit(void);
void libRFM69_Update(void);
void libRFM69_SetMode(libRFM69_mode_type mode);
void libRFM69_EnableEncryption(bool enable);
bool libRFM69_IsModeReady();
bool libRFM69_WaitForModeReady();
void libRFM69_SetCarrierFrequency(uint32_t frequency);
void libRFM69_EnableSequencer(bool enable);
void libRFM69_GetTemperature(uint8_t *temperature);
void libRFM69_SetBitRate(uint32_t bit_rate);
void libRFM69_SetDataMode(libRFM69_data_mode_type data_mode);
void libRFM69_SetModulationType(libRFM69_modulation_type_type modulation_type);
void libRFM69_SetModulationShaping(uint8_t modulation_shaping);
void libRFM69_SetFrequencyDeviation(uint16_t frequency_deviation);
void libRFM69_SetPowerAmplifierMode(uint8_t mode);
void libRFM69_SetPreambleLength(uint16_t length);
void libRFM69_SetSyncWordSize(uint8_t size);
bool libRFM69_SetSyncWord(const uint8_t *sync_word, uint8_t length);
void libRFM69_SetPacketFormat(libRFM69_packet_format_type packet_format);
void libRFM69_SetFIFOFillCondition(libRFM69_fifo_fill_condition_type
                                   fill_condition);
void libRFM69_SetTXStartCondition(libRFM69_tx_start_condition_type
                                  start_condition);
uint8_t libRFM69_ReadFromFIFO(uint8_t *data, uint8_t max_length);
void libRFM69_EnableSyncWordGeneration(bool enabled);
void libRFM69_EnableListenMode(bool enabled);
void libRFM69_CalibrateRCOscillator(void);
libRFM69_mode_type libRFM69_GetMode(void);
uint32_t libRFM69_GetBitrate(void);
uint8_t libRFM69_GetChipVersion(void);
uint8_t libRFM69_GetPowerAmplifierMode(void);
int8_t libRFM69_GetOutputPower(void);
int8_t libRFM69_GetRSSI(void);
uint16_t libRFM69_GetPreambleLength(void);
uint8_t libRFM69_GetSyncWordSize(void);
uint8_t libRFM69_GetSyncWord(uint8_t *sync_word, uint8_t length);
void libRFM69_EnableOCP(bool enabled);
bool libRFM69_ClearFIFOOverrun(void);
void libRFM69_ClearFIFO(void);
uint8_t libRFM69_WriteToFIFO(const uint8_t *data, uint8_t length);
bool libRFM69_IsHighPowerEnabled(void);
bool libRFM69_IsRxReady();
bool libRFM69_IsTxReady();
bool libRFM69_IsRSSIThresholdExceeded();
bool libRFM69_IsFIFOFull(void);
bool libRFM69_IsFIFONotEmpty(void);
bool libRFM69_IsFIFOLevel(void);
bool libRFM69_IsFIFOOverrun(void);
bool libRFM69_IsPacketSent(void);
bool libRFM69_IsPayloadReady(void);
bool libRFM69_IsCRCOk(void);
void libRFM69_SetAddressFiltering(libRFM69_address_filtering_type filtering);
void libRFM69_SetNodeAddress(uint8_t node_address);
void libRFM69_SetBroadcastAddress(uint8_t broadcast_address);
void libRFM69_EnableHighPowerSetting(bool enable);
void libRFM69_SetPowerLevel(uint8_t power_level);
void libRFM69_SetPacketRxDelay(uint8_t val);
void libRFM69_RestartRx(void);
void libRFM69_WriteRegister(uint8_t address, uint8_t register_data);
void libRFM69_ReadRegister(uint8_t address, uint8_t *register_data);

#ifdef DEBUG_ENABLE
void libRFM69_DumpRegisterValues(void);
#endif

#endif /* LIBRFM69_H_ */