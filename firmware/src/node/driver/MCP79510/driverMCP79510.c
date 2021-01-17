/**
 * @file   driverMCP79510.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2021-01-17 (Last edit)
 * @brief  Driver for the MCP79510 RTC.
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

#include <stdlib.h>

#include "common.h"
#include "libDebug.h"
#include "Bit.h"
#include "driverMCP79510.h"
#include "driverMCP79510Registers.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define SRAM_SIZE 64
#define EUI_MAX_SIZE 8

#define GetDecimalRegisterValue(address) BCDToDecimal(ReadRegister(address))
#define SetDecimalRegisterValue(address, value) WriteRegister(address, DecimalToBCD(value))

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

struct module_t
{
    libSPI_callback_type PreSPI;
    libSPI_callback_type PostSPI;
};

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static struct module_t module;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

static uint8_t ReadRegister(uint8_t address);
static void WriteRegister(uint8_t address, uint8_t register_data);

#ifdef DEBUG_ENABLE
static void DumpRegisterValues(void) __attribute__((unused));
#endif

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void driverMCP79510_Init(libSPI_callback_type pre_fp,
                         libSPI_callback_type post_fp
                        )
{
    sc_assert(pre_fp != NULL);
    sc_assert(post_fp != NULL);

    module.PreSPI = pre_fp;
    module.PostSPI = post_fp;

    driverMCP79510_EnableSquareWave(false);
    driverMCP79510_EnableOscillator(true);
    driverMCP79510_SetOscillatorTrimming(0);

    while(!driverMCP79510_IsOscillatorRunning())
    {
        /**
         * Wait for the oscillator to start.
         */
    }

    INFO("MCP79510 initialized");
}

void driverMCP79510_GetHundredthSecond(uint8_t *hsec_p)
{
    *hsec_p = GetDecimalRegisterValue(REG_TC_SEC_CENT);
}

bool driverMCP79510_SetHundredthSecond(uint8_t hsec)
{
    bool status = false;

    if (hsec < 100)
    {
        SetDecimalRegisterValue(REG_TC_SEC_CENT, hsec);
        status = true;
    }

    return status;
}

bool driverMCP79510_SetAlarmHundredthSecond(uint8_t hsec)
{
    bool status = false;

    if (hsec < 100)
    {
        SetDecimalRegisterValue(REG_ALARM1_SEC_CENT, hsec);
        status = true;
    }

    return status;
}

void driverMCP79510_GetSecond(uint8_t *sec_p)
{
    uint8_t register_content;
    register_content = ReadRegister(REG_TC_SEC);
    register_content &= ~(1 << REG_TC_SEC_OSC_BIT);

    *sec_p = BCDToDecimal(register_content);
}

bool driverMCP79510_SetSecond(uint8_t sec)
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

bool driverMCP79510_SetAlarmSeconds(uint8_t sec, uint8_t alarm_index)
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

void driverMCP79510_GetMinute(uint8_t *minute_p)
{
    *minute_p = GetDecimalRegisterValue(REG_TC_MIN);
}

bool driverMCP79510_SetMinute(uint8_t minute)
{
    bool status = false;

    if (minute < 60)
    {
        SetDecimalRegisterValue(REG_TC_MIN, minute);
        status = true;
    }

    return status;
}

bool driverMCP79510_SetAlarmMinute(uint8_t minute, uint8_t alarm_index)
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

void driverMCP79510_GetHour(uint8_t *hour_p)
{
    uint8_t register_content;
    register_content = ReadRegister(REG_TC_HOUR);

    //Check if 24-hour mode is used.
    if (!Bit_Get(REG_TC_HOUR_MODE_BIT, &register_content))
    {
        register_content &= 0x3F;
    }
    else
    {
        register_content &= 0x1F;
    }

    *hour_p = BCDToDecimal(register_content);
}

bool driverMCP79510_SetHour(uint8_t hour)
{
    const bool is_24_hour_mode = driverMCP79510_Is24HourMode();
    const uint8_t min_hour = is_24_hour_mode ? 0 : 1;
    const uint8_t max_hour = is_24_hour_mode ? 23 : 12;
    bool status = false;

    if (hour >= min_hour && hour <= max_hour)
    {
        const uint8_t clear_mask = is_24_hour_mode ? 0xC0 : 0xE0;
        uint8_t register_content = ReadRegister(REG_TC_HOUR);

        register_content = (register_content & clear_mask) | DecimalToBCD(hour);
        WriteRegister(REG_TC_HOUR, register_content);
        status = true;
    }

    return status;
}

