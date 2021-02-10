/**
 * @file   driverRTC.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2021-02-10 (Last edit)
 * @brief  Virtual RTC driver.
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

#ifndef DRIVERRTC_H_
#define DRIVERRTC_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stdbool.h>

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

struct driverRTC_time_t
{
    uint8_t year;
    uint8_t month;
    uint8_t date;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
};

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

/**
 * Initialize the RTC.
 */
void driverRTC_Init(void);

/**
 * Get the current time.
 *
 * @param time_p Pointer to variable where the result will be stored.
 */
void driverRTC_GetTime(struct driverRTC_time_t *time_p);

/**
 * Get the current year.
 *
 * @param year_p Pointer to variable where the result will be stored(0-99).
 */
void driverRTC_GetYear(uint8_t *year_p);

/**
 * Set the year.
 *
 * @param year Years since 2000 (0-99).
 *
 * @return True if the value was set, otherwise false.
 */
bool driverRTC_SetYear(uint8_t year);

/**
 * Get the current month.
 *
 * @param month_p Pointer to variable where the result will be stored(1-12).
 */
void driverRTC_GetMonth(uint8_t *month_p);

/**
 * Set the month.
 *
 * @param month Month(1-12).
 *
 * @return True if the value was set, otherwise false.
 */
bool driverRTC_SetMonth(uint8_t month);

/**
 * Get the current date.
 *
 * @param date_p Pointer to variable where the result will be stored(1-31).
 */
void driverRTC_GetDate(uint8_t *date_p);

/**
 * Set the date.
 *
 * @param date Date(1-31).
 *
 * @return True if the value was set, otherwise false.
 */
bool driverRTC_SetDate(uint8_t date);

/**
 * Get the current day of the week.
 *
 * @param date_p Pointer to variable where the result will be stored(1-7).
 */
void driverRTC_GetDay(uint8_t *day_p);

/**
 * Set the day of the week.
 *
 * @param day Day of the week(1-7).
 */
bool driverRTC_SetDay(uint8_t day);

/**
 * Get the current hour.
 *
 * The range of the returned value depends on the set hour mode, 12h or 24h.
 *
 * @param hour_p Pointer to variable where the result will be stored(0-23).
 */
void driverRTC_GetHour(uint8_t *hour_p);

/**
 * Set the hour.
 *
 * @param hour Hour(0-23).
 *
 * @return True if the value was set, otherwise false.
 */
bool driverRTC_SetHour(uint8_t hour);

/**
 * Get the current minute.
 *
 * @param minute_p Pointer to variable where the result will be stored(0-59).
 */
void driverRTC_GetMinute(uint8_t *minute_p);

/**
 * Set the minute.
 *
 * @param minute Minute(0-59).
 *
 * @return True if the value was set, otherwise false.
 */
bool driverRTC_SetMinute(uint8_t minute);

/**
 * Get the current second.
 *
 * @param second_p Pointer to variable where the result will be stored(0-59).
 */
void driverRTC_GetSecond(uint8_t *second_p);

/**
 * Set the second.
 *
 * @param second Second(0-59).
 *
 * @return True if the value was set, otherwise false.
 */
bool driverRTC_SetSecond(uint8_t second);

/**
 * Check if 24-hour mode is enabled.
 *
 * @return True if enabled, otherwise false.
 */
bool driverRTC_Is24HourMode(void);

/**
 * Enable an alarm.
 *
 * @param enable True for enable, otherwise false.
 */
void driverRTC_EnableAlarm(bool enable);

/**
 * Clear the alarm flag.
 */
void driverRTC_ClearAlarmFlag(void);

/**
 * Set the alarm date.
 *
 * @param date Alarm date(1-31).
 *
 * @return True if the value was set, otherwise false.
 */
bool driverRTC_SetAlarmDate(uint8_t date);

/**
 * Set the alarm hour.
 *
 * @param hour Alarm hour(0-23).
 *
 * @return True if the value was set, otherwise false.
 */
bool driverRTC_SetAlarmHour(uint8_t hour);

/**
 * Set the alarm minute.
 *
 * @param minute Alarm minute(0-59).
 *
 * @return True if the value was set, otherwise false.
 */
bool driverRTC_SetAlarmMinute(uint8_t minute);

/**
 * Set the alarm second.
 *
 * @param second Alarm second(0-59).
 *
 * @return True if the value was set, otherwise false.
 */
bool driverRTC_SetAlarmSecond(uint8_t second);

#endif
