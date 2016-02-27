/**
 * @file   libMCP79510.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2016-02-27 (Last edit)
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

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//TODO: Verify SS pin
#define SS  DDC0
#define SPIMODE 1

//#define SRAM_SIZE 256

#define GetDecimalRegisterValue(address) BCDToDecimal(ReadRegister(address))

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
/// @brief Get year.
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