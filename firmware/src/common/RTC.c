/**
 * @file   RTC.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2021-02-13 (Last edit)
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
#include "driverRTC.h"
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

void RTC_Init(void)
{
    driverRTC_Init();
}

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

    struct driverRTC_time_t rtc_time;
    driverRTC_GetTime(&rtc_time);
    time_p->year = rtc_time.year;
    time_p->month = rtc_time.month;
    time_p->date = rtc_time.date;
    time_p->hour = rtc_time.hour;
    time_p->minute = rtc_time.minute;
    time_p->second = rtc_time.second;
    return true;
}

bool RTC_SetCurrentTime(const struct time_t *time_p)
{
    sc_assert(time_p != NULL);

    return driverRTC_SetYear(time_p->year) &&
           driverRTC_SetMonth(time_p->month) &&
           driverRTC_SetDate(time_p->date) &&
           driverRTC_SetHour(time_p->hour) &&
           driverRTC_SetMinute(time_p->minute) &&
           driverRTC_SetSecond(time_p->second);
}

bool RTC_SetAlarmTime(const struct time_t *time_p)
{
    sc_assert(time_p != NULL);

    return driverRTC_SetAlarmHour(time_p->hour) &&
           driverRTC_SetAlarmMinute(time_p->minute) &&
           driverRTC_SetAlarmSecond(time_p->second);
}

void RTC_EnableAlarm(bool enable)
{
    driverRTC_EnableAlarm(enable);
}

void RTC_ClearAlarm(void)
{
    driverRTC_ClearAlarmFlag();
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////
