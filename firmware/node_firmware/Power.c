/**
 * @file   Power.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2015-12-12 (Last edit)
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

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define MAX_NR_LISTENERS 5

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static uint8_t nr_listeners = 0;
static uint32_t time_since_sleep = 0;
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

    if (IsTimeForSleep() == TRUE)
    {
        event = NewEvent(0);
        NotifyListeners(&event);

        //TODO: Do this in Debug sleep event handler.
        libUART_WaitForEmptyBuffer();

        //Enter sleep mode, execution will continue from this point
        //after sleep is done.
        //TODO: Set RTC wakeup alarm here
        libPower_Sleep();

        event = NewEvent(1);
        NotifyListeners(&event);
        time_since_sleep = event.timestamp;
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
    return (Timer_TimeDifference(time_since_sleep) > 5000);
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