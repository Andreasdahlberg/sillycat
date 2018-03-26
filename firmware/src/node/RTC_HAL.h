/**
 * @file   RTC_HAL.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-03-26 (Last edit)
 * @brief  Header of RTC HAL
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

#ifndef RTCHAL_H_
#define RTCHAL_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include "libMCP79510.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define RTC_ALARM_INDEX 1
#define RTC_IsAlarmsSupported() true

#define RTC_InitHW()    libMCP79510_HWInit()
#define RTC_Init()      libMCP79510_Init()

#define RTC_GetYear(year)           libMCP79510_GetYear(year)
#define RTC_GetMonth(month)         libMCP79510_GetMonth(month)
#define RTC_GetDate(date)           libMCP79510_GetDate(date)
#define RTC_GetHour(hour)           libMCP79510_GetHour(hour)
#define RTC_GetMinutes(minutes)     libMCP79510_GetMinute(minutes)
#define RTC_GetSeconds(seconds)     libMCP79510_GetSecond(seconds)
#define RTC_GetDay(day)             libMCP79510_GetDay(day)
#define RTC_GetHourMode(hour_mode)  //TODO: Fix this!

#define RTC_SetMinutes(minutes) libMCP79510_SetMinute(minutes)
#define RTC_SetSeconds(seconds) libMCP79510_SetSecond(seconds)
#define RTC_SetHour(hour)       libMCP79510_SetHour(hour)
#define RTC_SetYear(year)       libMCP79510_SetYear(year)
#define RTC_SetMonth(month)     libMCP79510_SetMonth(month)
#define RTC_SetDate(date)       libMCP79510_SetDate(date)
#define RTC_SetDay(day)         libMCP79510_SetDay(day)

#define RTC_SetAlarmDate(date)          libMCP79510_SetAlarmDate(date, RTC_ALARM_INDEX)
#define RTC_SetAlarmHour(hour)          libMCP79510_SetAlarmHour(hour, RTC_ALARM_INDEX)
#define RTC_SetAlarmMinutes(minutes)    libMCP79510_SetAlarmMinute(minutes, RTC_ALARM_INDEX)
#define RTC_SetAlarmSeconds(seconds)    libMCP79510_SetAlarmSeconds(seconds, RTC_ALARM_INDEX)

#define RTC_ClearAlarm()        libMCP79510_ClearAlarmFlag(RTC_ALARM_INDEX)
#define RTC_EnableAlarm(enable) libMCP79510_EnableAlarm(enable, RTC_ALARM_INDEX)

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

#endif
