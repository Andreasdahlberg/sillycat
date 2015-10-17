/**
 * @file   RTC.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2015-10-17 (Last edit)
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

#ifdef IS_MAIN
#include "libDS3234.h"
#endif

#include "RTC.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define TIMESTAMP_FORMAT "20%02u-%02u-%02u %02u:%02u:%02u"

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

void RTC_GetTimestamp(char *timestamp)
{
    rtc_time_type time;

    if (RTC_GetCurrentTime(&time))
    {
        sprintf(timestamp, TIMESTAMP_FORMAT, time.year, time.month, time.date,
                time.hour, time.minute, time.second);
    }
    else
    {
        ERROR("Failed to get the current time");
    }
    return;
}

//TODO: Implement abstraction layer
bool RTC_GetCurrentTime(rtc_time_type *current_time)
{
    return (libDS3234_GetYear(&current_time->year) &&
            libDS3234_GetMonth(&current_time->month) &&
            libDS3234_GetDate(&current_time->date) &&
            libDS3234_GetHour(&current_time->hour) &&
            libDS3234_GetMinutes(&current_time->minute) &&
            libDS3234_GetSeconds(&current_time->second));
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////