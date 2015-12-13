/**
 * @file   RTC.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2015-12-12 (Last edit)
 * @brief  Header of RTC interface
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

#ifndef RTC_H_
#define RTC_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include "RTC_HAL.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

typedef struct
{
    uint8_t year;
    uint8_t month;
    uint8_t date;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} rtc_time_type;

typedef enum
{
    JANUARY = 1,
    FEBRUARY,
    MARCH,
    APRIL,
    MAY,
    JUNE,
    JULY,
    AUGUST,
    SEPTEMBER,
    OCTOBER,
    NOVEMBER,
    DECEMBER
} rtc_months_type;

#define DAYS_IN_JAN 31
#define DAYS_IN_FEB 28
#define DAYS_IN_MAR 31
#define DAYS_IN_APR 30
#define DAYS_IN_MAY 31
#define DAYS_IN_JUN 30
#define DAYS_IN_JUL 31
#define DAYS_IN_AUG 31
#define DAYS_IN_SEP 30
#define DAYS_IN_OCT 31
#define DAYS_IN_NOV 30
#define DAYS_IN_DEC 31
//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

void RTC_GetTimestamp(char *timestamp);
bool RTC_GetCurrentTime(rtc_time_type *time);
bool RTC_SetCurrentTime(rtc_time_type *time);
bool RTC_IsDaylightSavingActive(rtc_time_type *time, uint8_t week_day);
bool RTC_IsLeapYear(uint16_t year);
void RTC_AddMinutes(rtc_time_type *time, uint8_t minutes);
void RTC_AddHours(rtc_time_type *time, uint8_t hours);
void RTC_AddDays(rtc_time_type *time, uint8_t days);
void RTC_AddMonths(rtc_time_type *time, uint8_t months);
void RTC_AddYear(rtc_time_type *time, uint8_t year);

#endif /* RTC_H_ */