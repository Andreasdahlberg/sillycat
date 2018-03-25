/**
 * @file   libMCP79510.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-03-25 (Last edit)
 * @brief  Implementation of a driver for the MCP79510 RTC.
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

#define MFP DD3
#define SS  DDD4
#define SPIMODE 0

#define SRAM_SIZE 64
#define EUI_MAX_SIZE 8

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

#ifdef DEBUG_ENABLE
static void DumpRegisterValues(void) __attribute__((unused));
#endif

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void libMCP79510_Init(void)
{
    libMCP79510_EnableSquareWave(false);
    libMCP79510_EnableOscillator(true);

    while(!libMCP79510_IsOscillatorRunning())
    {
    }

    INFO("MCP79510 initialized");
    return;
}

void libMCP79510_HWInit(void)
{
    //Set SS as output and pull high to release device.
    DDRD |= (1 << SS);
    PORTD |= (1 << SS);

    //Set MFP as input with an pull-up active.
    DDRD &= ~(1 << MFP);
    PORTD |= (1 << MFP);
    return;
}

void libMCP79510_GetHundredthSecond(uint8_t *hsec_p)
{
    *hsec_p = GetDecimalRegisterValue(REG_TC_SEC_CENT);
    return;
}

bool libMCP79510_SetHundredthSecond(uint8_t hsec)
{
    bool status = false;

    if (hsec < 100)
    {
        SetDecimalRegisterValue(REG_TC_SEC_CENT, hsec);
        status = true;
    }

    return status;
}

bool libMCP79510_SetAlarmHundredthSecond(uint8_t hsec)
{
    bool status = false;

    if (hsec < 100)
    {
        SetDecimalRegisterValue(REG_ALARM1_SEC_CENT, hsec);
        status = true;
    }

    return status;
}

void libMCP79510_GetSecond(uint8_t *sec_p)
{
    uint8_t register_content;
    register_content = ReadRegister(REG_TC_SEC);
    register_content &= ~(1 << REG_TC_SEC_OSC_BIT);

    *sec_p = BCDToDecimal(register_content);
    return;
}

bool libMCP79510_SetSecond(uint8_t sec)
{
    bool status = false;

    if (sec < 60)
    {
        uint8_t register_content;
        register_content = ReadRegister(REG_TC_SEC);

        WriteRegister(REG_TC_SEC, DecimalToBCD(sec) | (register_content & 0x80));

        status = true;
    }

    return status;
}

bool libMCP79510_SetAlarmSeconds(uint8_t sec, uint8_t alarm_index)
{
    sc_assert(alarm_index < 2);

    bool status = false;

    if (sec < 60)
    {
        uint8_t register_address;

        register_address = (alarm_index == 0) ? REG_ALARM0_SEC :
                           REG_ALARM1_SEC;
        SetDecimalRegisterValue(register_address, sec);

        status = true;
    }

    return status;
}

void libMCP79510_GetMinute(uint8_t *minute_p)
{
    *minute_p = GetDecimalRegisterValue(REG_TC_MIN);
    return;
}

bool libMCP79510_SetMinute(uint8_t minute)
{
    bool status = false;

    if (minute < 60)
    {
        SetDecimalRegisterValue(REG_TC_MIN, minute);
        status = true;
    }

    return status;
}

bool libMCP79510_SetAlarmMinute(uint8_t minute, uint8_t alarm_index)
{
    sc_assert(alarm_index < 2);

    bool status = false;

    if (minute < 60)
    {
        uint8_t register_address;
        register_address = (alarm_index == 0) ? REG_ALARM0_MIN :
                           REG_ALARM1_MIN;

        SetDecimalRegisterValue(register_address, minute);
        status = true;
    }

    return status;
}

void libMCP79510_GetHour(uint8_t *hour_p)
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

    *hour_p = BCDToDecimal(register_content);
    return;
}

bool libMCP79510_SetHour(uint8_t hour)
{
    bool status = false;

    //TODO: check after hour mode is known?
    if (hour < 24)
    {
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
        status = true;
    }

    return status;
}

bool libMCP79510_SetAlarmHour(uint8_t hour, uint8_t alarm_index)
{
    sc_assert(alarm_index < 2);

    bool status = false;

    //TODO: check after hour mode is known?
    if (hour < 24)
    {
        uint8_t register_address = (alarm_index == 0) ? REG_ALARM0_HOUR :
                                   REG_ALARM1_HOUR;
        uint8_t register_content = ReadRegister(register_address);
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

        WriteRegister(register_address, new_content);
        status = true;
    }
    return status;
}

void libMCP79510_GetDay(uint8_t *day_p)
{
    uint8_t register_content;
    register_content = ReadRegister(REG_TC_DAY);
    register_content &= 0x07;

    *day_p = BCDToDecimal(register_content);
    return;
}

bool libMCP79510_SetDay(uint8_t day)
{
    bool status = false;

    if (day > 0 && day < 8)
    {
        uint8_t register_content;
        register_content = ReadRegister(REG_TC_DAY);

        WriteRegister(REG_TC_DAY, DecimalToBCD(day) | (register_content & 0xF8));
        status = true;
    }

    return status;
}

void libMCP79510_GetDate(uint8_t *date_p)
{
    *date_p = GetDecimalRegisterValue(REG_TC_DATE);
    return;
}

bool libMCP79510_SetDate(uint8_t date)
{
    bool status = false;

    if (date > 0 && date < 32)
    {
        SetDecimalRegisterValue(REG_TC_DATE, date);
        status = true;
    }

    return status;
}

bool libMCP79510_SetAlarmDate(uint8_t date, uint8_t alarm_index)
{
    sc_assert(alarm_index < 2);

    bool status = false;

    if (date > 0 && date < 32)
    {
        uint8_t register_address;
        register_address = (alarm_index == 0) ? REG_ALARM0_DATE :
                           REG_ALARM1_DATE;

        SetDecimalRegisterValue(register_address, date);
        status = true;
    }

    return status;
}

void libMCP79510_GetMonth(uint8_t *month_p)
{
    uint8_t register_content;
    register_content = ReadRegister(REG_TC_MONTH);
    register_content &= 0x1F;

    *month_p = BCDToDecimal(register_content);
    return;
}

bool libMCP79510_SetMonth(uint8_t month)
{
    bool status = false;

    if (month > 0 && month < 13)
    {
        uint8_t register_content;
        register_content = ReadRegister(REG_TC_MONTH);

        WriteRegister(REG_TC_MONTH, DecimalToBCD(month) | (register_content & 0xE0));
        status = true;
    }

    return status;
}

void libMCP79510_GetYear(uint8_t *year_p)
{
    *year_p = GetDecimalRegisterValue(REG_TC_YEAR);
    return;
}

bool libMCP79510_SetYear(uint8_t year)
{
    bool status = false;

    if (year < 100)
    {
        SetDecimalRegisterValue(REG_TC_YEAR, year);
        status = true;
    }

    return status;
}

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

bool libMCP79510_Is24HourMode(void)
{
    uint8_t register_content;

    register_content = ReadRegister(REG_TC_HOUR);
    return !IsBitSet(REG_TC_HOUR_MODE_BIT, &register_content);
}

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

bool libMCP79510_IsLeapYear(void)
{
    uint8_t register_content;

    register_content = ReadRegister(REG_TC_MONTH);
    return IsBitSet(REG_TC_MONTH_LP_BIT, &register_content);
}

void libMCP79510_EnableAlarm(bool enable, uint8_t alarm_index)
{
    sc_assert(alarm_index < 2);

    uint8_t register_data = ReadRegister(REG_TC_CONTROL);

    if (enable == true)
    {
        SetBitD(register_data, (REG_TC_CONTROL_ALM0_BIT + alarm_index));
    }
    else
    {
        ClearBit(register_data, (REG_TC_CONTROL_ALM0_BIT + alarm_index));
    }

    WriteRegister(REG_TC_CONTROL, register_data);
    return;
}

void libMCP79510_EnableSquareWave(bool enable)
{
    uint8_t register_data = ReadRegister(REG_TC_CONTROL);

    SetBit(REG_TC_CONTROL_SQWE_BIT, enable, &register_data);
    WriteRegister(REG_TC_CONTROL, register_data);

    return;
}

bool libMCP79510_IsOscillatorRunning(void)
{
    uint8_t register_content;

    register_content = ReadRegister(REG_TC_DAY);
    return IsBitSet(REG_TC_DAY_OSCON_BIT, &register_content);
}

void libMCP79510_ClearBatterySwitchFlag(void)
{
    uint8_t register_content;

    register_content = ReadRegister(REG_TC_DAY);
    ClearBit(register_content, REG_TC_DAY_VBAT_BIT);

    WriteRegister(REG_TC_DAY, register_content);
    return;
}

void libMCP79510_ClearAlarmFlag(uint8_t alarm_index)
{
    sc_assert(alarm_index < 2);

    uint8_t register_address;
    register_address = (alarm_index == 0) ? REG_ALARM0_DAY :
                       REG_ALARM1_DAY;

    uint8_t register_content;
    register_content = ReadRegister(register_address);

    ClearBit(register_content, REG_ALARM_DAY_ALM0IF_BIT);
    WriteRegister(register_address, register_content);
    return;
}

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

void libMCP79510_ClearSRAM(void)
{
    libSPI_WriteByte(INST_CLRRAM, &PreCallback, NULL);
    libSPI_WriteByte(0x00, NULL, &PostCallback); //Dummy byte
    return;
}

void libMCP79510_GetEUI(uint8_t *eui, size_t length)
{
    sc_assert(length <= EUI_MAX_SIZE);

    libSPI_WriteByte(INST_IDREAD, &PreCallback, NULL);
    libSPI_WriteByte(EUI_MAX_SIZE - length, NULL, NULL);

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

#ifdef DEBUG_ENABLE
static void DumpRegisterValues(void)
{
    uint8_t address;

    DEBUG("****MCP79510 REGISTERS****\r\n");
    for (address = REG_TC_SEC_CENT; address <= REG_PU_MONTH; ++address)
    {
        //Skip reserved registers
        if (address != 0x0A && address != 0x0B)
        {
            uint8_t register_content;

            register_content = ReadRegister(address);
            DEBUG("REG ADDR: 0x%02X	REG VALUE: 0x%02X\r\n", address,
                  register_content);
        }
    }
    return;
}
#endif

static void WriteRegister(uint8_t address, uint8_t register_data)
{
    sc_assert(address < 0x20);

    libSPI_WriteByte(INST_WRITE, &PreCallback, NULL);
    libSPI_WriteByte(address, NULL, NULL);
    libSPI_WriteByte(register_data, NULL, &PostCallback);
    return;
}

static uint8_t ReadRegister(uint8_t address)
{
    sc_assert(address < 0x20);

    uint8_t register_data;

    libSPI_WriteByte(INST_READ, &PreCallback, NULL);
    libSPI_WriteByte(address, NULL, NULL);
    libSPI_ReadByte(&register_data, NULL, &PostCallback);
    return register_data;
}

static void PreCallback(void)
{
    libSPI_SetMode(SPIMODE);
    PORTD &= ~(1 << SS); //Pull SS low to select device
    return;
}

static void PostCallback(void)
{
    PORTD |= (1 << SS); //Pull SS high to release device
    return;
}
