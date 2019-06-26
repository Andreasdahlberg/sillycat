/**
 * @file   Timer.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2019-06-27 (Last edit)
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

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include "driverTimer.h"
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

void Timer_Init(void)
{
    driverTimer_Init();
}

void Timer_Reset(void)
{
    driverTimer_Reset();
}

uint32_t Timer_GetMilliseconds(void)
{
    return driverTimer_GetMilliseconds();
}

uint32_t Timer_GetSeconds(void)
{
    return ((Timer_GetMilliseconds() + 500) / 1000);
}

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
