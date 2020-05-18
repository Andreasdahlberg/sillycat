/**
 * @file   libRFM69.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2020-05-18 (Last edit)
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

#include <stdbool.h>
#include <stdint.h>

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

typedef enum
{
    RFM_CLKOUT_32MHZ = 0,
    RFM_CLKOUT_16MHZ,
    RFM_CLKOUT_8MHZ,
    RFM_CLKOUT_4MHZ,
    RFM_CLKOUT_2MHZ,
    RFM_CLKOUT_1MHZ,
    RFM_CLKOUT_RC,
    RFM_CLKOUT_OFF,
} libRFM69_clkout_freq_type;

typedef enum
{
    RFM_LNA_ZIN_50OHM = 0,
    RFM_LNA_ZIN_200OHM,
} libRFM69_lna_zin_type;

typedef enum
{
    RFM_LNA_GAIN_AUTO = 0,
    RFM_LNA_GAIN_G1,
    RFM_LNA_GAIN_G2,
    RFM_LNA_GAIN_G3,
    RFM_LNA_GAIN_G4,
    RFM_LNA_GAIN_G5,
    RFM_LNA_GAIN_G6,
} libRFM69_lna_gain_type;

typedef enum
{
    RFM_DCC_FREQ_16 = 0,
    RFM_DCC_FREQ_8,
    RFM_DCC_FREQ_4,
    RFM_DCC_FREQ_2,
    RFM_DCC_FREQ_1,
    RFM_DCC_FREQ_05,
    RFM_DCC_FREQ_025,
    RFM_DCC_FREQ_0125,
} libRFM69_dcc_freq_type;

#define RFM_PWR_1   0x04 //PA0 output on pin RFIO
#define RFM_PWR_2   0x02 //PA1 enabled on pin PA_BOOST
#define RFM_PWR_3_4 0x03//PA1 and PA2 combined on pin PA_BOOST /PA1+PA2 on PA_BOOST with high output power +20dBm

#define MAX_SYNC_WORD_SIZE 8

#define RFM_FIFO_SIZE 66

#define RFM_AES_KEY_LENGTH 16

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

void libRFM69_Init(void);
void libRFM69_Update(void) __attribute__((noreturn));
void libRFM69_SetMode(libRFM69_mode_type mode);
void libRFM69_EnableEncryption(bool enable);
void libRFM69_SetAESKey(const uint8_t *aes_key);
bool libRFM69_IsModeReady(void);
bool libRFM69_IsResetDone(void);
bool libRFM69_WaitForModeReady(void);
void libRFM69_Reset(void);
void libRFM69_SetCarrierFrequency(uint32_t frequency);
void libRFM69_EnableSequencer(bool enable);
void libRFM69_GetTemperature(int8_t *temperature);
void libRFM69_SetBitRate(uint32_t bit_rate);
uint32_t libRFM69_GetBitRate(void);
void libRFM69_SetDataMode(libRFM69_data_mode_type data_mode);
void libRFM69_SetModulationType(libRFM69_modulation_type_type modulation_type);
/**
 * Get the transceiver modulation type.
 *
 * @return FSK- or OOK-modulation.
 */
libRFM69_modulation_type_type libRFM69_GetModulationType(void);
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
void libRFM69_CalibrateRCOscillator(void) __attribute__((noreturn));
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
bool libRFM69_IsRxReady(void);
bool libRFM69_IsTxReady(void);
bool libRFM69_IsRSSIThresholdExceeded(void);
bool libRFM69_IsRxTimeoutFlagSet(void);
bool libRFM69_SetRxTimeout(uint16_t timeout_ms);
bool libRFM69_SetRSSIThresholdTimeout(uint16_t timeout_ms);
void libRFM69_SetRSSIThreshold(int8_t threshold);
int8_t libRFM69_GetRSSIThreshold(void);
bool libRFM69_IsFIFOFull(void);
bool libRFM69_IsFIFONotEmpty(void);
bool libRFM69_IsFIFOLevel(void);
bool libRFM69_IsFIFOOverrun(void);
bool libRFM69_IsPacketSent(void);
bool libRFM69_IsPayloadReady(void);
bool libRFM69_IsCRCOk(void);
void libRFM69_EnableCRC(bool enable);
void libRFM69_EnableCRCAutoClear(bool enable);
void libRFM69_SetAddressFiltering(libRFM69_address_filtering_type filtering);
void libRFM69_SetNodeAddress(uint8_t node_address);
void libRFM69_SetBroadcastAddress(uint8_t broadcast_address);
void libRFM69_EnableHighPowerSetting(bool enable);
void libRFM69_SetPowerLevel(uint8_t power_level);
void libRFM69_SetPacketRxDelay(uint8_t val);
void libRFM69_RestartRx(void);

/**
 * Enable or disable automatic Rx restart.
 *
 * If enabled, Rx is restarted after PayloadReady occurred and packet has been
 * completely read from the FIFO.
 *
 * @param enable Enable/disable.
 */
void libRFM69_EnableAutoRxRestart(bool enable);
void libRFM69_SetClockOutFrequency(libRFM69_clkout_freq_type frequency);
void libRFM69_SetLNAGain(libRFM69_lna_gain_type gain);
libRFM69_lna_gain_type libRFM69_GetCurrentLNAGain(void);
void libRFM69_SetLNAInputImpedance(libRFM69_lna_zin_type impedance);
void libRFM69_EnableAFCLowBeta(bool enabled);
bool libRFM69_IsAFCLowBetaEnabled(void);
void libRFM69_EnableContinuousDAGC(bool enabled);

/**
 * Set the channel filter bandwidth.
 *
 * @param frequency Requested channel filter bandwidth in Hz.
 *
 * @return Actual channel filter bandwidth set.
 */
uint32_t libRFM69_SetChannelFilterBandwidth(uint32_t frequency);

/**
 * Set the DC cancellation cutoff frequency.
 *
 * The cutoff frequency is set in percent of the channel filter
 * bandwidth(RxBw).
 *
 * @param dcc_freq DC cancellation cutoff frequency.
 */
void libRFM69_SetDcCancellationCutoffFrequency(libRFM69_dcc_freq_type dcc_freq);
void libRFM69_WriteRegister(uint8_t address, uint8_t register_data);
void libRFM69_ReadRegister(uint8_t address, uint8_t *register_data);

#ifdef DEBUG_ENABLE
void libRFM69_DumpRegisterValues(void);
#endif

#endif