bool driverMCP79510_SetAlarmHour(uint8_t hour, uint8_t alarm_index)
{
    sc_assert(alarm_index < 2);

    const bool is_24_hour_mode = driverMCP79510_Is24HourMode();
    const uint8_t min_hour = is_24_hour_mode ? 0 : 1;
    const uint8_t max_hour = is_24_hour_mode ? 23 : 12;
    bool status = false;

    if (hour >= min_hour && hour <= max_hour)
    {
        const uint8_t register_address = (alarm_index == 0) ? REG_ALARM0_HOUR : REG_ALARM1_HOUR;
        const uint8_t clear_mask = is_24_hour_mode ? 0xC0 : 0xE0;
        uint8_t register_content = ReadRegister(register_address);

        register_content = (register_content & clear_mask) | DecimalToBCD(hour);
        WriteRegister(register_address, register_content);
        status = true;
    }

    return status;
}

void driverMCP79510_GetDay(uint8_t *day_p)
{
    uint8_t register_content;
    register_content = ReadRegister(REG_TC_DAY);
    register_content &= 0x07;

    *day_p = BCDToDecimal(register_content);
}

bool driverMCP79510_SetDay(uint8_t day)
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

void driverMCP79510_GetDate(uint8_t *date_p)
{
    *date_p = GetDecimalRegisterValue(REG_TC_DATE);
}

bool driverMCP79510_SetDate(uint8_t date)
{
    bool status = false;

    if (date > 0 && date < 32)
    {
        SetDecimalRegisterValue(REG_TC_DATE, date);
        status = true;
    }

    return status;
}

bool driverMCP79510_SetAlarmDate(uint8_t date, uint8_t alarm_index)
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

void driverMCP79510_GetMonth(uint8_t *month_p)
{
    uint8_t register_content;
    register_content = ReadRegister(REG_TC_MONTH);
    register_content &= 0x1F;

    *month_p = BCDToDecimal(register_content);
}

bool driverMCP79510_SetMonth(uint8_t month)
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

void driverMCP79510_GetYear(uint8_t *year_p)
{
    *year_p = GetDecimalRegisterValue(REG_TC_YEAR);
}

bool driverMCP79510_SetYear(uint8_t year)
{
    bool status = false;

    if (year < 100)
    {
        SetDecimalRegisterValue(REG_TC_YEAR, year);
        status = true;
    }

    return status;
}

void driverMCP79510_Enable24HourMode(bool enabled)
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
}

bool driverMCP79510_Is24HourMode(void)
{
    uint8_t register_content;

    register_content = ReadRegister(REG_TC_HOUR);
    return !Bit_Get(REG_TC_HOUR_MODE_BIT, &register_content);
}

void driverMCP79510_EnableOscillator(bool enabled)
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
}

bool driverMCP79510_IsLeapYear(void)
{
    uint8_t register_content;

    register_content = ReadRegister(REG_TC_MONTH);
    return Bit_Get(REG_TC_MONTH_LPYR_BIT, &register_content);
}

void driverMCP79510_EnableAlarm(bool enable, uint8_t alarm_index)
{
    sc_assert(alarm_index < 2);

    uint8_t register_data = ReadRegister(REG_TC_CONTROL);
    Bit_Set(REG_TC_CONTROL_ALM0EN_BIT + alarm_index, enable, &register_data);

    WriteRegister(REG_TC_CONTROL, register_data);
}

void driverMCP79510_EnableSquareWave(bool enable)
{
    uint8_t register_data = ReadRegister(REG_TC_CONTROL);

    Bit_Set(REG_TC_CONTROL_SQWEN_BIT, enable, &register_data);
    WriteRegister(REG_TC_CONTROL, register_data);

}

bool driverMCP79510_IsOscillatorRunning(void)
{
    uint8_t register_content;

    register_content = ReadRegister(REG_TC_DAY);
    return Bit_Get(REG_TC_DAY_OSCRUN_BIT, &register_content);
}

void driverMCP79510_SetOscillatorTrimming(int16_t trim_value)
{
    const uint8_t reg_trim_value = abs(trim_value) > UINT8_MAX ? UINT8_MAX : (uint8_t)abs(trim_value);
    WriteRegister(REG_TC_OSCTRIM, reg_trim_value);

    uint8_t register_content;
    register_content = ReadRegister(REG_TC_HOUR);

    const bool is_unsigned = trim_value >= 0;
    Bit_Set(REG_TC_HOUR_TRIMSIGN_BIT, is_unsigned, &register_content);
    WriteRegister(REG_TC_HOUR, register_content);
}

