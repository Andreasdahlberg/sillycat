/**
 * @file   RTC_HAL.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2015-12-6 (Last edit)
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

#include "libDS3234.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define RTC_ALARM_INDEX 1

#define RTC_InitHW()    libDS3234_Init()
#define RTC_Init()      libDS3234_HWInit()

#define RTC_GetYear(year)           libDS3234_GetYear(year)
#define RTC_GetMonth(month)         libDS3234_GetMonth(month)
#define RTC_GetDate(date)           libDS3234_GetDate(date)
#define RTC_GetHour(hour)           libDS3234_GetHour(hour)
#define RTC_GetMinutes(minutes)     libDS3234_GetMinutes(minutes)
#define RTC_GetSeconds(seconds)     libDS3234_GetSeconds(seconds)
#define RTC_GetDay(day)             libDS3234_GetDay(day)
#define RTC_GetHourMode(hour_mode)  libDS3234_GetHourMode(hour_mode)

#define RTC_SetMinutes(minutes) libDS3234_SetMinutes(minutes)
#define RTC_SetSeconds(seconds) libDS3234_SetSeconds(seconds)
#define RTC_SetHour(hour)       libDS3234_SetHour(hour)
#define RTC_SetYear(year)       libDS3234_SetYear(year)
#define RTC_SetMonth(month)     libDS3234_SetMonth(month)
#define RTC_SetDate(date)       libDS3234_SetDate(date)
#define RTC_SetDay(day)         libDS3234_SetDay(day)

#define RTC_SetAlarmDate(date)          libDS3234_SetAlarmDate(date, RTC_ALARM_INDEX)
#define RTC_SetAlarmHour(hour)          libDS3234_SetAlarmHour(hour, RTC_ALARM_INDEX)
#define RTC_SetAlarmMinutes(minutes)    libDS3234_SetAlarmMinutes(minutes, RTC_ALARM_INDEX)
#define RTC_SetAlarmSeconds(seconds)    libDS3234_SetAlarmSeconds(seconds)

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

#endif /* RTCHAL_H_ */