/**
 * @file   RTC.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2021-01-14 (Last edit)
 * @brief  Implementation of RTC interface
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
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include <stdbool.h>
#include <stdint.h>
#include "Time.h"

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

/**
 * Initialize the RTC.
 */
void RTC_Init(void);

/**
 * Get the number of seconds since midnight, January 1 2000, UTC.
 *
 * @param timestamp_p Pointer to timestamp where the result will be stored.
 *
 * @return True if current time was successfully read, otherwise false.
 */
bool RTC_GetTimeStamp(uint32_t *timestamp_p);

/**
 * Get a time struct with fields for different time periods.
 *
 * @param time_p Pointer to struct where the result will be stored.
 *
 * @return True, if current time was successfully read, otherwise false.
 */
bool RTC_GetCurrentTime(struct time_t *time_p);

/**
 * Set the current time.
 *
 * @param time_p Pointer to struct with time to set.
 *
 * @return True if new time was set successfully, otherwise false.
 */
bool RTC_SetCurrentTime(const struct time_t *time_p);

/**
 * Set an alarm time.
 *
 * @param time_p Pointer to struct with alarm time to set.
 *
 * @return True if new alarm time was set successfully, otherwise false.
 */
bool RTC_SetAlarmTime(const struct time_t *time_p);

/**
 * Enable the alarm.
 * @param enable
 */
void RTC_EnableAlarm(bool enable);

/**
 * Clear the alarm.
 */
void RTC_ClearAlarm(void);

#endif
