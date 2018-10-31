/**
 * @file   RTC.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-10-31 (Last edit)
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

#define RTC_HAL

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#ifdef RTC_HAL
#include "RTC_HAL.h"
#endif

#include <stdbool.h>
#include "Time.h"

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

#ifdef RTC_HAL

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
#endif

#endif
