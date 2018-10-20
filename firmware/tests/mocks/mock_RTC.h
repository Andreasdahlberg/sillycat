/**
 * @file   mock_RTC.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-10-19 (Last edit)
 * @brief  Mock functions for the RTC module.
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

#ifndef WRAP_RTC_H_
#define WRAP_RTC_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include <stdbool.h>
#include "RTC.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

void __wrap_RTC_AddSeconds(rtc_time_type *time, uint8_t seconds);
void __wrap_RTC_AddMonths(rtc_time_type *time, uint8_t months);
bool __wrap_RTC_GetCurrentTime(rtc_time_type *time);
bool __wrap_RTC_SetAlarmTime(const rtc_time_type *time);
bool __wrap_RTC_IsDaylightSavingActive(const rtc_time_type *time, uint8_t week_day);
bool __wrap_RTC_IsLeapYear(uint16_t year);
uint8_t __wrap_RTC_CalculateDayOfWeek(rtc_time_type *time);
bool __wrap_RTC_GetTimeStamp(uint32_t *timestamp);
uint8_t __wrap_RTC_GetDaysInMonth(const rtc_time_type *time);
void __wrap_RTC_AddMinutes(rtc_time_type *time, uint8_t minutes);
bool __wrap_RTC_SetCurrentTime(const rtc_time_type *time);
void __wrap_RTC_AddHours(rtc_time_type *time, uint8_t hours);
void __wrap_RTC_FormatTimestamp(const rtc_time_type *time, char *timestamp, size_t size);
uint32_t __wrap_RTC_ConvertToTimestamp(const rtc_time_type *timeptr);
void __wrap_RTC_AddYears(rtc_time_type *time, uint8_t year);
void __wrap_RTC_AddDays(rtc_time_type *time, uint8_t days);

#endif