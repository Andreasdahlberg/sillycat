/**
 * @file   RTC.c
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

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include "common.h"
#include "RTC.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

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

#ifdef RTC_HAL
bool RTC_GetTimeStamp(uint32_t *timestamp_p)
{
    sc_assert(timestamp_p != NULL);
    struct time_t time;

    if (RTC_GetCurrentTime(&time) == true)
    {
        *timestamp_p = Time_ConvertToTimestamp(&time);
        return true;
    }
    return false;
}

bool RTC_GetCurrentTime(struct time_t *time_p)
{
    sc_assert(time_p != NULL);

    RTC_GetYear(&time_p->year);
    RTC_GetMonth(&time_p->month);
    RTC_GetDate(&time_p->date);
    RTC_GetHour(&time_p->hour);
    RTC_GetMinutes(&time_p->minute);
    RTC_GetSeconds(&time_p->second);
    return true;
}

bool RTC_SetCurrentTime(const struct time_t *time_p)
{
    sc_assert(time_p != NULL);

    return RTC_SetYear(time_p->year) &&
           RTC_SetMonth(time_p->month) &&
           RTC_SetDate(time_p->date) &&
           RTC_SetHour(time_p->hour) &&
           RTC_SetMinutes(time_p->minute) &&
           RTC_SetSeconds(time_p->second);
}

bool RTC_SetAlarmTime(const struct time_t *time_p)
{
    sc_assert(time_p != NULL);

    return RTC_SetAlarmHour(time_p->hour) &&
           RTC_SetAlarmMinutes(time_p->minute) &&
           RTC_SetAlarmSeconds(time_p->second);
}
#endif

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////
