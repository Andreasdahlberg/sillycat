/**
 * @file   RTC.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2016-04-03 (Last edit)
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

//One hour, expressed in seconds
#define ONE_HOUR_S 3600

//One day, expressed in seconds
#define ONE_DAY_S 86400

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
void RTC_FormatTimestamp(const rtc_time_type *time, char *timestamp)
{
    sc_assert(time != NULL);
    sc_assert(timestamp != NULL);

    sprintf(timestamp, TIMESTAMP_FORMAT, time->year, time->month, time->date,
            time->hour, time->minute, time->second);

    return;
}

#ifdef RTC_HAL
///
/// @brief Get the number of seconds since midnight, January 1 2000, UTC.
///
/// @param  *timestamp Pointer to timestamp where the result will be stored
/// @return bool true, if current time was successfully read, otherwise false.
///
bool RTC_GetTimeStamp(uint32_t *timestamp)
{
    rtc_time_type time;

    if (RTC_GetCurrentTime(&time) == true)
    {
        *timestamp = RTC_ConvertToTimestamp(&time);
        return true;
    }
    return false;
}

///
/// @brief Get a time struct with fields for different time periods.
///
/// @param  *time Pointer to struct where the result will be stored
/// @return bool true, if current time was successfully read, otherwise false.
///
bool RTC_GetCurrentTime(rtc_time_type *time)
{
    return (RTC_GetYear(&time->year) &&
            RTC_GetMonth(&time->month) &&
            RTC_GetDate(&time->date) &&
            RTC_GetHour(&time->hour) &&
            RTC_GetMinutes(&time->minute) &&
            RTC_GetSeconds(&time->second));
}

///
/// @brief Set the current time.
///
/// @param  *time Pointer to struct with time to set.
/// @return bool true, if new time was set successfully.
///
bool RTC_SetCurrentTime(const rtc_time_type *time)
{
    return (RTC_SetYear(time->year) &&
            RTC_SetMonth(time->month) &&
            RTC_SetDate(time->date) &&
            RTC_SetHour(time->hour) &&
            RTC_SetMinutes(time->minute) &&
            RTC_SetSeconds(time->second));
}

///
/// @brief Set an alarm time.
///
/// @param  *time Pointer to struct with alarm time to set.
/// @return bool true, if new alarm time was set successfully.
///
bool RTC_SetAlarmTime(const rtc_time_type *time)
{
    return (RTC_SetAlarmHour(time->hour) &&
            RTC_SetAlarmMinutes(time->minute) &&
            RTC_SetAlarmSeconds(time->second));
}
#endif

///
/// @brief Check if daylight time savings is active.
///
/// IMPORTENT: Only valid for DST in Sweden!
///
/// @param  *time Pointer to struct with current time.
/// @param  week_day Index of current week day, starting at Monday=1.
/// @return bool true if DST is active, otherwise false.
///
bool RTC_IsDaylightSavingActive(const rtc_time_type *time, uint8_t week_day)
{
    sc_assert(time != NULL);
    sc_assert(week_day <= 7);

    bool dst_active;
    uint8_t next_sunday;

    if (time->month > MARCH && time->month < OCTOBER)
    {
        dst_active = true;
    }
    else if (time->month < MARCH || time->month > OCTOBER)
    {
        dst_active = false;
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

        //TODO: Fix edge cases!
        dst_active = ((time->month == MARCH && time->date > next_sunday) ||
                      (time->month == OCTOBER && time->date < next_sunday));
    }
    return dst_active;
}

///
/// @brief Check if a year is a leap year.
///
/// @param  year Year to check.
/// @return bool true if leap year, otherwise false.
///
bool RTC_IsLeapYear(uint16_t year)
{
    bool is_leap_year = false;

    //All leap years are divisible by 4
    if (year % 4 == 0)
    {
        if (year % 100 != 0 || year % 400 == 0)
        {
            is_leap_year = true;
        }
    }

    return is_leap_year;
}

///
/// @brief Add seconds to time.
///
/// @param  *time Pointer to struct with time.
/// @param  seconds Number of seconds to add.
/// @return None
///
void RTC_AddSeconds(rtc_time_type *time, uint8_t seconds)
{
    sc_assert(time != NULL);
    uint16_t carry;

    carry = (time->second + seconds) / 60;
    time->second = (time->second + seconds) % 60;

    if (carry > 0)
    {
        RTC_AddMinutes(time, carry);
    }
    return;
}

///
/// @brief Add minutes to time.
///
/// @param  *time Pointer to struct with time.
/// @param  minutes Number of minutes to add.
/// @return None
///
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

///
/// @brief Add hours to time.
///
/// @param  *time Pointer to struct with time.
/// @param  hours Number of hours to add.
/// @return None
///
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

///
/// @brief Add days to time.
///
/// @param  *time Pointer to struct with time.
/// @param  days Number of days to add.
/// @return None
///
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

///
/// @brief Add months to time.
///
/// @param  *time Pointer to struct with time.
/// @param  months Number of months to add.
/// @return None
///
void RTC_AddMonths(rtc_time_type *time, uint8_t months)
{
    sc_assert(time != NULL);
    uint16_t carry;

    carry =  (time->month + months) / 12;
    time->month = (time->month + months) % 12;

    if (carry > 0)
    {
        RTC_AddYears(time, carry);
    }
    return;
}

///
/// @brief Add years to time.
///
/// @param  *time Pointer to struct with time.
/// @param  years Number of years to add.
/// @return None
///
void RTC_AddYears(rtc_time_type *time, uint8_t years)
{
    sc_assert(time != NULL);
    time->year += years;

    sc_assert(time->year < 100);
    return;
}

///
/// @brief Convert an broken down time struct into number of seconds since
///     midnight, January 1 2000, UTC
///
/// @author Michael Duane Rice, Original
/// @author Andreas Dahlberg, Adaptations
///
/// @param  *time Pointer to time struct
/// @return uint32_t Timestamp in seconds
///
uint32_t RTC_ConvertToTimestamp(const rtc_time_type *time)
{
    sc_assert(time != NULL);

    uint32_t tmp;
    int n, d, leaps;

    //Determine elapsed whole days since the epoch to the beginning of this year. Since our epoch is
    //at a conjunction of the leap cycles, we can do this rather quickly.
    n = time->year;
    leaps = 0;
    if (n)
    {
        uint32_t m;
        m = n - 1;
        leaps = m / 4;
        leaps -= m / 100;
        leaps++;
    }
    tmp = 365UL * n + leaps;

    //Derive the day of year from month and day of month. We use the pattern of 31 day months
    //followed by 30 day months to our advantage, but we must 'special case' Jan/Feb, and
    //account for a 'phase change' between July and August (153 days after March 1).

    d = time->date - 1; //Date is one based

    //Handle Jan/Feb as a special case
    if (time->month < 2)
    {
        if (time->month)
        {
            d += 31;
        }
    }
    else
    {
        uint32_t m;
        n = 59 + RTC_IsLeapYear((uint16_t)time->year + 2000);
        d += n;
        n = time->month - MARCH;

        //Account for phase change
        if (n > (JULY - MARCH))
        {
            d += 153;
        }
        n %= 5;

        //n is now an index into a group of alternating 31 and 30
        //day months... 61 day pairs.
        m = n / 2;
        m *= 61;
        d += m;

        //if n is odd, we are in the second half of the month pair
        if (n & 1)
        {
            d += 31;
        }
    }

    uint32_t return_value;

    //Add day of year to elapsed days, and convert to seconds
    tmp += d;
    tmp *= ONE_DAY_S;
    return_value = tmp;

    //Compute 'fractional' day
    tmp = time->hour;
    tmp *= ONE_HOUR_S;
    tmp += time->minute * 60UL;
    tmp += time->second;

    return return_value += tmp;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////