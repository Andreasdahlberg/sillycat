/**
 * @file   RTC.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2015-12-12 (Last edit)
 * @brief  Implementation of RTC interface
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

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>

#include "common.h"
#include "libDebug.h"
#include "RTC.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define TIMESTAMP_FORMAT "20%02u-%02u-%02u %02u:%02u:%02u"

#define DAYS_IN_MARCH_OCTOBER 31
#define DAYS_IN_WEEK 7

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static const uint8_t days_in_months[12] = {DAYS_IN_JAN,
                                           DAYS_IN_FEB,
                                           DAYS_IN_MAR,
                                           DAYS_IN_APR,
                                           DAYS_IN_MAY,
                                           DAYS_IN_JUN,
                                           DAYS_IN_JUL,
                                           DAYS_IN_AUG,
                                           DAYS_IN_SEP,
                                           DAYS_IN_OCT,
                                           DAYS_IN_NOV,
                                           DAYS_IN_DEC
                                          };
//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

///
/// @brief Format time to an string timestamp
///
/// @param  *time Pointer to time
/// @param  *timestamp Pointer to char array where timestamp will be stored
/// @return None
///
void RTC_FormatTimestamp(rtc_time_type *time, char *timestamp)
{
    sc_assert(time != NULL);
    sc_assert(timestamp != NULL);

    sprintf(timestamp, TIMESTAMP_FORMAT, time->year, time->month, time->date,
            time->hour, time->minute, time->second);

    return;
}

#ifdef RTC_HAL
bool RTC_GetCurrentTime(rtc_time_type *time)
{
    return (RTC_GetYear(&time->year) &&
            RTC_GetMonth(&time->month) &&
            RTC_GetDate(&time->date) &&
            RTC_GetHour(&time->hour) &&
            RTC_GetMinutes(&time->minute) &&
            RTC_GetSeconds(&time->second));
}

bool RTC_SetCurrentTime(rtc_time_type *time)
{
    return (RTC_SetYear(time->year) &&
            RTC_SetMonth(time->month) &&
            RTC_SetDate(time->date) &&
            RTC_SetHour(time->hour) &&
            RTC_SetMinutes(time->minute) &&
            RTC_SetSeconds(time->second));
}
#endif

//TODO: Fix edge cases!
bool RTC_IsDaylightSavingActive(rtc_time_type *time, uint8_t week_day)
{
    sc_assert(time != NULL);
    sc_assert(week_day <= 7);

    bool dst_active;
    uint8_t next_sunday;

    if (time->month > MARCH && time->month < OCTOBER)
    {
        dst_active = TRUE;
    }
    else if (time->month < MARCH || time->month > OCTOBER)
    {
        dst_active = FALSE;
    }
    else
    {
        next_sunday = time->date + (DAYS_IN_WEEK - week_day);

        if (next_sunday < DAYS_IN_MARCH_OCTOBER)
        {
            while (next_sunday + DAYS_IN_WEEK < DAYS_IN_MARCH_OCTOBER)
            {
                next_sunday += DAYS_IN_WEEK;
            }
        }
        else
        {
            next_sunday -= DAYS_IN_WEEK;
        }

        dst_active = ((time->month == MARCH && time->date > next_sunday) ||
                      (time->month == OCTOBER && time->date < next_sunday));
    }
    return dst_active;
}

bool RTC_IsLeapYear(uint16_t year)
{
    bool is_leap_year = FALSE;

    //All leap years are divisible by 4
    if (year % 4 == 0)
    {
        if (year % 100 != 0 || year % 400 == 0)
        {
            is_leap_year = TRUE;
        }
    }

    return is_leap_year;
}

void RTC_AddMinutes(rtc_time_type *time, uint8_t minutes)
{
    sc_assert(time != NULL);
    uint16_t carry;

    carry = (time->minute + minutes) / 60;
    time->minute = (time->minute + minutes) % 60;

    if (carry > 0)
    {
        RTC_AddHours(time, carry);
    }
    return;
}

void RTC_AddHours(rtc_time_type *time, uint8_t hours)
{
    sc_assert(time != NULL);
    uint16_t carry;

    carry =  (time->hour + hours) / 24;
    time->hour = (time->hour + hours) % 24;

    if (carry > 0)
    {
        RTC_AddDays(time, carry);
    }
    return;
}

void RTC_AddDays(rtc_time_type *time, uint8_t days)
{
    sc_assert(time != NULL);
    uint16_t carry;
    uint8_t days_in_month;

    days_in_month = days_in_months[time->month - 1];
    if (time->month == FEBRUARY && RTC_IsLeapYear(2000 + (uint16_t)time->year))
    {
        ++days_in_month;
    }

    carry = (time->date + days) / days_in_month;
    time->date = (time->date + days) % days_in_month;

    //TODO: Fix cases when carry > 1
    sc_assert(carry < 2);

    if (carry > 0)
    {
        RTC_AddMonths(time, carry);
    }
    return;
}

void RTC_AddMonths(rtc_time_type *time, uint8_t months)
{
    sc_assert(time != NULL);
    uint16_t carry;

    carry =  (time->month + months) / 12;
    time->month = (time->month + months) % 12;

    if (carry > 0)
    {
        RTC_AddYear(time, carry);
    }
    return;
}

void RTC_AddYear(rtc_time_type *time, uint8_t year)
{
    sc_assert(time != NULL);
    time->year += year;

    sc_assert(time->year < 100);
    return;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////