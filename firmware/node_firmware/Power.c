/**
 * @file   Power.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2016-01-31 (Last edit)
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
#include "Event.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

#define LOW_VOLTAGE_MV 2200
#define CRITICAL_VOLTAGE_MV 1900

typedef enum
{
    POWER_INIT = 0,
    POWER_NORMAL,
    POWER_LOW,
    POWER_CRITICAL,
    POWER_CHARGING,
    POWER_CONNECTED
} power_state_type;

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static power_state_type state = POWER_INIT;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

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
    state = POWER_NORMAL;

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
    sc_assert(state != POWER_INIT);

    switch (state)
    {
        case POWER_NORMAL:

            if (libPower_IsCharging() == true)
            {
                event_type event = {0};

                INFO("Charging started");
                event = Event_New(EVENT_BATTERY_CHARGING_STARTED);
                Event_Trigger(&event);
                state = POWER_CHARGING;
            }
            else if (libPower_IsBatteryVoltageValid() == true
                     && libPower_GetBatteryVoltage() < LOW_VOLTAGE_MV)
            {
                event_type event = {0};

                INFO("Low battery voltage");
                event = Event_New(EVENT_BATTERY_LOW);
                Event_Trigger(&event);
                state = POWER_LOW;
            }
            break;

        case POWER_LOW:
            if (libPower_IsCharging() == true)
            {
                event_type event = {0};

                INFO("Charging started");
                event = Event_New(EVENT_BATTERY_CHARGING_STARTED);
                Event_Trigger(&event);
                state = POWER_CHARGING;
            }
            else if (libPower_IsBatteryVoltageValid() == true
                     && libPower_GetBatteryVoltage() < CRITICAL_VOLTAGE_MV)
            {
                event_type event = {0};

                WARNING("Critical battery voltage");
                event = Event_New(EVENT_BATTERY_CRITICAL);
                Event_Trigger(&event);
                state = POWER_CRITICAL;
            }
            break;

        case POWER_CRITICAL:
            if (libPower_IsCharging() == true)
            {
                event_type event = {0};

                INFO("Charging started");
                event = Event_New(EVENT_BATTERY_CHARGING_STARTED);
                Event_Trigger(&event);
                state = POWER_CHARGING;
            }
            break;

        case POWER_CHARGING:
            if (libPower_IsCharging() == false)
            {
                event_type event = {0};

                INFO("Charging stopped");
                event = Event_New(EVENT_BATTERY_CHARGING_STOPPED);
                Event_Trigger(&event);
                state = POWER_CONNECTED;
            }
            break;

        case POWER_CONNECTED:
            if (libPower_IsCharging() == true)
            {
                event_type event = {0};

                INFO("Charging started");
                event = Event_New(EVENT_BATTERY_CHARGING_STARTED);
                Event_Trigger(&event);
                state = POWER_CHARGING;
            }

            if (libPower_IsChargerConnected() == false)
            {
                event_type event = {0};

                INFO("Charger disconnected");
                event = Event_New(EVENT_BATTERY_CHARGER_DISCONNECTED);
                Event_Trigger(&event);
                state = POWER_NORMAL;
            }
            break;

        default:
            sc_assert_fail();
            break;
    }

    return;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////