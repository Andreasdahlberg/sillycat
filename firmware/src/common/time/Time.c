/**
 * @file   Time.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2019-06-13 (Last edit)
 * @brief  Module with functions for manipulating time.
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
#include "common.h"
#include "Time.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define TIMESTAMP_FORMAT "20%02u-%02u-%02u %02u:%02u:%02u"

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
#define DAYS_IN_MARCH_OCTOBER 31
#define DAYS_IN_WEEK 7

/* One hour, expressed in seconds. */
#define ONE_HOUR_S 3600

/* One day, expressed in seconds. */
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

void Time_FormatTimestamp(const struct time_t *time_p,
                          char *timestamp_p,
                          size_t size
                         )
{
    sc_assert(time_p != NULL);
    sc_assert(timestamp_p != NULL);
    sc_assert(size > 0);

    snprintf(timestamp_p,
             size,
             TIMESTAMP_FORMAT,
             time_p->year,
             time_p->month,
             time_p->date,
             time_p->hour,
             time_p->minute,
             time_p->second
            );
}

bool Time_IsDaylightSavingActive(const struct time_t *time_p)
{
    sc_assert(time_p != NULL);

    bool dst_active;

    if (time_p->month > MARCH && time_p->month < OCTOBER)
    {
        dst_active = true;
    }
    else if (time_p->month < MARCH || time_p->month > OCTOBER)
    {
        dst_active = false;
    }
    else
    {
        uint8_t next_sunday;
        next_sunday = time_p->date + (DAYS_IN_WEEK - Time_CalculateDayOfWeek(time_p));

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
        dst_active = ((time_p->month == MARCH && time_p->date >= next_sunday) ||
                      (time_p->month == OCTOBER && time_p->date < next_sunday));
    }
    return dst_active;
}

bool Time_IsLeapYear(const struct time_t *time_p)
{
    sc_assert(time_p != NULL);
    uint16_t year = time_p->year;
    bool is_leap_year = false;

    year += 2000;

    //All leap years are divisible by 4
    if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))
    {
        is_leap_year = true;
    }
    return is_leap_year;
}

void Time_AddSeconds(struct time_t *time_p, uint8_t seconds)
{
    sc_assert(time_p != NULL);

    uint32_t timestamp = Time_ConvertToTimestamp(time_p);
    timestamp += seconds;
    *time_p = Time_ConvertFromTimestamp(timestamp);
}

void Time_AddMinutes(struct time_t *time_p, uint8_t minutes)
{
    sc_assert(time_p != NULL);

    uint32_t timestamp = Time_ConvertToTimestamp(time_p);
    timestamp += minutes * 60;
    *time_p = Time_ConvertFromTimestamp(timestamp);
}

void Time_AddHours(struct time_t *time_p, uint8_t hours)
{
    sc_assert(time_p != NULL);

    uint32_t timestamp = Time_ConvertToTimestamp(time_p);
    timestamp += hours * 60 * 60;
    *time_p = Time_ConvertFromTimestamp(timestamp);
}

void Time_AddDays(struct time_t *time_p, uint8_t days)
{
    sc_assert(time_p != NULL);

    uint32_t timestamp = Time_ConvertToTimestamp(time_p);
    timestamp += days * 24 * 3600;
    *time_p = Time_ConvertFromTimestamp(timestamp);
}

struct time_t Time_ConvertFromTimestamp(uint32_t timestamp)
{
    struct time_t time;
    struct div_t result;
    uint16_t days;
    uint16_t n;
    uint16_t leapyear;
    uint16_t years;

    /* break down timer into whole days */
    days = timestamp / 86400UL;

    /* Extract hour, minute, and second from the fractional day */
    time.second = timestamp % 60;
    timestamp /= 60;
    time.minute = timestamp % 60;
    timestamp /= 60;
    time.hour = timestamp % 24;

    /*
     * Our epoch year has the property of being at the conjunction of all three 'leap cycles',
     * 4, 100, and 400 years ( though we can ignore the 400 year cycle in this library).
     *
     * Using this property, we can easily 'map' the time stamp into the leap cycles, quickly
     * deriving the year and day of year, along with the fact of whether it is a leap year.
     */

    /* map into a 100 year cycle */
    result = Divide(days, 36525L);
    years = 100 * result.quotient;

    /* map into a 4 year cycle */
    result = Divide(result.remainder, 1461L);
    years += 4 * result.quotient;
    days = result.remainder;
    if (years > 100)
    {
        days++;
    }

