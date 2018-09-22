/**
 * @file   Timer.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-09-22 (Last edit)
 * @brief  Module with timer related functions.
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

#ifndef TIMER_H_
#define TIMER_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

/**
 * Initialize the system timer.
 */
void Timer_Init(void);

/**
 * Reset the system timer.
 */
void Timer_Reset(void);

/**
 *  Get the current time(ms).
 *
 * @return The current system time in milliseconds.
 */
uint32_t Timer_GetMilliseconds(void);

/**
 *  Get the current time(s).
 *
 * @return The current system time in seconds.
 */
uint32_t Timer_GetSeconds(void);

/**
 * Get the difference between the system time and the supplied timestamp(ms).
 *
 * Protected against timer wrap-around. CAUTION: Does not detect several
 * wrap-arounds but this is a very long time(over 49 days).
 *
 * @param time_ms Timestamp in ms.
 *
 * @return The difference in ms between the timestamp and the system time.
 */
uint32_t Timer_TimeDifference(uint32_t time_ms);

#endif