void driverMCP79510_EnableExternalBattery(bool enable)
{
    uint8_t register_data = ReadRegister(REG_TC_DAY);

    Bit_Set(REG_TC_DAY_VBATEN_BIT, enable, &register_data);
    WriteRegister(REG_TC_DAY, register_data);
}

bool driverMCP79510_IsExternalBatteryEnabled(void)
{
    uint8_t register_content;

    register_content = ReadRegister(REG_TC_DAY);
    return Bit_Get(REG_TC_DAY_VBATEN_BIT, &register_content);
}

void driverMCP79510_ClearBatterySwitchFlag(void)
{
    uint8_t register_content;

    register_content = ReadRegister(REG_TC_DAY);
    Bit_Set(REG_TC_DAY_PWRFAIL_BIT, false, &register_content);

    WriteRegister(REG_TC_DAY, register_content);
}

bool driverMCP79510_GetBatterySwitchFlag(void)
{
    uint8_t register_content;

    register_content = ReadRegister(REG_TC_DAY);
    return Bit_Get(REG_TC_DAY_PWRFAIL_BIT, &register_content);
}

void driverMCP79510_ClearAlarmFlag(uint8_t alarm_index)
{
    sc_assert(alarm_index < 2);

    uint8_t register_address;
    register_address = (alarm_index == 0) ? REG_ALARM0_DAY :
                       REG_ALARM1_DAY;

    uint8_t register_content;
    register_content = ReadRegister(register_address);

    Bit_Set(REG_ALARM_DAY_ALM0IF_BIT, false, &register_content);
    WriteRegister(register_address, register_content);
}

bool driverMCP79510_WriteToSRAM(uint8_t address, const void *data_p,
                                uint8_t length)
{
    bool status = false;

    if ((uint16_t)address + (uint16_t)length <= SRAM_SIZE)
    {
        libSPI_WriteByte(INST_WRITE, module.PreSPI, NULL);
        libSPI_WriteByte(SRAM_ADDRESS + address, NULL, NULL);

        for (uint8_t i = 0; i < length; ++i)
        {
            const uint8_t *tmp_p = (const uint8_t *)data_p;
            libSPI_WriteByte(tmp_p[i], NULL, NULL);
        }
        module.PostSPI();
        status = true;
    }
    return status;
}

bool driverMCP79510_ReadFromSRAM(uint8_t address, void *data_p, uint8_t length)
{
    bool status = false;

    if ((uint16_t)address + (uint16_t)length <= SRAM_SIZE)
    {
        libSPI_WriteByte(INST_READ, module.PreSPI, NULL);
        libSPI_WriteByte(SRAM_ADDRESS + address, NULL, NULL);

        for (uint8_t i = 0; i < length; ++i)
        {
            uint8_t *tmp_p = (uint8_t *)data_p;

            //SRAM address is auto incremented after each read
            libSPI_ReadByte(&tmp_p[i], NULL, NULL);
        }
        module.PostSPI();
        status = true;
    }
    return status;
}

void driverMCP79510_ClearSRAM(void)
{
    libSPI_WriteByte(INST_CLRRAM, module.PreSPI, NULL);
    libSPI_WriteByte(0x00, NULL, module.PostSPI); //Dummy byte
}

void driverMCP79510_GetEUI(uint8_t *eui, size_t length)
{
    sc_assert(length <= EUI_MAX_SIZE);

    libSPI_WriteByte(INST_IDREAD, module.PreSPI, NULL);
    libSPI_WriteByte(EUI_MAX_SIZE - length, NULL, NULL);

    for (uint8_t i = 0; i < length; ++i)
    {
        libSPI_ReadByte(&eui[i], NULL, NULL);
    }

    module.PostSPI();
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

#ifdef DEBUG_ENABLE
static void DumpRegisterValues(void)
{
    DEBUG("****MCP79510 REGISTERS****\r\n");
    for (uint8_t address = REG_TC_SEC_CENT; address <= REG_PU_MONTH; ++address)
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
}
#endif

static void WriteRegister(uint8_t address, uint8_t register_data)
{
    sc_assert(address < 0x20);

    libSPI_WriteByte(INST_WRITE, module.PreSPI, NULL);
    libSPI_WriteByte(address, NULL, NULL);
    libSPI_WriteByte(register_data, NULL, module.PostSPI);
}

static uint8_t ReadRegister(uint8_t address)
{
    sc_assert(address < 0x20);

    uint8_t register_data;

    libSPI_WriteByte(INST_READ, module.PreSPI, NULL);
    libSPI_WriteByte(address, NULL, NULL);
    libSPI_ReadByte(&register_data, NULL, module.PostSPI);
    return register_data;
}
