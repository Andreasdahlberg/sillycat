/**
 * @file   mock_driverMCP79510.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2020-10-21 (Last edit)
 * @brief  Mock functions for the MCP79510 driver.
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
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include <string.h>
#include "mock_driverMCP79510.h"

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

void __wrap_driverMCP79510_Init(libSPI_callback_type pre_fp, libSPI_callback_type post_fp)
{
}

void __wrap_driverMCP79510_GetHundredthSecond(uint8_t *hsec_p)
{
}

bool __wrap_driverMCP79510_SetHundredthSecond(uint8_t hsec)
{
    mock_type(bool);
}

bool __wrap_driverMCP79510_SetAlarmHundredthSecond(uint8_t hsec)
{
    mock_type(bool);
}

void __wrap_driverMCP79510_GetSecond(uint8_t *sec_p)
{
}

bool __wrap_driverMCP79510_SetSecond(uint8_t sec)
{
    mock_type(bool);
}

bool __wrap_driverMCP79510_SetAlarmSeconds(uint8_t sec, uint8_t alarm_index)
{
    mock_type(bool);
}

void __wrap_driverMCP79510_GetMinute(uint8_t *minute_p)
{
}

bool __wrap_driverMCP79510_SetMinute(uint8_t minute)
{
    mock_type(bool);
}

bool __wrap_driverMCP79510_SetAlarmMinute(uint8_t minute, uint8_t alarm_index)
{
    mock_type(bool);
}

void __wrap_driverMCP79510_GetHour(uint8_t *hour_p)
{
}

bool __wrap_driverMCP79510_SetHour(uint8_t hour)
{
    mock_type(bool);
}

bool __wrap_driverMCP79510_SetAlarmHour(uint8_t hour, uint8_t alarm_index)
{
    mock_type(bool);
}

void __wrap_driverMCP79510_GetDay(uint8_t *day_p)
{
}

bool __wrap_driverMCP79510_SetDay(uint8_t day)
{
    mock_type(bool);
}

void __wrap_driverMCP79510_GetDate(uint8_t *date_p)
{
}

bool __wrap_driverMCP79510_SetDate(uint8_t date)
{
    mock_type(bool);
}

bool __wrap_driverMCP79510_SetAlarmDate(uint8_t date, uint8_t alarm_index)
{
    mock_type(bool);
}

void __wrap_driverMCP79510_GetMonth(uint8_t *month_p)
{
}

bool __wrap_driverMCP79510_SetMonth(uint8_t month)
{
    mock_type(bool);
}

void __wrap_driverMCP79510_GetYear(uint8_t *year_p)
{
}

bool __wrap_driverMCP79510_SetYear(uint8_t year)
{
    mock_type(bool);
}

void __wrap_driverMCP79510_Enable24HourMode(bool enabled)
{
}

bool __wrap_driverMCP79510_Is24HourMode(void)
{
    mock_type(bool);
}

void __wrap_driverMCP79510_EnableOscillator(bool enabled)
{
}

bool __wrap_driverMCP79510_IsLeapYear(void)
{
    mock_type(bool);
}

void __wrap_driverMCP79510_EnableAlarm(bool enable, uint8_t alarm)
{
}

void __wrap_driverMCP79510_EnableSquareWave(bool enable)
{
}

bool __wrap_driverMCP79510_IsOscillatorRunning(void)
{
    mock_type(bool);
}

void __wrap_driverMCP79510_ClearBatterySwitchFlag(void)
{
}

void __wrap_driverMCP79510_ClearAlarmFlag(uint8_t alarm_index)
{
}

bool __wrap_driverMCP79510_WriteToSRAM(uint8_t address, const void *data_p, uint8_t length)
{
    function_called();
}

bool __wrap_driverMCP79510_ReadFromSRAM(uint8_t address, void *data_p, uint8_t length)
{
    memset(data_p, 0, length);
}

void __wrap_driverMCP79510_ClearSRAM(void)
{
}

void __wrap_driverMCP79510_GetEUI(uint8_t *eui_p, size_t length)
{
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

