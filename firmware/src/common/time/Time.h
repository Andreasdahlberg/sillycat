/**
 * @file   Time.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2019-06-05 (Last edit)
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

#ifndef TIME_H_
#define TIME_H_

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include <stdbool.h>
#include <stdint.h>

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

struct time_t
{
    uint8_t year;
    uint8_t month;
    uint8_t date;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
};

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
} time_month_t;

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

/**
 * Get the supplied time as a formated string timestamp.
 *
 * @param time_p      Pointer to time struct.
 * @param timestamp_p Pointer to char array where the timestamp will be stored.
 * @param size        Max size of the returned timestamp.
 */
void Time_FormatTimestamp(const struct time_t *time_p,
                          char *timestamp_p,
                          size_t size
                         );

/**
 * Check if daylight time savings is active.
 *
 * Only valid for DST in Sweden!
 *
 * @param time_p Pointer to time struct.
 *
 * @return True if DST is active, otherwise false.
 */
bool Time_IsDaylightSavingActive(const struct time_t *time_p);

/**
 * Check if it's a leap year.
 *
 * @param time_p Pointer to time struct.
 *
 * @return True if leap year, otherwise false.
 */
bool Time_IsLeapYear(const struct time_t *time_p);

/**
 * Add seconds to the supplied time.
 *
 * @param time_p  Pointer to time struct.
 * @param seconds Number of seconds to add.
 */
void Time_AddSeconds(struct time_t *time_p, uint8_t seconds);

/**
 * Add minutes to the supplied time.
 *
 * @param time_p  Pointer to time struct.
 * @param minutes Number of minutes to add.
 */
void Time_AddMinutes(struct time_t *time_p, uint8_t minutes);

/**
 * Add hours to the supplied time.
 *
 * @param time_p Pointer to time struct.
 * @param hours  Number of hours to add.
 */
void Time_AddHours(struct time_t *time_p, uint8_t hours);

/**
 * Add days to the supplied time.
 *
 * @param time_p Pointer to time struct.
 * @param days   Number of days to add.
 */
void Time_AddDays(struct time_t *time_p, uint8_t days);

/**
 * Convert a timestamp into an broken down time struct.
 *
 * @author Michael Duane Rice, Original.
 * @author Andreas Dahlberg, Adaptations.
 *
 * @param  timestamp Timestamp in seconds.
 *
 * @return Time struct.
 */
struct time_t Time_ConvertFromTimestamp(uint32_t timestamp);

/**
 * Convert an broken down time struct into number of seconds since midnight,
 * January 1 2000, UTC.
 *
 * @author Michael Duane Rice, Original.
 * @author Andreas Dahlberg, Adaptations.
 *
 * @param time_p Pointer to time struct.
 *
 * @return Timestamp in seconds.
 */
uint32_t Time_ConvertToTimestamp(const struct time_t *time_p);

/**
 * Get the number of days in the supplied month.
 *
 * @param time_p Pointer to time struct.
 *
 * @return Number of days in month.
 */
uint8_t Time_GetDaysInMonth(const struct time_t *time_p);

/**
 * Calculate the week day of the supplied time.
 *
 * @param time_p Pointer to time struct.
 *
 * @return Week day index.
 */
uint8_t Time_CalculateDayOfWeek(const struct time_t *time_p);

#endif