    /*
     * 'years' is now at the first year of a 4 year leap cycle, which will always be a leap year,
     * unless it is 100. 'days' is now an index into that cycle.
     */
    leapyear = 1;
    if (years == 100)
    {
        leapyear = 0;
    }

    /* compute length, in days, of first year of this cycle */
    n = 364 + leapyear;

    /*
     * if the number of days remaining is greater than the length of the
     * first year, we make one more division.
     */
    if (days > n)
    {
        days -= leapyear;
        leapyear = 0;
        result = Divide(days, 365);
        years += result.quotient;
        days = result.remainder;
    }
    time.year = years;

    /*
     * Given the year, day of year, and leap year indicator, we can break down the
     * month and day of month. If the day of year is less than 59 (or 60 if a leap year), then
     * we handle the Jan/Feb month pair as an exception.
     */
    n = 59 + leapyear;
    if (days < n)
    {
        /* special case: Jan/Feb month pair */
        result = Divide(days, 31);
        time.month = result.quotient;
        time.date = result.remainder;
    }
    else
    {
        /*
        * The remaining 10 months form a regular pattern of 31 day months alternating with 30 day
        * months, with a 'phase change' between July and August (153 days after March 1).
        * We proceed by mapping our position into either March-July or August-December.
        */
        days -= n;
        result = Divide(days, 153);
        time.month = 2 + result.quotient * 5;

        /* map into a 61 day pair of months */
        result = Divide(result.remainder, 61);
        time.month += result.quotient * 2;

        /* map into a month */
        result = Divide(result.remainder, 31);
        time.month += result.quotient;
        time.date = result.remainder;
    }

    /* Month and date has offset of one. */
    time.month += 1;
    time.date += 1;

    return time;
}

uint32_t Time_ConvertToTimestamp(const struct time_t *time_p)
{
    sc_assert(time_p != NULL);

    uint32_t tmp;
    int n;
    int d;
    int leaps;
    int month;

    /**
     * Determine elapsed whole days since the epoch to the beginning of this
     * year. Since our epoch is at a conjunction of the leap cycles, we can do
     * this rather quickly.
     */
    n = time_p->year;
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

    /**
     * Derive the day of year from month and day of month. We use the pattern
     * of 31 day months followed by 30 day months to our advantage, but we must
     * 'special case' Jan/Feb, and account for a 'phase change' between July
     * and August (153 days after March 1).
     */
    d = time_p->date - 1; /* Date is one based. */

    /* Month has offset of one. */
    month = time_p->month - 1;

    /* Handle Jan/Feb as a special case. */
    if (month < 2)
    {
        if (month)
        {
            d += 31;
        }
    }
    else
    {
        uint32_t m;
        n = 59;
        if (Time_IsLeapYear(time_p))
        {
            n += 1;
        }
        d += n;
        n = month - MARCH + 1;

        /* Account for phase change. */
        if (n > (JULY - MARCH))
        {
            d += 153;
        }
        n %= 5;

        /**
         * n is now an index into a group of alternating 31 and 30 day
         * months... 61 day pairs.
         */
        m = n / 2;
        m *= 61;
        d += m;

        /* If n is odd, we are in the second half of the month pair. */
        if (n & 1)
        {
            d += 31;
        }
    }

    uint32_t return_value;

    /* Add day of year to elapsed days, and convert to seconds. */
    tmp += d;
    tmp *= ONE_DAY_S;
    return_value = tmp;

    /* Compute 'fractional' day. */
    tmp = time_p->hour;
    tmp *= ONE_HOUR_S;
    tmp += time_p->minute * 60UL;
    tmp += time_p->second;

    return return_value + tmp;
}

uint8_t Time_GetDaysInMonth(const struct time_t *time_p)
{
    sc_assert(time_p != NULL);
    sc_assert(time_p->month > 0 && time_p->month <= ElementsIn(days_in_months));

    if (time_p->month == FEBRUARY && Time_IsLeapYear(time_p))
    {
        return days_in_months[time_p->month - 1] + 1;
    }
    else
    {
        return days_in_months[time_p->month - 1];
    }

}

uint8_t Time_CalculateDayOfWeek(const struct time_t *time_p)
{
    sc_assert(time_p != NULL);

    const uint8_t t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    uint16_t year = time_p->year + 2000;

    year -= time_p->month < 3;

    uint16_t leap_year_adjustment;
    leap_year_adjustment = year / 4 - year / 100 + year / 400;

    return (year + leap_year_adjustment + t[time_p->month - 1] + time_p->date) % 7;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////
