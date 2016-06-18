/**
 * @file   Timer.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2015-11-06 (Last edit)
 * @brief  Implementation of Timer functions
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

#include "Timer.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define TIMERMAX (uint32_t)0xFFFFFFFF

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

///
/// @brief Get the current time(s).
///
/// @param  None
/// @return uint32_t The current system time in seconds
///
uint32_t Timer_GetSeconds()
{
    return ((Timer_GetMilliseconds() + 500) / 1000);
}

///
/// @brief Get the difference between the system timer and the supplied timestamp(ms).
///		   Protected against timer wrap around. CAUTION: Does not detect several
///        wrap arounds but this is a very long time(over 49 days).
///
/// @param  time_ms Timestamp in ms to compare with system timer
/// @return uint32_t The difference in ms beetween timestamp and system timer
///
uint32_t Timer_TimeDifference(uint32_t time_ms)
{
    uint32_t time_difference;
    uint32_t current_time = Timer_GetMilliseconds();

    //Check for timer overflow
    if (time_ms > current_time)
    {
        time_difference = (TIMERMAX - time_ms) + current_time;
    }
    else
    {
        time_difference = current_time - time_ms;
    }
    return time_difference;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////