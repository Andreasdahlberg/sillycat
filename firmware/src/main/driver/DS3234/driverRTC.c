/**
 * @file   driverRTC.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2021-02-13 (Last edit)
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
#include "driverDS3234.h"

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
    driverDS3234_Init(Board_RTC_SPIPreCallback, Board_RTC_SPIPostCallback);
}

void driverRTC_GetTime(struct driverRTC_time_t *time_p)
{
    driverDS3234_GetTime(time_p);
}

void driverRTC_GetYear(uint8_t *year_p)
{
    driverDS3234_GetYear(year_p);
}

bool driverRTC_SetYear(uint8_t year)
{
    return driverDS3234_SetYear(year);
}

void driverRTC_GetMonth(uint8_t *month_p)
{
    driverDS3234_GetMonth(month_p);
}

bool driverRTC_SetMonth(uint8_t month)
{
    return driverDS3234_SetMonth(month);
}

void driverRTC_GetDate(uint8_t *date_p)
{
    driverDS3234_GetDate(date_p);
}

bool driverRTC_SetDate(uint8_t date)
{
    return driverDS3234_SetDate(date);
}

void driverRTC_GetDay(uint8_t *day_p)
{
    driverDS3234_GetDay(day_p);
}

bool driverRTC_SetDay(uint8_t day)
{
    return driverDS3234_SetDay(day);
}

void driverRTC_GetHour(uint8_t *hour_p)
{
    driverDS3234_GetHour(hour_p);
}

bool driverRTC_SetHour(uint8_t hour)
{
    return driverDS3234_SetHour(hour);
}

void driverRTC_GetMinute(uint8_t *minute_p)
{
    driverDS3234_GetMinute(minute_p);
}

bool driverRTC_SetMinute(uint8_t minute)
{
    return driverDS3234_SetMinute(minute);
}

void driverRTC_GetSecond(uint8_t *second_p)
{
    driverDS3234_GetSecond(second_p);
}

bool driverRTC_SetSecond(uint8_t second)
{
    return driverDS3234_SetSecond(second);
}

bool driverRTC_Is24HourMode(void)
{
    return driverDS3234_Is24HourMode();
}

void driverRTC_EnableAlarm(bool enable)
{
    driverDS3234_EnableAlarm(enable, ALARM_INDEX);
}

void driverRTC_ClearAlarmFlag(void)
{
    driverDS3234_ClearAlarmFlag(ALARM_INDEX);
}

bool driverRTC_SetAlarmDate(uint8_t date)
{
    return driverDS3234_SetAlarmDate(date, ALARM_INDEX);
}

bool driverRTC_SetAlarmHour(uint8_t hour)
{
    return driverDS3234_SetAlarmHour(hour, ALARM_INDEX);
}

bool driverRTC_SetAlarmMinute(uint8_t minute)
{
    return driverDS3234_SetAlarmMinute(minute, ALARM_INDEX);
}

bool driverRTC_SetAlarmSecond(uint8_t second)
{
    return driverDS3234_SetAlarmSecond(second);
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////
