/**
 * @file   driverRTC.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2021-02-15 (Last edit)
 * @brief  RTC driver interface.
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

#include "common.h"
#include "Board.h"
#include "driverMCP79510.h"
#include "driverRTC.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define ALARM_INDEX 0

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

void driverRTC_Init(void)
{
    driverMCP79510_Init(Board_RTC_SPIPreCallback, Board_RTC_SPIPostCallback);
}

void driverRTC_GetTime(struct driverRTC_time_t *time_p)
{
    driverMCP79510_GetTime(time_p);
}

bool driverRTC_SetTime(const struct driverRTC_time_t *time_p)
{
    return driverMCP79510_SetTime(time_p);
}

void driverRTC_GetYear(uint8_t *year_p)
{
    driverMCP79510_GetYear(year_p);
}

bool driverRTC_SetYear(uint8_t year)
{
    return driverMCP79510_SetYear(year);
}

void driverRTC_GetMonth(uint8_t *month_p)
{
    driverMCP79510_GetMonth(month_p);
}

bool driverRTC_SetMonth(uint8_t month)
{
    return driverMCP79510_SetMonth(month);
}

void driverRTC_GetDate(uint8_t *date_p)
{
    driverMCP79510_GetDate(date_p);
}

bool driverRTC_SetDate(uint8_t date)
{
    return driverMCP79510_SetDate(date);
}

void driverRTC_GetDay(uint8_t *day_p)
{
    driverMCP79510_GetDay(day_p);
}

bool driverRTC_SetDay(uint8_t day)
{
    return driverMCP79510_SetDay(day);
}

void driverRTC_GetHour(uint8_t *hour_p)
{
    driverMCP79510_GetHour(hour_p);
}

bool driverRTC_SetHour(uint8_t hour)
{
    return driverMCP79510_SetHour(hour);
}

void driverRTC_GetMinute(uint8_t *minute_p)
{
    driverMCP79510_GetMinute(minute_p);
}

bool driverRTC_SetMinute(uint8_t minute)
{
    return driverMCP79510_SetMinute(minute);
}

void driverRTC_GetSecond(uint8_t *second_p)
{
    driverMCP79510_GetSecond(second_p);
}

bool driverRTC_SetSecond(uint8_t second)
{
    return driverMCP79510_SetSecond(second);
}

bool driverRTC_Is24HourMode(void)
{
    return driverMCP79510_Is24HourMode();
}

void driverRTC_EnableAlarm(bool enable)
{
    driverMCP79510_EnableAlarm(enable, ALARM_INDEX);
}

void driverRTC_ClearAlarmFlag(void)
{
    driverMCP79510_ClearAlarmFlag(ALARM_INDEX);
}

bool driverRTC_SetAlarmDate(uint8_t date)
{
    return driverMCP79510_SetAlarmDate(date, ALARM_INDEX);
}

bool driverRTC_SetAlarmHour(uint8_t hour)
{
    return driverMCP79510_SetAlarmHour(hour, ALARM_INDEX);
}

bool driverRTC_SetAlarmMinute(uint8_t minute)
{
    return driverMCP79510_SetAlarmMinute(minute, ALARM_INDEX);
}

bool driverRTC_SetAlarmSecond(uint8_t second)
{
    return driverMCP79510_SetAlarmSeconds(second, ALARM_INDEX);
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////
