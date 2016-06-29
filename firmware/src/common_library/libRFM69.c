/**
 * @file   libRFM69.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2016-06-29 (Last edit)
 * @brief  Implementation of RFM69HW-library.
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

#include "libRFM69.h"
#include "libSPI.h"
#include "libDebug.h"
#include "RFM69Registers.h"
#include "RFM69_HAL.h"

#include "Timer.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define REG_OPMODE_MODE_MASK 0x1C

#define REG_DATA_MODUL_DATA_MODE_MASK           0x60
#define REG_DATA_MODUL_MODULATION_TYPE_MASK     0x18
#define REG_DATA_MODUL_MODULATION_SHAPING_MASK  0x03

#define REG_PA_LEVEL_PA_MASK 0xE0
#define REG_PA_LEVEL_POUT_MASK 0x1F

#define RFM_FXOSC 32000000 //32MHz
#define RFM_FSTEP (float)61.03515625 // FSTEP = FXOSC / 2^19

#define WAIT_TIMEOUT_MS 10
#define POR_TIME_MS 10
#define RESET_TIMING_US 110

#define TEMPERATURE_CALIBRATION_OFFSET (-90)

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

typedef enum
{
    REG_IRQFLAGS2_BIT_CRCOK = 1,
    REG_IRQFLAGS2_BIT_PAYLOADREADY,
    REG_IRQFLAGS2_BIT_PACKETSENT,
    REG_IRQFLAGS2_BIT_FIFOOVERRUN,
    REG_IRQFLAGS2_BIT_FIFOLEVEL,
    REG_IRQFLAGS2_BIT_FIFONOTEMPTY,
    REG_IRQFLAGS2_BIT_FIFOFULL,
} irqflags2_bit_type;

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static uint32_t reset_time_ms;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

static bool IsBitSetInRegister(uint8_t address, uint8_t bit);

static void PreCallback(void);
static void PostCallback(void);

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

///
/// @brief Init libRFM69.
///
/// @param  None
/// @return None
///
void libRFM69_Init(void)
{
    libRFM69_InitHW();
    libRFM69_Reset();

    //TODO: Remove
    _delay_ms(15);
    return;
}

///
/// @brief Init required IO-pins. This function should be called as
///        early as possible in a systems with several SPI-devices.
///
/// @param  None
/// @return None
///
void libRFM69_InitHW(void)
{
    InitReset();

    InitCS();
    PullCS(); //Pull SS high to release device

    InitIO();

    return;
}

///
/// @brief Update the internal state of libRFM69. NOT IMPLEMENTED
///
/// @param  None
/// @return None
///
void libRFM69_Update(void)
{
    sc_assert_fail();
    return;
}

///
/// @brief Reset the RFM69 module.
///
/// Use libRFM69_IsPowerUpDone() to check if the module is ready to use again.
///
/// @param  None
/// @return None
///
void libRFM69_Reset(void)
{
    PullReset();
    _delay_us(RESET_TIMING_US);
    ReleaseReset();

    reset_time_ms = Timer_GetMilliseconds();
    return;
}

///
/// @brief Check if RFM69HW module is ready to use after power on/reset.
///
/// @param  None
/// @return None
///
bool libRFM69_IsPowerUpDone(void)
{
    return (Timer_TimeDifference(reset_time_ms) > POR_TIME_MS);
}

//TODO: Use remove these functions and use macros!
bool libRFM69_IsFIFOFull(void)
{
    return IsBitSetInRegister(REG_IRQFLAGS2, REG_IRQFLAGS2_BIT_FIFOFULL);
}

bool libRFM69_IsFIFONotEmpty(void)
{
    return IsBitSetInRegister(REG_IRQFLAGS2, REG_IRQFLAGS2_BIT_FIFONOTEMPTY);
}

bool libRFM69_IsFIFOLevel(void)
{
    return IsBitSetInRegister(REG_IRQFLAGS2, REG_IRQFLAGS2_BIT_FIFOLEVEL);
}

bool libRFM69_IsFIFOOverrun(void)
{
    return IsBitSetInRegister(REG_IRQFLAGS2, REG_IRQFLAGS2_BIT_FIFOOVERRUN);
}

bool libRFM69_IsPacketSent(void)
{
    return IsBitSetInRegister(REG_IRQFLAGS2, REG_IRQFLAGS2_BIT_PACKETSENT);
}

bool libRFM69_IsPayloadReady(void)
{
    return IsBitSetInRegister(REG_IRQFLAGS2, REG_IRQFLAGS2_BIT_PAYLOADREADY);
}

bool libRFM69_IsCRCOk(void)
{
    return IsBitSetInRegister(REG_IRQFLAGS2, REG_IRQFLAGS2_BIT_CRCOK);
}

bool libRFM69_IsOCPEnabled(void)
{
    return IsBitSetInRegister(REG_OCP, 4);
}

///
/// @brief Enabled/disabled CRC auto clear
///
/// @param  enabled Enabled(True): Clear FIFO and restart new packet reception.
///                                No PayloadReady interrupt issued.
///                 Disabled(False): Do not clear FIFO. PayloadReady interrupt issued.
/// @return None
///
void libRFM69_EnableCRCAutoClear(bool enable)
{
    uint8_t register_content;

    libRFM69_ReadRegister(REG_PACKETCONFIG1, &register_content);
    register_content = (register_content & 0xF7) | ((uint8_t)!enable << 3);
    libRFM69_WriteRegister(REG_PACKETCONFIG1, register_content);
    return;
}

///
/// @brief Enabled/disabled CRC calculation for both Tx and Rx.
///
/// @param  enabled Enabled/disable
/// @return None
///
void libRFM69_EnableCRC(bool enable)
{
    uint8_t register_content;

    libRFM69_ReadRegister(REG_PACKETCONFIG1, &register_content);
    register_content = (register_content & 0xEF) | ((uint8_t)enable << 4);
    libRFM69_WriteRegister(REG_PACKETCONFIG1, register_content);
    return;
}

///
/// @brief Write data to FIFO.
///
/// @param  data Pointer to buffer with data to write.
/// @param  length Number of bytes to write.
/// @return uint8_t Number of bytes written.
///
uint8_t libRFM69_WriteToFIFO(const uint8_t *data, uint8_t length)
{
    sc_assert(length <= RFM_FIFO_SIZE);

    uint8_t index;

    for (index = 0; index < length; ++index)
    {
        if (libRFM69_IsFIFOFull())
        {
            ERROR("FIFO is full");
            break;
        }

        libRFM69_WriteRegister(REG_FIFO, data[index]);
    }

    return index;
}

///
/// @brief Read data from FIFO.
///
/// @param  data Pointer to buffer where the FIFO data will be stored.
/// @param  max_length Max number of bytes to read.
/// @return uint8_t Number of bytes read.
///
uint8_t libRFM69_ReadFromFIFO(uint8_t *data, uint8_t max_length)
{
    uint8_t index = 0;

    while (libRFM69_IsFIFONotEmpty() && index < max_length)
    {
        libRFM69_ReadRegister(REG_FIFO, &data[index]);
        ++index;
    }

    return index;
}

void libRFM69_ClearFIFO(void)
{
    INFO("Clear FIFO");
    libRFM69_WriteRegister(REG_IRQFLAGS2, (1 << REG_IRQFLAGS2_BIT_FIFOOVERRUN));
    return;
}

///
/// @brief Get the active operating mode.
///
/// @param  None
/// @return libRFM69_mode_type Operating mode (0-4)
///
libRFM69_mode_type libRFM69_GetMode(void)
{
    uint8_t register_content;

    libRFM69_ReadRegister(REG_OPMODE, &register_content);
    return (libRFM69_mode_type)((register_content & REG_OPMODE_MODE_MASK) >> 2);
}

void libRFM69_SetMode(libRFM69_mode_type mode)
{
    uint8_t register_content;

    libRFM69_ReadRegister(REG_OPMODE, &register_content);
    register_content = (register_content & ~REG_OPMODE_MODE_MASK) | (mode << 2);
    libRFM69_WriteRegister(REG_OPMODE, register_content);

    return;
}

bool libRFM69_WaitForModeReady(void)
{
    bool status = true;
    uint32_t timeout_timer = Timer_GetMilliseconds();

    while (!libRFM69_IsModeReady())
    {
        if (Timer_TimeDifference(timeout_timer) > WAIT_TIMEOUT_MS)
        {
            ERROR("Timeout!");
            status = false;
            break;
        }
    }
    return status;
}

bool libRFM69_IsModeReady(void)
{
    uint8_t register_content;

    libRFM69_ReadRegister(REG_IRQFLAGS1, &register_content);
    return (register_content & RF_IRQFLAGS1_MODEREADY) == RF_IRQFLAGS1_MODEREADY;
}

bool libRFM69_IsRxReady(void)
{
    uint8_t register_content;

    libRFM69_ReadRegister(REG_IRQFLAGS1, &register_content);
    return (register_content & RF_IRQFLAGS1_RXREADY) == RF_IRQFLAGS1_RXREADY;
}

bool libRFM69_IsTxReady(void)
{
    return IsBitSetInRegister(REG_IRQFLAGS1, 5);
}

bool libRFM69_IsRSSIThresholdExceeded(void)
{
    uint8_t register_content;

    libRFM69_ReadRegister(REG_IRQFLAGS1, &register_content);
    return (register_content & RF_IRQFLAGS1_RSSI) == RF_IRQFLAGS1_RSSI;
}

///
/// @brief Check if RX timeout flag is set.
///
/// Set when a Rx timeout occurs and cleared when leaving Rx or FIFO is emptied.
///
/// @return None
/// @return bool True if timeout has occurred, otherwise false.
///
bool libRFM69_IsRxTimeoutFlagSet(void)
{
    return IsBitSetInRegister(REG_IRQFLAGS1, 2);
}

///
/// @brief Set the RSSI trigger level for RSSI interrupt.
///
/// @return uint8_t RSSI threshold
/// @return None
///
void libRFM69_SetRSSIThreshold(int8_t threshold)
{
    libRFM69_WriteRegister(REG_RSSITHRESH, (uint8_t)(threshold * -1) << 1);
    return;
}

///
/// @brief Get the RSSI trigger level for RSSI interrupt.
///
/// @param  None
/// @return int8_t RSSI threshold
///
int8_t libRFM69_GetRSSIThreshold(void)
{
    uint8_t register_content;

    libRFM69_ReadRegister(REG_RSSITHRESH, &register_content);

    return ((int8_t)(register_content >> 1) * -1);
}


void libRFM69_SetCarrierFrequency(uint32_t frequency)
{
    uint32_t frequency_value = frequency / RFM_FSTEP;

    DEBUG("Freq: %lu\r\n", frequency);
    DEBUG("Freq value: 0x%03lX\r\n", frequency_value);

    libRFM69_WriteRegister(REG_FRFMSB, (frequency_value >> 16) & 0xFF);
    libRFM69_WriteRegister(REG_FRFMID, (frequency_value >> 8) & 0xFF);
    libRFM69_WriteRegister(REG_FRFLSB, frequency_value & 0xFF);

    return;
}

void libRFM69_EnableListenMode(bool enable)
{
    uint8_t register_content;

    libRFM69_ReadRegister(REG_OPMODE, &register_content);
    register_content = (register_content & 0xBF) | (uint8_t)enable;
    libRFM69_WriteRegister(REG_OPMODE, register_content);

    return;
}

void libRFM69_EnableSequencer(bool enable)
{
    uint8_t register_content;

    libRFM69_ReadRegister(REG_OPMODE, &register_content);
    register_content = (register_content & 0x7F) | ((uint8_t)!enable << 7);
    libRFM69_WriteRegister(REG_OPMODE, register_content);

    return;
}

void libRFM69_EnableEncryption(bool enable)
{
    uint8_t register_content;

    libRFM69_ReadRegister(REG_PACKETCONFIG2, &register_content);
    SetBit(PACKETCONFIG2_BIT_AESON, enable, &register_content);
    libRFM69_WriteRegister(REG_PACKETCONFIG2, register_content);

    return;
}

void libRFM69_SetPacketFormat(libRFM69_packet_format_type packet_format)
{
    uint8_t register_content;

    libRFM69_ReadRegister(REG_PACKETCONFIG1, &register_content);
    SetBit(PACKETCONFIG1_BIT_PCKFORMAT, (bool)packet_format,
           &register_content);

    libRFM69_WriteRegister(REG_PACKETCONFIG1, register_content);
    return;
}

void libRFM69_SetTXStartCondition(libRFM69_tx_start_condition_type
                                  start_condition)
{
    uint8_t register_content;

    libRFM69_ReadRegister(REG_FIFOTHRESH, &register_content);
    SetBit(0x07, (bool)start_condition, &register_content);

    libRFM69_WriteRegister(REG_FIFOTHRESH, register_content);
    return;
}

///
/// @brief Force Rx restart.
///
/// @param  None
/// @return None
///
void libRFM69_RestartRx(void)
{
    uint8_t register_content;

    libRFM69_ReadRegister(REG_PACKETCONFIG2, &register_content);
    register_content = (register_content | (1 << 2));

    libRFM69_WriteRegister(REG_PACKETCONFIG2, register_content);
    return;
}

///
/// @brief Set the inter packet rx delay. Defines the delay between
///        FIFO empty and the start of a new RSSI phase for next
///        packet. Must match the transmitters PA ramp-down time.
///
/// @param  val Inter packet Rx delay
/// @return None
///
void libRFM69_SetPacketRxDelay(uint8_t val)
{
    uint8_t register_content;

    libRFM69_ReadRegister(REG_PACKETCONFIG2, &register_content);
    register_content = (register_content & 0x0F) | (val << 4);

    libRFM69_WriteRegister(REG_PACKETCONFIG2, register_content);
    return;
}

///
/// @brief Get the chip temperature. This function changes the mode to
///        standby while the measurement is in progress, the previous mode
///        is then restored.
///
/// @param  *temperature Pointer to variable where the temperature will be stored.
/// @return None
///
void libRFM69_GetTemperature(int8_t *temperature)
{
    uint8_t register_data;
    libRFM69_mode_type active_mode;

    //Save the active mode so it can be restored after the measurement is done.
    active_mode = libRFM69_GetMode();

    //Enter standby mode, temperature can only be read in standby or synthesizer mode.
    libRFM69_SetMode(RFM_STANDBY);
    libRFM69_WaitForModeReady();

    libRFM69_WriteRegister(REG_TEMP1, RF_TEMP1_MEAS_START);

    //Wait for temperature measurement to finish. Less then 100 uS.
    do
    {
        libRFM69_ReadRegister(REG_TEMP1, &register_data);
    }
    while (register_data & RF_TEMP1_MEAS_RUNNING);

    libRFM69_ReadRegister(REG_TEMP2, &register_data);
    *temperature = (int8_t)(~register_data + TEMPERATURE_CALIBRATION_OFFSET);

    //Restore mode.
    libRFM69_SetMode(active_mode);
    libRFM69_WaitForModeReady();

    return;
}

void libRFM69_SetBitRate(uint32_t bit_rate)
{
    uint16_t bit_rate_value = (uint16_t)(RFM_FXOSC / bit_rate);

    libRFM69_WriteRegister(REG_BITRATEMSB, (uint8_t)(bit_rate_value >> 8));
    libRFM69_WriteRegister(REG_BITRATELSB, (uint8_t)(bit_rate_value));

    return;
}

uint32_t libRFM69_GetBitrate(void)
{
    uint16_t bit_rate_value;
    uint8_t tmp;

    libRFM69_ReadRegister(REG_BITRATEMSB, &tmp);
    bit_rate_value = (uint16_t)(tmp << 8);

    libRFM69_ReadRegister(REG_BITRATELSB, &tmp);
    bit_rate_value |= tmp;

    if (bit_rate_value > 0)
    {
        return (RFM_FXOSC / bit_rate_value);
    }
    return 0;
}

///
/// @brief Set the address filtering type
///
/// @param  filtering The filtering type,
///         RFM_ADDRESS_FILTER_NONE: None (off)
///         RFM_ADDRESS_FILTER_ADDRESS: Address must match node address
///         RFM_ADDRESS_FILTER_ADDRESS_BROADCAST: Address must match node address or broadcast address

/// @return None
///
void libRFM69_SetAddressFiltering(libRFM69_address_filtering_type filtering)
{
    sc_assert(filtering < 3);

    uint8_t register_content;

    libRFM69_ReadRegister(REG_PACKETCONFIG1, &register_content);
    register_content = (register_content & 0xF9) |
                       (filtering << 1);

    libRFM69_WriteRegister(REG_PACKETCONFIG1, register_content);
    return;
}

///
/// @brief Set the node address.
///
/// @param  node_address Address
/// @return None
///
void libRFM69_SetNodeAddress(uint8_t node_address)
{
    libRFM69_WriteRegister(REG_NODEADRS, node_address);
    return;
}

///
/// @brief Set the broadcast address.
///
/// Broadcast address is used when with address filtering to send
/// to all devices in the network.
///
/// @param  broadcast_address Address
/// @return None
///
void libRFM69_SetBroadcastAddress(uint8_t broadcast_address)
{
    libRFM69_WriteRegister(REG_BROADCASTADRS, broadcast_address);
    return;
}

///
/// @brief Set the data mode.
///
/// Data processing mode:
/// 0 - Packet mode
/// 1 - reserved
/// 2 - Continuous mode with bit synchronizer
/// 3 - Continuous mode without bit synchronizer
///
/// @param  data_mode Data mode to set.
/// @return None
///
void libRFM69_SetDataMode(libRFM69_data_mode_type data_mode)
{
    uint8_t register_content;

    libRFM69_ReadRegister(REG_DATAMODUL, &register_content);
    register_content = (register_content & ~REG_DATA_MODUL_DATA_MODE_MASK) |
                       (data_mode << 5);

    libRFM69_WriteRegister(REG_DATAMODUL, register_content);
    return;
}

///
/// @brief Set the modulation type.
///
/// @param  modulation_type FSK or OOK.
/// @return None
///
void libRFM69_SetModulationType(libRFM69_modulation_type_type modulation_type)
{
    sc_assert(modulation_type == RFM_FSK || modulation_type == RFM_OOK);
    uint8_t register_content;

    libRFM69_ReadRegister(REG_DATAMODUL, &register_content);
    register_content = (register_content & ~REG_DATA_MODUL_MODULATION_TYPE_MASK) |
                       (modulation_type << 3);

    libRFM69_WriteRegister(REG_DATAMODUL, register_content);
    return;
}

///
/// @brief Set the modulation shaping.
///
/// Data shaping:
/// in FSK:
/// 0 - no shaping
/// 1 - Gaussian filter, BT = 1.0
/// 2 - Gaussian filter, BT = 0.5
/// 3 - Gaussian filter, BT = 0.3
/// in OOK:
/// 0 - no shaping
/// 1 - filtering with fcutoff = BR
/// 2 - filtering with fcutoff = 2*BR
/// 3 - reserved
///
/// @param  modulation_shaping The modulation shaping value.
/// @return None
///
void libRFM69_SetModulationShaping(uint8_t modulation_shaping)
{
    sc_assert(modulation_shaping < 0x04);
    uint8_t register_content;

    DEBUG("Modulation shaping: 0x%02X\r\n", modulation_shaping);

    libRFM69_ReadRegister(REG_DATAMODUL, &register_content);
    register_content = (register_content & ~REG_DATA_MODUL_MODULATION_SHAPING_MASK)
                       | modulation_shaping;

    libRFM69_WriteRegister(REG_DATAMODUL, register_content);
    return;
}

///
/// @brief Set the frequency deviation.
///
/// @param  frequency_deviation The frequency deviation in hertz.
/// @return None
///
void libRFM69_SetFrequencyDeviation(uint16_t frequency_deviation)
{
    //Minimum frequency according to the RFM69HW datasheet.
    sc_assert(frequency_deviation > 600);

    uint16_t frequency_deviation_value = (uint16_t)((float)frequency_deviation /
                                         RFM_FSTEP);

    DEBUG("Freq deviation: %u\r\n", frequency_deviation);
    DEBUG("Freq deviation value: 0x%04X\r\n", frequency_deviation_value);

    libRFM69_WriteRegister(REG_FDEVMSB, ((frequency_deviation_value >> 8) & 0xFF));
    libRFM69_WriteRegister(REG_FDEVLSB, (frequency_deviation_value & 0xFF));

    return;
}

///
/// @brief Set the power amplifier mode.
///
/// @param  mode The power amplifier mode, value between 2 and 4.
/// @return None
///
void libRFM69_SetPowerAmplifierMode(uint8_t mode)
{
    sc_assert(mode > 0x01 && mode < 0x05);
    uint8_t register_content;

    DEBUG("Power amplifier mode: 0x%02X\r\n", mode);

    libRFM69_ReadRegister(REG_PALEVEL, &register_content);

    register_content &= ~REG_PA_LEVEL_PA_MASK;
    register_content |= (mode << 5);

    libRFM69_WriteRegister(REG_PALEVEL, register_content);
    return;
}

///
/// @brief Set the power level. The resulting output power is dependent on
///        the power amplifier mode and if the high power setting is active.
///
/// @param  power_level Value between 0 and 31.
/// @return None
///
void libRFM69_SetPowerLevel(uint8_t power_level)
{
    sc_assert(power_level <= 31);

    uint8_t register_content;
    libRFM69_ReadRegister(REG_PALEVEL, &register_content);

    register_content &= ~REG_PA_LEVEL_POUT_MASK;
    register_content |= power_level;

    libRFM69_WriteRegister(REG_PALEVEL, register_content);
    return;
}

///
/// @brief Check if the high power setting is enabled.
///
/// @param  None
/// @return true if enabled, otherwise false.
///
bool libRFM69_IsHighPowerEnabled(void)
{
    uint8_t register_content;

    libRFM69_ReadRegister(REG_TESTPA1, &register_content);

    if (register_content == 0x5D)
    {
        libRFM69_ReadRegister(REG_TESTPA2, &register_content);
        return (register_content == 0x7C);
    }

    return false;
}

///
/// @brief Enabled the high power setting. OCP must first be disabled
///        for high power setting to work properly.
///
/// @param  enable Enable/disable
/// @return None
///
void libRFM69_EnableHighPowerSetting(bool enable)
{
    if (enable == true)
    {

#ifdef DEBUG_ENABLE
        if (libRFM69_IsOCPEnabled() == true)
        {
            WARNING("OCP must be disabled while in high power mode");
        }
#endif

        libRFM69_WriteRegister(REG_TESTPA1, 0x5D);
        libRFM69_WriteRegister(REG_TESTPA2, 0x7C);
    }
    else
    {
        libRFM69_WriteRegister(REG_TESTPA1, 0x55);
        libRFM69_WriteRegister(REG_TESTPA2, 0x70);
    }

    return;
}

///
/// @brief Get the output power.
///
/// @param  None
/// @return int8_t Output power in dBm, -18 to 20.
///
int8_t libRFM69_GetOutputPower(void)
{
    int8_t pout = 0;
    uint8_t register_content;

    libRFM69_ReadRegister(REG_PALEVEL, &register_content);
    register_content &= REG_PA_LEVEL_POUT_MASK;

    switch (libRFM69_GetPowerAmplifierMode())
    {
        case RFM_PWR_1:
        case RFM_PWR_2:
            pout = -18 + (int8_t)register_content;
            break;

        case RFM_PWR_3_4:
            if (libRFM69_IsHighPowerEnabled() == true)
            {
                pout = -11 + (int8_t)register_content;
            }
            else
            {
                pout = -14 + (int8_t)register_content;
            }
            break;

        default:
            ERROR("Invalid PA-mode");
            break;
    }

    return pout;
}

///
/// @brief Get the power amplifier mode.
///
/// @param  None
/// @return uint8_t The power amplifier mode.
///         0x04, PA0 output on pin RFIO,                                       -18 to +13 dBm
///         0x02, PA1 enabled on pin PA_BOOST,                                  -2 to +13 dBm
///         0x03, PA1 and PA2 combined on pin PA_BOOST,                         +2 to +17 dBm
///         0x03, PA1+PA2 on PA_BOOST with high output power +20dBm settings,   +5 to +20 dBm
///
uint8_t libRFM69_GetPowerAmplifierMode(void)
{
    uint8_t register_content;

    libRFM69_ReadRegister(REG_PALEVEL, &register_content);
    return (register_content >> 5);
}

///
/// @brief Enable/disable the PA over current protection. To ensure correct
///        operation at the highest power levels, make sure to adjust the OCP
///        limit accordingly, except above +18dBm where it must be disabled.
///
/// @param  None
/// @return bool true if overrun flag was set, otherwise false.
///
void libRFM69_EnableOCP(bool enabled)
{
    uint8_t register_content;

    libRFM69_ReadRegister(REG_OCP, &register_content);
    SetBit(4, enabled, &register_content);

    libRFM69_WriteRegister(REG_OCP, register_content);
    return;
}

///
/// @brief Get the RSSI-value. This function trigger a RSSI measurement and
///        blocks until the RSSI-done flag is set.
///
/// @param  None
/// @return int8_t The RSSI-value.
///
int8_t libRFM69_GetRSSI(void)
{
    uint8_t register_content;

    //Start RSSI measurement
    libRFM69_WriteRegister(REG_RSSICONFIG, 0x01);

    uint32_t timer = Timer_GetMilliseconds();
    do
    {
        libRFM69_ReadRegister(REG_RSSICONFIG, &register_content);

        if (Timer_TimeDifference(timer) > 500)
        {
            ERROR("Timeout while waiting for RSSI.");
            return 0;
        }
    }
    while (register_content == 0);

    libRFM69_ReadRegister(REG_RSSIVALUE, &register_content);
    return (-1 * (int8_t)(register_content >> 1));
}

///
/// @brief Clear any active FIFO overrun flag.
///
/// @param  None
/// @return bool true if overrun flag was set, otherwise false.
///
bool libRFM69_ClearFIFOOverrun(void)
{
    uint8_t register_content;

    libRFM69_ReadRegister(REG_IRQFLAGS2, &register_content);

    bool status;
    status = IsBitSet(REG_IRQFLAGS2_BIT_FIFOOVERRUN, &register_content);

    if (status)
    {
        SetBit(REG_IRQFLAGS2_BIT_FIFOOVERRUN, true, &register_content);
        libRFM69_WriteRegister(REG_IRQFLAGS2, register_content);
    }
    return status;
}

///
/// @brief Calibrate the internal RC oscillator. This oscillator is automatically
///        calibrated at the device power-up but for applications with large
///        temperature variations, and for which the power supply is never removed,
///        RC calibration can be performed upon user request. Use the function
///        libRFM69_IsRCCalibrationDone() to check if the calibration is done.
///
///        IMPORTENT: Not implemented
///
/// @param  None
/// @return None
///
void libRFM69_CalibrateRCOscillator(void)
{
    sc_assert_fail(); //Not implemented yet
    return;
}

///
/// @brief Set the sync word size.
///
/// @param  uint8_t Size of the sync word, (1-8).
/// @return None
///
void libRFM69_SetSyncWordSize(uint8_t size)
{
    sc_assert(size > 0 && size <= MAX_SYNC_WORD_SIZE);

    uint8_t register_content;

    libRFM69_ReadRegister(REG_SYNCCONFIG, &register_content);
    register_content &= (0xC7);

    register_content |= ((size - 1) << 3);
    libRFM69_WriteRegister(REG_SYNCCONFIG, register_content);

    return;
}

///
/// @brief Set the number of preamble bytes.
///
/// @param length Number of preamble bytes.
/// @return None
///
void libRFM69_SetPreambleLength(uint16_t length)
{
    libRFM69_WriteRegister(REG_PREAMBLEMSB, (length >> 8));
    libRFM69_WriteRegister(REG_PREAMBLELSB, (length & 0x00FF));
    return;
}

///
/// @brief Get the number of preamble bytes.
///
/// @param None
/// @return uint16_t Number of preamble bytes.
///
uint16_t libRFM69_GetPreambleLength(void)
{
    uint16_t length;
    uint8_t register_content;

    libRFM69_ReadRegister(REG_PREAMBLEMSB, &register_content);
    length = (uint16_t)(register_content << 8);

    libRFM69_ReadRegister(REG_PREAMBLELSB, &register_content);
    length |= register_content;

    return length;
}

///
/// @brief Get the current sync word size.
///
/// @param None
/// @return uint8_t Size of the sync word, (1-8).
///
uint8_t libRFM69_GetSyncWordSize(void)
{
    uint8_t register_content;

    libRFM69_ReadRegister(REG_SYNCCONFIG, &register_content);

    //From RFM69HW datasheet, table 28: sync word size = RegSyncConfig[5:2] + 1
    return ((register_content >> 3) & 0x07) + 1;
}

///
/// @brief Set a new sync word. The length must match the configured
///        sync word size.
///
/// @param sync_world* Pointer to new sync word.
/// @param length Length of the sync word.
/// @return bool true if the new sync word was valid, otherwise false.
///
bool libRFM69_SetSyncWord(const uint8_t *sync_word, uint8_t length)
{
    //Validate sync word.
    if (length != libRFM69_GetSyncWordSize())
    {
        ERROR("Invalid sync word length: %u", length);
        return false;
    }

    //From RFM69HW datasheet, note on page 56: sync
    //word values equal to 0x00 is forbidden.
    uint8_t index;
    for (index = 0; index < length; ++index)
    {
        if (sync_word[index] == 0x00)
        {
            ERROR("Invalid sync word value: 0x00");
            return false;
        }
    }

    //Write new sync word if all checks passed.
    for (index = 0; index < length; ++index)
    {
        libRFM69_WriteRegister(REG_SYNCVALUE1 + index, sync_word[index]);
    }

    return true;
}

///
/// @brief Get the current sync word.
///
/// @param sync_world* Pointer to buffer where the sync word will be stored.
/// @param buffer_size Size of the supplied buffer.
/// @return uint8_t Number of bytes written to buffer.
///
uint8_t libRFM69_GetSyncWord(uint8_t *sync_word, uint8_t buffer_size)
{
    uint8_t sync_word_size;
    uint8_t index = 0;

    sync_word_size = libRFM69_GetSyncWordSize();

    if (buffer_size >= sync_word_size)
    {
        for (; index < sync_word_size; ++index)
        {
            libRFM69_ReadRegister(REG_SYNCVALUE1 + index, &sync_word[index]);
        }
    }
    return index;
}

///
/// @brief Get the version code of the chip. Bits 7-4 give the full revision
///        number. Bits 3-0 give the metal mask revision number.
///
/// @param None
/// @return uint8_t Version code
///
uint8_t libRFM69_GetChipVersion(void)
{
    uint8_t register_content;
    libRFM69_ReadRegister(REG_VERSION, &register_content);

    return register_content;
}

#ifdef DEBUG_ENABLE
///
/// @brief Print all register values
///
/// @param None
/// @return None
///
void libRFM69_DumpRegisterValues(void)
{
    uint8_t register_address;
    uint8_t register_content;

    DEBUG("****Register content****\r\n");

    for (register_address = 0; register_address < 0x4F; ++register_address)
    {
        libRFM69_ReadRegister(register_address, &register_content);
        DEBUG("REG ADDR: 0x%02x	REG VALUE: 0x%02x\r\n", register_address,
              register_content);
    }

    libRFM69_ReadRegister(REG_TESTLNA, &register_content);
    DEBUG("REG ADDR: 0x%02x	REG VALUE: 0x%02x\r\n", REG_TESTLNA, register_content);

    libRFM69_ReadRegister(REG_TESTPA1, &register_content);
    DEBUG("REG ADDR: 0x%02x	REG VALUE: 0x%02x\r\n", REG_TESTPA1, register_content);

    libRFM69_ReadRegister(REG_TESTPA2, &register_content);
    DEBUG("REG ADDR: 0x%02x	REG VALUE: 0x%02x\r\n", REG_TESTPA2, register_content);

    libRFM69_ReadRegister(REG_TESTDAGC, &register_content);
    DEBUG("REG ADDR: 0x%02x	REG VALUE: 0x%02x\r\n", REG_TESTDAGC, register_content);

    libRFM69_ReadRegister(REG_TESTAFC, &register_content);
    DEBUG("REG ADDR: 0x%02x	REG VALUE: 0x%02x\r\n", REG_TESTAFC, register_content);
    return;
}
#endif

///
/// @brief Enable or disable sync word generation.
///
/// @param enabled Enable/disable
/// @return None
///
void libRFM69_EnableSyncWordGeneration(bool enabled)
{
    uint8_t register_content;

    libRFM69_ReadRegister(REG_SYNCCONFIG, &register_content);
    SetBit(7, enabled, &register_content);
    libRFM69_WriteRegister(REG_SYNCCONFIG, register_content);

    return;
}

///
/// @brief Set the FIFO fill condition.
///
/// @param register fill_condition The fill condition, AUTO or MAN.
/// @return None
///
void libRFM69_SetFIFOFillCondition(libRFM69_fifo_fill_condition_type
                                   fill_condition)
{
    uint8_t register_content;

    libRFM69_ReadRegister(REG_SYNCCONFIG, &register_content);
    SetBit(6, (bool)fill_condition, &register_content);
    libRFM69_WriteRegister(REG_SYNCCONFIG, register_content);

    return;
}

///
/// @brief Set frequency of the clock output.
///
/// @param frequency Frequency to set.
/// @return None
///
void libRFM69_SetClockOutFrequency(libRFM69_clkout_freq_type frequency)
{
    sc_assert(frequency <= RFM_CLKOUT_OFF);

    uint8_t register_content;
    libRFM69_ReadRegister(REG_DIOMAPPING2, &register_content);
    libRFM69_WriteRegister(REG_DIOMAPPING2, (register_content & 0xF8) | frequency);
}

///
/// @brief Set LNA gain setting.
///
/// LNA gain setting:
/// AUTO = gain set by the internal AGC loop
/// G1 = highest gain
/// G2 = highest gain - 6 dB
/// G3 = highest gain - 12 dB
/// G4 = highest gain - 24 dB
/// G5 = highest gain - 36 dB
/// G6 = highest gain - 48 dB
///
/// @param gain Gain to set.
/// @return None
///
void libRFM69_SetLNAGain(libRFM69_lna_gain_type gain)
{
    sc_assert(gain <= RFM_LNA_GAIN_G6);

    uint8_t register_content;
    libRFM69_ReadRegister(REG_LNA, &register_content);
    libRFM69_WriteRegister(REG_LNA, (register_content & 0xF8) | gain);
}

///
/// @brief Set the LNA input impedance.
///
/// @param impedance Impedance to set.
/// @return None
///
void libRFM69_SetLNAInputImpedance(libRFM69_lna_zin_type impedance)
{
    sc_assert(impedance <= RFM_LNA_ZIN_200OHM);

    uint8_t register_content;
    libRFM69_ReadRegister(REG_LNA, &register_content);
    SetBit(7, impedance, &register_content);
    libRFM69_WriteRegister(REG_LNA, register_content);
}

///
/// @brief Enabled improved AFC routine for signals with modulation index
///        lower than 2.
///
/// @param enabled True or False
/// @return None
///
void libRFM69_EnableAFCLowBeta(bool enabled)
{
    uint8_t register_content;

    libRFM69_ReadRegister(REG_AFCCTRL, &register_content);
    SetBit(5, enabled, &register_content);
    libRFM69_WriteRegister(REG_AFCCTRL, register_content);

    return;
}

///
/// @brief Check if AFC low beta is enabled.
///
/// @param None
/// @return bool True if enabled, otherwise false.
///
bool libRFM69_IsAFCLowBetaEnabled(void)
{
    return IsBitSetInRegister(REG_AFCCTRL, 5);
}

///
/// @brief Enable Continuous-Time DAGC,
///
/// In addition to the automatic gain control, the RFM69HW is capable of continuously
/// adjusting its gain in the digital domain, after the analog to digital conversion
/// has occurred.
///
/// @param enabled True or False
/// @return None
///
void libRFM69_EnableContinuousDAGC(bool enabled)
{
    uint8_t register_content;

    if (enabled)
    {
        register_content = libRFM69_IsAFCLowBetaEnabled() == true ?
                           RF_DAGC_IMPROVED_LOWBETA1 : RF_DAGC_IMPROVED_LOWBETA0;
    }
    else
    {
        register_content = RF_DAGC_NORMAL;
    }

    libRFM69_WriteRegister(REG_TESTDAGC, register_content);
    return;
}

void libRFM69_WriteRegister(uint8_t address, uint8_t register_data)
{
    sc_assert(address <= REG_TESTAFC);

    libSPI_WriteByte(address | WRITE_REG, &PreCallback, NULL);
    libSPI_WriteByte(register_data, NULL, &PostCallback);

    return;
}

void libRFM69_ReadRegister(uint8_t address, uint8_t *register_data)
{
    sc_assert(address <= REG_TESTAFC);

    libSPI_WriteByte(address & READ_REG, &PreCallback, NULL);
    libSPI_ReadByte(register_data, NULL, &PostCallback);

    return;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static void PreCallback(void)
{
    libSPI_SetMode(SPIMODE);

    PullCS(); //Select device
    return;
}

static void PostCallback(void)
{
    ReleaseCS(); //Release device
    return;
}

///
/// @brief Check if a bit is set in a register. If something fails this
//         function will return true!
///
/// @param register Register to check in
/// @param bit Bit to check
/// @return true if bit is set
/// @return false if bit is not set
///
static bool IsBitSetInRegister(uint8_t address, uint8_t bit)
{
    sc_assert(bit <= 7);
    sc_assert(address <= REG_TESTPA2);

    uint8_t register_content;

    libRFM69_ReadRegister(address, &register_content);
    return ((register_content & (1 << bit)) > 0);
}