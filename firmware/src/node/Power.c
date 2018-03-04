/**
 * @file   Power.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-03-04 (Last edit)
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
#include "driverCharger.h"

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

static power_state_type battery_state;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

static void BatteryMonitoringSM(void);

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
    battery_state = POWER_NORMAL;

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
    BatteryMonitoringSM();
    return;
}

///
/// @brief Handle wake up event.
///
/// @param  *event Pointer to triggered event
/// @return None
///
void Power_WakeUp(const event_type *event __attribute__ ((unused)))
{
    sc_assert(event != NULL);

    DEBUG("Battery voltage: %u\r\n", driverCharger_GetBatteryVoltage());
    DEBUG("Charger connected: %u\r\n", (uint8_t)driverCharger_IsConnected());
    DEBUG("Charging: %u\r\n", (uint8_t)driverCharger_IsCharging());
    return;
}
//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static void BatteryMonitoringSM(void)
{
    switch (battery_state)
    {
        case POWER_NORMAL:
            if (driverCharger_IsCharging() == true)
            {
                INFO("Charging started");
                event_type event = Event_New(EVENT_BATTERY_CHARGING_STARTED);
                Event_Trigger(&event);
                battery_state = POWER_CHARGING;
            }
            else if (driverCharger_GetBatteryVoltage() < LOW_VOLTAGE_MV)
            {
                INFO("Low battery voltage[%u mV]", driverCharger_GetBatteryVoltage());
                event_type event = Event_New(EVENT_BATTERY_LOW);
                Event_Trigger(&event);
                battery_state = POWER_LOW;
            }
            break;

        case POWER_LOW:
            if (driverCharger_IsCharging() == true)
            {
                INFO("Charging started");
                event_type event = Event_New(EVENT_BATTERY_CHARGING_STARTED);
                Event_Trigger(&event);
                battery_state = POWER_CHARGING;
            }
            else if (driverCharger_GetBatteryVoltage() < CRITICAL_VOLTAGE_MV)
            {
                WARNING("Critical battery voltage");
                event_type event = Event_New(EVENT_BATTERY_CRITICAL);
                Event_Trigger(&event);
                battery_state = POWER_CRITICAL;
            }
            break;

        case POWER_CRITICAL:
            if (driverCharger_IsCharging() == true)
            {
                INFO("Charging started");
                event_type event = Event_New(EVENT_BATTERY_CHARGING_STARTED);
                Event_Trigger(&event);
                battery_state = POWER_CHARGING;
            }
            break;

        case POWER_CHARGING:
            if (driverCharger_IsCharging() == false)
            {
                INFO("Charging stopped");
                event_type event = Event_New(EVENT_BATTERY_CHARGING_STOPPED);
                Event_Trigger(&event);
                battery_state = POWER_CONNECTED;
            }
            break;

        case POWER_CONNECTED:
            if (driverCharger_IsCharging() == true)
            {
                INFO("Charging started");
                event_type event = Event_New(EVENT_BATTERY_CHARGING_STARTED);
                Event_Trigger(&event);
                battery_state = POWER_CHARGING;
            }

            if (driverCharger_IsConnected() == false)
            {
                INFO("Charger disconnected");
                event_type event = Event_New(EVENT_BATTERY_CHARGER_DISCONNECTED);
                Event_Trigger(&event);
                battery_state = POWER_NORMAL;
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
