/**
 * @file   Power.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2016-01-17 (Last edit)
 * @brief  Implementation of Power manager
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

//NOTE: Include before all other headers
#include "common.h"

#include "libDebug.h"
#include "libPower.h"

#include "Timer.h"
#include "Power.h"
#include "RTC.h"
#include "Config.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define MAX_NR_LISTENERS 5

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

#define LOW_VOLTAGE_MV 2100
#define CRITICAL_VOLTAGE_MV 1900

#define AWAKE_TIME_S 3

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static uint8_t nr_listeners;
static uint32_t time_since_sleep;
static event_listener listener_pool[MAX_NR_LISTENERS] = {0};

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

static bool IsTimeForSleep(void);
static void NotifyListeners(event_type *event);

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

///
/// @brief Init Power module
///
/// @param  None
/// @return None
///
void Power_Init(void)
{
    sc_assert(Config_GetReportInterval() > AWAKE_TIME_S);

    nr_listeners = 0;
    time_since_sleep = Timer_GetMilliseconds();
    return;
}

///
/// @brief Update the internal state of the Power module
///
/// @param  None
/// @return None
///
void Power_Update(void)
{
    static event_type event = {0};

    RTC_ClearAlarm();
    RTC_EnableAlarm(FALSE);

    if (IsTimeForSleep() == TRUE)
    {
        event = NewEvent(EVENT_SLEEP);
        NotifyListeners(&event);

        rtc_time_type time;
        RTC_GetCurrentTime(&time);
        DEBUG("%u:%u:%u\r\n", time.hour, time.minute, time.second);

        RTC_AddSeconds(&time, Config_GetReportInterval() - AWAKE_TIME_S);
        RTC_SetAlarmTime(&time);
        RTC_EnableAlarm(TRUE);

        //TODO: Do this in Debug sleep event handler.
        libUART_WaitForEmptyBuffer();

        //Enter sleep mode, execution will continue from this point
        //after sleep is done.
        libPower_Sleep();
        RTC_ClearAlarm();
        RTC_EnableAlarm(FALSE);

        RTC_GetCurrentTime(&time);
        DEBUG("%u:%u:%u\r\n", time.hour, time.minute, time.second);

        event = NewEvent(EVENT_WAKEUP);
        NotifyListeners(&event);
        time_since_sleep = event.timestamp;
    }

    if (libPower_IsBatteryVoltageValid() == TRUE &&
            libPower_GetBatteryVoltage() < CRITICAL_VOLTAGE_MV)
    {
        WARNING("Critical battery voltage");

        event = NewEvent(EVENT_SLEEP);
        NotifyListeners(&event);

        while (1)
        {
            //NOTE: No wake up alarm is set here. The wake up interrupt is triggered
            //      by the charger circuit when a battery charger is connected.
            libPower_Sleep();
            RTC_ClearAlarm();

            //Make sure the battery is charging before exiting sleep.
            //This should never happen, an assert would be better here but
            //would risk to completely deplete the battery if it happens.
            if (libPower_IsChargerConnected() == TRUE)
            {
                event = NewEvent(EVENT_WAKEUP);
                NotifyListeners(&event);
                break;
            }
            CRITICAL("Wakeup from critical battery sleep");
        }
    }
    return;
}

///
/// @brief Add listener event handler to events from Power module
///
/// @param  listener Function pointer to an event handler
/// @return None
///
void Power_AddListener(event_listener listener)
{
    sc_assert(listener != NULL);
    sc_assert(nr_listeners < MAX_NR_LISTENERS);

    listener_pool[nr_listeners] = listener;
    ++nr_listeners;

    return;
}

//TODO: Implement functionality to add functions to call before sleep, if
//      they return FALSE, wait before sleeping. Use timeout!

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static bool IsTimeForSleep(void)
{
    //TODO: Check if for active modules here?
    return (Timer_TimeDifference(time_since_sleep) > (AWAKE_TIME_S * 1000));
}

static void NotifyListeners(event_type *event)
{
    sc_assert(nr_listeners < MAX_NR_LISTENERS);

    uint8_t index;
    for (index = 0; index < nr_listeners; ++index)
    {
        sc_assert(listener_pool[index] != NULL);

        DEBUG("Notify listener: %u\r\n", index);
        listener_pool[index](event);
    }
    return;
}