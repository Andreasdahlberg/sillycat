/**
 * @file   RTC.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2015-11-29 (Last edit)
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

#define DAYS_IN_MARCH_OCTOBER 31
#define DAYS_IN_WEEK 7

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

//TODO: Remove this function, implement time struct to string instead
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

bool RTC_GetCurrentTime(rtc_time_type *time)
{
    return (libDS3234_GetYear(&time->year) &&
            libDS3234_GetMonth(&time->month) &&
            libDS3234_GetDate(&time->date) &&
            libDS3234_GetHour(&time->hour) &&
            libDS3234_GetMinutes(&time->minute) &&
            libDS3234_GetSeconds(&time->second));
}

bool RTC_SetCurrentTime(rtc_time_type *time)
{
    return (libDS3234_SetYear(time->year) &&
            libDS3234_SetMonth(time->month) &&
            libDS3234_SetDate(time->date) &&
            libDS3234_SetHour(time->hour) &&
            libDS3234_SetMinutes(time->minute) &&
            libDS3234_SetSeconds(time->second));
}

//TODO: Fixe edge cases!
bool RTC_IsDaylightSavingActive(void)
{
    rtc_time_type time;
    bool dst_active;
    uint8_t next_sunday;
    uint8_t week_day;

    if (RTC_GetCurrentTime(&time) != TRUE || libDS3234_GetDay(&week_day) != TRUE)
    {
        return FALSE;
    }

    if (time.month > MARCH && time.month < OCTOBER)
    {
        dst_active = TRUE;
    }
    else if (time.month < MARCH || time.month > OCTOBER)
    {
        dst_active = FALSE;
    }
    else
    {
        next_sunday = time.date + (DAYS_IN_WEEK - week_day);

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

        dst_active = ((time.month == MARCH && time.date > next_sunday) ||
                      (time.month == OCTOBER && time.date < next_sunday));
    }
    return dst_active;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////