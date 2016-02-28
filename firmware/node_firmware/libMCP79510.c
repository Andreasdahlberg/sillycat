/**
 * @file   libMCP79510.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2016-02-28 (Last edit)
 * @brief  Implementation of MCP79510-library.
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

#include <avr/io.h>

#include "libDebug.h"
#include "libSPI.h"
#include "libMCP79510.h"
#include "MCP79510Registers.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//TODO: Verify SS pin
#define SS  DDC0
#define SPIMODE 1

#define SRAM_SIZE 64

#define GetDecimalRegisterValue(address) BCDToDecimal(ReadRegister(address))
#define SetDecimalRegisterValue(address, value) WriteRegister(address, DecimalToBCD(value))

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

static uint8_t ReadRegister(uint8_t address);
static void WriteRegister(uint8_t address, uint8_t register_data);
static void PreCallback(void);
static void PostCallback(void);

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

///
/// @brief Init the MCP79510 RTC
///
/// @param  None
/// @return None
///
void libMCP79510_Init(void)
{
    libMCP79510_HWInit();
    libMCP79510_EnableOscillator(true);

    INFO("Init done");
    return;
}

///
/// @brief Set the SS pin as output and pull high. This function should
///        be called as early as possible in a system with several SPI-devices.
///
/// @param  None
/// @return None
///
void libMCP79510_HWInit(void)
{
    //Set SS as output
    DDRC |= (1 << SS);
    //Pull SS high to release device
    PORTC |= (1 << SS);
    return;
}

///
/// @brief Get hundredth of second.
///
/// @param  *hsec Pointer to variable where the hundredth of second will be stored.
/// @return None
///
void libMCP79510_GetHundredthSecond(uint8_t *hsec)
{
    *hsec = GetDecimalRegisterValue(REG_TC_SEC_CENT);
    return;
}

///
/// @brief Set hundredth of second.
///
/// @param  hsec Hundredth of second to set.
/// @return None
///
void libMCP79510_SetHundredthSecond(uint8_t hsec)
{
    sc_assert(hsec < 100);

    SetDecimalRegisterValue(REG_TC_SEC_CENT, hsec);
    return;
}

///
/// @brief Get second.
///
/// @param  *sec Pointer to variable where the second will be stored.
/// @return None
///
void libMCP79510_GetSecond(uint8_t *sec)
{
    uint8_t register_content;
    register_content = ReadRegister(REG_TC_SEC);
    register_content &= ~(1 << REG_TC_SEC_OSC_BIT);

    *sec = BCDToDecimal(register_content);
    return;
}

///
/// @brief Set second.
///
/// @param  sec Second to set.
/// @return None
///
void libMCP79510_SetSecond(uint8_t sec)
{
    sc_assert(sec < 60);

    uint8_t register_content;
    register_content = ReadRegister(REG_TC_SEC);

    WriteRegister(REG_TC_SEC, DecimalToBCD(sec) | (register_content & 0x80));
    return;
}

///
/// @brief Get minute.
///
/// @param  *minute Pointer to variable where the minute will be stored.
/// @return None
///
void libMCP79510_GetMinute(uint8_t *minute)
{
    *minute = GetDecimalRegisterValue(REG_TC_MIN);
    return;
}

///
/// @brief Set minute.
///
/// @param  minute Minute to set.
/// @return None
///
void libMCP79510_SetMinute(uint8_t minute)
{
    sc_assert(minute < 60);

    SetDecimalRegisterValue(REG_TC_MIN, minute);
    return;
}

///
/// @brief Get hour.
///
/// @param  *hour Pointer to variable where the hour will be stored.
/// @return None
///
void libMCP79510_GetHour(uint8_t *hour)
{
    uint8_t register_content;
    register_content = ReadRegister(REG_TC_HOUR);

    //Check if 24-hour mode is used.
    if (!IsBitSet(REG_TC_HOUR_MODE_BIT, &register_content))
    {
        register_content &= 0x3F;
    }
    else
    {
        register_content &= 0x1F;
    }

    *hour = BCDToDecimal(register_content);
    return;
}

///
/// @brief Set hour.
///
/// @param  hour Hour to set.
/// @return None
///
void libMCP79510_SetHour(uint8_t hour)
{
    //TODO: check after hour mode is known?
    sc_assert(hour < 24);

    uint8_t register_content = ReadRegister(REG_TC_HOUR);
    uint8_t new_content;

    //Check if 24-hour mode is used.
    if (!IsBitSet(REG_TC_HOUR_MODE_BIT, &register_content))
    {
        new_content = (register_content & 0xC0) | DecimalToBCD(hour);
    }
    else
    {
        new_content = (register_content & 0xE0) | DecimalToBCD(hour);
    }

    WriteRegister(REG_TC_HOUR, new_content);
    return;
}

///
/// @brief Get day.
///
/// @param  *day Pointer to variable where the day will be stored.
/// @return None
///
void libMCP79510_GetDay(uint8_t *day)
{
    uint8_t register_content;
    register_content = ReadRegister(REG_TC_DAY);
    register_content &= 0x07;

    *day = BCDToDecimal(register_content);
    return;
}

///
/// @brief Set day.
///
/// @param  day Day to set.
/// @return None
///
void libMCP79510_SetDay(uint8_t day)
{
    sc_assert(day < 8);

    uint8_t register_content;
    register_content = ReadRegister(REG_TC_DAY);

    WriteRegister(REG_TC_DAY, DecimalToBCD(day) | (register_content & 0xF8));
    return;
}

///
/// @brief Get date.
///
/// @param  *date Pointer to variable where the date will be stored.
/// @return None
///
void libMCP79510_GetDate(uint8_t *date)
{
    *date = GetDecimalRegisterValue(REG_TC_DATE);
    return;
}

///
/// @brief Set date.
///
/// @param  date Date to set.
/// @return None
///
void libMCP79510_SetDate(uint8_t date)
{
    sc_assert(date < 32);

    SetDecimalRegisterValue(REG_TC_DATE, date);
    return;
}

///
/// @brief Get month.
///
/// @param  *month Pointer to variable where the month will be stored.
/// @return None
///
void libMCP79510_GetMonth(uint8_t *month)
{
    uint8_t register_content;
    register_content = ReadRegister(REG_TC_MONTH);
    register_content &= 0x1F;

    *month = BCDToDecimal(register_content);
    return;
}

///
/// @brief Set month.
///
/// @param  month Month to set.
/// @return None
///
void libMCP79510_SetMonth(uint8_t month)
{
    sc_assert(month < 13);

    uint8_t register_content;
    register_content = ReadRegister(REG_TC_MONTH);

    WriteRegister(REG_TC_MONTH, DecimalToBCD(month) | (register_content & 0xE0));
    return;
}

///
/// @brief Get year(number of years since 2000).
///
/// @param  *year Pointer to variable where the year will be stored.
/// @return None
///
void libMCP79510_GetYear(uint8_t *year)
{
    *year = GetDecimalRegisterValue(REG_TC_YEAR);
    return;
}

///
/// @brief Set year.
///
/// @param  year Year to set, number of years since 2000.
/// @return None
///
void libMCP79510_SetYear(uint8_t year)
{
    sc_assert(year < 100);

    SetDecimalRegisterValue(REG_TC_YEAR, year);
    return;
}

///
/// @brief Enabled 24-hour mode.
///
/// @param  enabled Set to true for enabling 24-hour mode.
/// @return None
///
void libMCP79510_Enable24HourMode(bool enabled)
{
    uint8_t register_content;
    register_content = ReadRegister(REG_TC_HOUR);

    if (enabled == true)
    {
        register_content &= ~(1 << REG_TC_HOUR_MODE_BIT);
    }
    else
    {
        register_content |= (1 << REG_TC_HOUR_MODE_BIT);
    }

    WriteRegister(REG_TC_HOUR, register_content);
    return;
}

///
/// @brief Check if 24-hour mode is active.
///
/// @param  None
/// @return false  If 24-mode is inactive and 12-hour mode is used.
/// @return true If 24-mode is active
///
bool libMCP79510_Is24HourMode(void)
{
    uint8_t register_content;

    register_content = ReadRegister(REG_TC_HOUR);
    return !IsBitSet(REG_TC_HOUR_MODE_BIT, &register_content);
}

///
/// @brief Enable/disable the MCP79510 on-board oscillator.
///
/// @param  enabled Set to true for enabling the oscillator.
/// @return None
///
void libMCP79510_EnableOscillator(bool enabled)
{
    uint8_t register_content;
    register_content = ReadRegister(REG_TC_SEC);

    if (enabled == true)
    {
        register_content |= (1 << REG_TC_SEC_OSC_BIT);
    }
    else
    {
        register_content &= ~(1 << REG_TC_SEC_OSC_BIT);
    }

    WriteRegister(REG_TC_SEC, register_content);
    return;
}

///
/// @brief Check if the active year is a leap year.
///
/// @param  None
/// @return bool True if leap year, otherwise false.
///
bool libMCP79510_IsLeapYear(void)
{
    uint8_t register_content;

    register_content = ReadRegister(REG_TC_MONTH);
    return IsBitSet(REG_TC_MONTH_LP_BIT, &register_content);
}

///
/// @brief Enable an alarm
///
/// @param  Index of alarm to enable, 1 or 2
/// @return None
///
void libMCP79510_EnableAlarm(bool enable, uint8_t alarm)
{
    sc_assert(alarm < 3);

    uint8_t register_data = ReadRegister(REG_TC_CONTROL);

    if (enable == true)
    {
        register_data |= alarm << 4;
    }
    else
    {
        register_data &= ~(alarm << 4);
    }

    WriteRegister(REG_TC_CONTROL, register_data);
    return;
}

///
/// @brief Enable the square wave output.
///
/// Enables the divided output from the crystal oscillator.
///
/// @param  enabled Set to true for enabling the output.
/// @return None
///
void libMCP79510_EnableSquareWave(bool enable)
{
    uint8_t register_data = ReadRegister(REG_TC_CONTROL);

    SetBit(REG_TC_CONTROL_SQWE_BIT, enable, &register_data);
    WriteRegister(REG_TC_CONTROL, register_data);

    return;
}

///
/// @brief Check if oscillator is running.
///
/// This flag is set and cleared by hardware. If this flag is set, the oscillator is
/// running; if clear, the oscillator is not running. This flag does not indicate that
/// the oscillator is running at the correct frequency. The flag will wait 32 oscillator
/// cycles before the bit is set.
///
/// @param  None
/// @return bool True if oscillator is running, otherwise false.
///
bool libMCP79510_IsOscillatorRunning(void)
{
    uint8_t register_content;

    register_content = ReadRegister(REG_TC_DAY);
    return IsBitSet(REG_TC_DAY_OSCON_BIT, &register_content);
}

///
/// @brief Clear the external battery switched flag.
///
/// This flag is set by hardware when the VCC fails and the VBAT is used to power
/// the oscillator and the RTCC registers. This flag is cleared by software.
///
/// @param  None
/// @return None
///
void libMCP79510_ClearBatterySwitchFlag(void)
{
    uint8_t register_content;

    register_content = ReadRegister(REG_TC_DAY);
    ClearBit(register_content, REG_TC_DAY_VBAT_BIT);

    WriteRegister(REG_TC_DAY, register_content);
    return;
}

///
/// @brief  Write data to SRAM.
///
/// Write data to SRAM. The SRAM is powered by the RTC battery so data is
/// persistent between power cycles.
///
/// @param  address Address in flash to write. Virtual address starting at zero.
/// @param  *data Pointer to data to write.
/// @param  length Number of bytes to write.
/// @return bool True if address and length is valid, otherwise false.
///
bool libMCP79510_WriteToSRAM(uint8_t address, const uint8_t *data,
                             uint8_t length)
{
    bool status = false;

    if ((uint16_t)address + (uint16_t)length <= SRAM_SIZE)
    {
        libSPI_WriteByte(INST_WRITE, &PreCallback, NULL);
        libSPI_WriteByte(SRAM_ADDRESS + address, NULL, NULL);

        uint8_t index;
        for (index = 0; index < length; ++index)
        {
            //TODO: Verify that address is auto incremented for write.
            libSPI_WriteByte(data[index], NULL, NULL);
        }
        PostCallback();
        status = true;
    }
    return status;
}

///
/// @brief  Read data from SRAM.
///
/// Read data from SRAM. The SRAM is powered by the RTC battery so data is
/// persistent between power cycles.
///
/// @param  address Address in flash to read. Virtual address starting at zero.
/// @param  *data Pointer to buffer where data will be stored.
/// @param  length Number of bytes to read.
/// @return bool True if address and length is valid, otherwise false.
///
bool libMCP79510_ReadFromSRAM(uint8_t address, uint8_t *data, uint8_t length)
{
    bool status = false;

    if ((uint16_t)address + (uint16_t)length <= SRAM_SIZE)
    {
        libSPI_WriteByte(INST_READ, &PreCallback, NULL);
        libSPI_WriteByte(SRAM_ADDRESS + address, NULL, NULL);

        uint8_t index;
        for (index = 0; index < length; ++index)
        {
            //SRAM address is auto incremented after each read
            libSPI_ReadByte(&data[index], NULL, NULL);
        }
        PostCallback();
        status = true;
    }
    return status;
}

///
/// @brief Clear SRAM.
///
/// Clear SRAM by reseting all locations to 0x00.
///
/// @param  None
/// @return None
///
void libMCP79510_ClearSRAM(void)
{
    libSPI_WriteByte(INST_CLRRAM, &PreCallback, NULL);
    libSPI_WriteByte(0x00, NULL, &PostCallback); //Dummy byte
    return;
}

///
/// @brief Get unique ID.
///
/// Get the unique EUI-48/68 id. ID could be preprogrammed from the factory
/// or defined by the user application.
///
/// @param  *eui Pointer to buffer where the ID will be stored.
/// @param  length Length of ID.
/// @return None
///
void libMCP79510_GetEUI(uint8_t *eui, size_t length)
{
    sc_assert(length < 7);

    uint8_t address_offset = 6 - length;
    libSPI_WriteByte(INST_IDREAD, &PreCallback, NULL);
    libSPI_WriteByte(address_offset, NULL, NULL);

    uint8_t idx;
    for (idx = 0; idx < length; ++idx)
    {
        libSPI_ReadByte(&eui[idx], NULL, NULL);
    }

    PostCallback();
    return;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

///
/// @brief Write value to register.
///
/// @param  address Address of register.
/// @param  register_data Value to write.
/// @return None
///
static void WriteRegister(uint8_t address, uint8_t register_data)
{
    sc_assert(address < 0x20);

    libSPI_WriteByte(INST_WRITE, &PreCallback, NULL);
    libSPI_WriteByte(address, NULL, NULL);
    libSPI_WriteByte(register_data, NULL, &PostCallback);
    return;
}

///
/// @brief Read value from register.
///
/// @param  address Address of register.
/// @return uint8_t Register value
///
static uint8_t ReadRegister(uint8_t address)
{
    sc_assert(address < 0x20);

    uint8_t register_data;

    libSPI_WriteByte(INST_WRITE, &PreCallback, NULL);
    libSPI_WriteByte(address, NULL, NULL);
    libSPI_ReadByte(&register_data, NULL, &PostCallback);
    return register_data;
}

///
/// @brief Set correct SPI-mode and select device.
///
/// @param  None
/// @return None
///
static void PreCallback(void)
{
    libSPI_SetMode(SPIMODE);
    PORTC &= ~(1 << SS); //Pull SS low to select device
    return;
}

///
/// @brief Deselect device.
///
/// Pulls SS high so the device is deselected. The SPI-mode is left as it is.
///
/// @param  None
/// @return None
///
static void PostCallback(void)
{
    PORTC |= (1 << SS); //Pull SS high to release device
    return;
}