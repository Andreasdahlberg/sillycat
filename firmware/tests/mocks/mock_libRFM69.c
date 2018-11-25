/**
 * @file   mock_libRFM69.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-11-25 (Last edit)
 * @brief  Mock functions for the RFM69HW driver.
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
#include <string.h>
#include "mock_libRFM69.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void __wrap_libRFM69_EnableHighPowerSetting(bool enable)
{
}

void __wrap_libRFM69_SetFIFOFillCondition(libRFM69_fifo_fill_condition_type fill_condition)
{
}

void __wrap_libRFM69_SetFrequencyDeviation(uint16_t frequency_deviation)
{
}

bool __wrap_libRFM69_IsPowerUpDone(void)
{
    mock_type(bool);
}

void __wrap_libRFM69_SetPacketFormat(libRFM69_packet_format_type packet_format)
{
}

void __wrap_libRFM69_SetBitRate(uint32_t bit_rate)
{
}

uint8_t __wrap_libRFM69_GetSyncWordSize(void)
{
    mock_type(uint8_t);
}

void __wrap_libRFM69_SetAddressFiltering(libRFM69_address_filtering_type filtering)
{
}

void __wrap_libRFM69_Init(void)
{
}

uint8_t __wrap_libRFM69_ReadFromFIFO(uint8_t *data, uint8_t max_length)
{
    uint8_t *mock_data_p = mock_ptr_type(uint8_t *);

    if (mock_data_p != NULL)
    {
        memcpy(data, mock_data_p, max_length);
    }

    mock_type(uint8_t);
}

void __wrap_libRFM69_SetLNAInputImpedance(libRFM69_lna_zin_type impedance)
{
}

bool __wrap_libRFM69_IsRxTimeoutFlagSet(void)
{
    mock_type(bool);
}

void __wrap_libRFM69_SetLNAGain(libRFM69_lna_gain_type gain)
{
}

void __wrap_libRFM69_SetNodeAddress(uint8_t node_address)
{
}

uint32_t __wrap_libRFM69_GetBitrate(void)
{
    mock_type(uint32_t);
}

void __wrap_libRFM69_EnableSyncWordGeneration(bool enabled)
{
}

bool __wrap_libRFM69_ClearFIFOOverrun(void)
{
    mock_type(bool);
}

void __wrap_libRFM69_SetMode(libRFM69_mode_type mode)
{
    check_expected(mode);
}

void __wrap_libRFM69_Reset(void)
{
}

uint16_t __wrap_libRFM69_GetPreambleLength(void)
{
    mock_type(uint16_t);
}

bool __wrap_libRFM69_IsRSSIThresholdExceeded(void)
{
    mock_type(bool);
}

uint32_t __wrap_libRFM69_GetBitRate(void)
{
    mock_type(uint32_t);
}

void __wrap_libRFM69_EnableCRC(bool enable)
{
}

void __wrap_libRFM69_WriteRegister(uint8_t address, uint8_t register_data)
{
}

void __wrap_libRFM69_EnableContinuousDAGC(bool enabled)
{
}

libRFM69_lna_gain_type __wrap_libRFM69_GetCurrentLNAGain(void)
{
    mock_type(libRFM69_lna_gain_type);
}

void __wrap_libRFM69_SetPreambleLength(uint16_t length)
{
}

bool __wrap_libRFM69_SetRSSIThresholdTimeout(uint16_t timeout_ms)
{
    false;
}

int8_t __wrap_libRFM69_GetOutputPower(void)
{
    mock_type(int8_t);
}

uint8_t __wrap_libRFM69_GetSyncWord(uint8_t *sync_word, uint8_t length)
{
    mock_type(uint8_t);
}

void __wrap_libRFM69_EnableAFCLowBeta(bool enabled)
{
}

bool __wrap_libRFM69_IsRxReady(void)
{
    mock_type(bool);
}

bool __wrap_libRFM69_IsPayloadReady(void)
{
    mock_type(bool);
}

void __wrap_libRFM69_SetTXStartCondition(libRFM69_tx_start_condition_type start_condition)
{
}

void __wrap_libRFM69_InitHW(void)
{
}

void __wrap_libRFM69_RestartRx(void)
{
    function_called();
}

void __wrap_libRFM69_Update(void)
{
}

void __wrap_libRFM69_SetCarrierFrequency(uint32_t frequency)
{
}

void __wrap_libRFM69_SetPowerAmplifierMode(uint8_t mode)
{
}

uint8_t __wrap_libRFM69_GetPowerAmplifierMode(void)
{
    mock_type(uint8_t);
}

void __wrap_libRFM69_EnableCRCAutoClear(bool enable)
{
}

bool __wrap_libRFM69_SetRxTimeout(uint16_t timeout_ms)
{
    true;
}

bool __wrap_libRFM69_IsAFCLowBetaEnabled(void)
{
    mock_type(bool);
}

bool __wrap_libRFM69_SetSyncWord(const uint8_t *sync_word, uint8_t length)
{
    true;
}

void __wrap_libRFM69_EnableListenMode(bool enabled)
{
}

bool __wrap_libRFM69_IsFIFOOverrun(void)
{
    mock_type(bool);
}

void __wrap_libRFM69_SetBroadcastAddress(uint8_t broadcast_address)
{
}

uint8_t __wrap_libRFM69_GetChipVersion(void)
{
    mock_type(uint8_t);
}

bool __wrap_libRFM69_IsFIFONotEmpty(void)
{
    mock_type(bool);
}

bool __wrap_libRFM69_IsModeReady(void)
{
    mock_type(bool);
}

void __wrap_libRFM69_SetClockOutFrequency(libRFM69_clkout_freq_type frequency)
{
}

void __wrap_libRFM69_EnableSequencer(bool enable)
{
}

bool __wrap_libRFM69_IsFIFOFull(void)
{
    mock_type(bool);
}

uint8_t __wrap_libRFM69_WriteToFIFO(const uint8_t *data, uint8_t length)
{
    mock_type(uint8_t);
}

void __wrap_libRFM69_SetPacketRxDelay(uint8_t val)
{
}

libRFM69_mode_type __wrap_libRFM69_GetMode(void)
{
    mock_type(libRFM69_mode_type);
}

bool __wrap_libRFM69_IsCRCOk(void)
{
    mock_type(bool);
}

bool __wrap_libRFM69_IsFIFOLevel(void)
{
    mock_type(bool);
}

void __wrap_libRFM69_ClearFIFO(void)
{
    function_called();
}

void __wrap_libRFM69_SetDataMode(libRFM69_data_mode_type data_mode)
{
}

void __wrap_libRFM69_SetModulationShaping(uint8_t modulation_shaping)
{
}

int8_t __wrap_libRFM69_GetRSSI(void)
{
    mock_type(int8_t);
}

void __wrap_libRFM69_EnableOCP(bool enabled)
{
}

void __wrap_libRFM69_ReadRegister(uint8_t address, uint8_t *register_data)
{
}

int8_t __wrap_libRFM69_GetRSSIThreshold(void)
{
    mock_type(int8_t);
}

void __wrap_libRFM69_SetAESKey(const uint8_t *aes_key)
{
}

void __wrap_libRFM69_SetRSSIThreshold(int8_t threshold)
{
}

void __wrap_libRFM69_SetPowerLevel(uint8_t power_level)
{
}

void __wrap_libRFM69_EnableEncryption(bool enable)
{
}

void __wrap_libRFM69_SetModulationType(libRFM69_modulation_type_type modulation_type)
{
}

libRFM69_modulation_type_type __wrap_libRFM69_GetModulationType(void)
{
    mock_type(libRFM69_modulation_type_type);
}

bool __wrap_libRFM69_IsPacketSent(void)
{
    mock_type(bool);
}

bool __wrap_libRFM69_IsTxReady(void)
{
    mock_type(bool);
}

void __wrap_libRFM69_CalibrateRCOscillator(void)
{
}

bool __wrap_libRFM69_WaitForModeReady(void)
{
    return true;
}

bool __wrap_libRFM69_IsHighPowerEnabled(void)
{
    mock_type(bool);
}

void __wrap_libRFM69_GetTemperature(int8_t *temperature)
{
}

void __wrap_libRFM69_SetSyncWordSize(uint8_t size)
{
}

uint32_t __wrap_libRFM69_SetChannelFilterBandwidth(uint32_t frequency)
{
    return frequency * 2 + 1;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

