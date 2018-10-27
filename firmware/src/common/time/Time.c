/**
 * @file   Time.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-10-27 (Last edit)
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

    timestamp_p[size] = '\0';
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
        dst_active = ((time_p->month == MARCH && time_p->date > next_sunday) ||
                      (time_p->month == OCTOBER && time_p->date < next_sunday));
    }
    return dst_active;
}

bool Time_IsLeapYear(const struct time_t *time_p)
{
    sc_assert(time_p != NULL);
    struct time_t tmp_time = *time_p;
    bool is_leap_year = false;

    tmp_time.year += 2000;

    //All leap years are divisible by 4
    if (tmp_time.year % 4 == 0)
    {
        if (tmp_time.year % 100 != 0 || tmp_time.year % 400 == 0)
        {
            is_leap_year = true;
        }
    }

    return is_leap_year;
}

void Time_AddSeconds(struct time_t *time_p, uint8_t seconds)
{
    sc_assert(time_p != NULL);
    uint16_t carry;

    carry = (time_p->second + seconds) / 60;
    time_p->second = (time_p->second + seconds) % 60;

    if (carry > 0)
    {
        Time_AddMinutes(time_p, carry);
    }
}

void Time_AddMinutes(struct time_t *time_p, uint8_t minutes)
{
    sc_assert(time_p != NULL);
    uint16_t carry;

    carry = (time_p->minute + minutes) / 60;
    time_p->minute = (time_p->minute + minutes) % 60;

    if (carry > 0)
    {
        Time_AddHours(time_p, carry);
    }
}

void Time_AddHours(struct time_t *time_p, uint8_t hours)
{
    sc_assert(time_p != NULL);
    uint16_t carry;

    carry =  (time_p->hour + hours) / 24;
    time_p->hour = (time_p->hour + hours) % 24;

    if (carry > 0)
    {
        Time_AddDays(time_p, carry);
    }
}

void Time_AddDays(struct time_t *time_p, uint8_t days)
{
    sc_assert(time_p != NULL);
    uint16_t carry;
    uint8_t days_in_month;

    days_in_month = Time_GetDaysInMonth(time_p);

    carry = (time_p->date + days) / days_in_month;
    time_p->date = (time_p->date + days) % days_in_month;

    //TODO: Fix cases when carry > 1
    sc_assert(carry < 2);

    if (carry > 0)
    {
        Time_AddMonths(time_p, carry);
    }
}

void Time_AddMonths(struct time_t *time_p, uint8_t months)
{
    sc_assert(time_p != NULL);
    uint16_t carry;

    carry =  (time_p->month + months) / 12;
    time_p->month = (time_p->month + months) % 12;

    if (carry > 0)
    {
        Time_AddYears(time_p, carry);
    }
}

void Time_AddYears(struct time_t *time_p, uint8_t years)
{
    sc_assert(time_p != NULL);
    time_p->year += years;

    if (time_p->year > 99)
    {
        time_p->year = 99;
    }
}

uint32_t Time_ConvertToTimestamp(const struct time_t *time_p)
{
    sc_assert(time_p != NULL);

    uint32_t tmp;
    int n;
    int d;
    int leaps;

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

    /* Handle Jan/Feb as a special case. */
    if (time_p->month < 2)
    {
        if (time_p->month)
        {
            d += 31;
        }
    }
    else
    {
        uint32_t m;
        n = 59 + Time_IsLeapYear(time_p);
        d += n;
        n = time_p->month - MARCH;

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
    sc_assert(time_p->month <= ElementsIn(days_in_months));
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
