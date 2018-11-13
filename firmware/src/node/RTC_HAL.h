/**
 * @file   RTC_HAL.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-11-13 (Last edit)
 * @brief  RTC HAL.
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

#ifndef RTCHAL_H_
#define RTCHAL_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include "driverMCP79510.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define RTC_ALARM_INDEX 1
#define RTC_IsAlarmsSupported() true

#define RTC_InitHW()
#define RTC_Init()      driverMCP79510_Init(Board_RTC_SPIPreCallback, Board_RTC_SPIPostCallback)

#define RTC_GetYear(year)           driverMCP79510_GetYear(year)
#define RTC_GetMonth(month)         driverMCP79510_GetMonth(month)
#define RTC_GetDate(date)           driverMCP79510_GetDate(date)
#define RTC_GetHour(hour)           driverMCP79510_GetHour(hour)
#define RTC_GetMinutes(minutes)     driverMCP79510_GetMinute(minutes)
#define RTC_GetSeconds(seconds)     driverMCP79510_GetSecond(seconds)
#define RTC_GetDay(day)             driverMCP79510_GetDay(day)
#define RTC_GetHourMode(hour_mode)  //TODO: Fix this!

#define RTC_SetMinutes(minutes) driverMCP79510_SetMinute(minutes)
#define RTC_SetSeconds(seconds) driverMCP79510_SetSecond(seconds)
#define RTC_SetHour(hour)       driverMCP79510_SetHour(hour)
#define RTC_SetYear(year)       driverMCP79510_SetYear(year)
#define RTC_SetMonth(month)     driverMCP79510_SetMonth(month)
#define RTC_SetDate(date)       driverMCP79510_SetDate(date)
#define RTC_SetDay(day)         driverMCP79510_SetDay(day)

#define RTC_SetAlarmDate(date)          driverMCP79510_SetAlarmDate(date, RTC_ALARM_INDEX)
#define RTC_SetAlarmHour(hour)          driverMCP79510_SetAlarmHour(hour, RTC_ALARM_INDEX)
#define RTC_SetAlarmMinutes(minutes)    driverMCP79510_SetAlarmMinute(minutes, RTC_ALARM_INDEX)
#define RTC_SetAlarmSeconds(seconds)    driverMCP79510_SetAlarmSeconds(seconds, RTC_ALARM_INDEX)

#define RTC_ClearAlarm()        driverMCP79510_ClearAlarmFlag(RTC_ALARM_INDEX)
#define RTC_EnableAlarm(enable) driverMCP79510_EnableAlarm(enable, RTC_ALARM_INDEX)

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

#endif
