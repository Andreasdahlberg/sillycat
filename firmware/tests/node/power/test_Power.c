/**
 * @file   test_Power.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2020-10-27 (Last edit)
 * @brief  Test suite for the Power module.
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

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include <stdbool.h>

#include "Power.h"
#include "Battery.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define NORMAL_VOLTAGE      (BATTERY_LOW_VOLTAGE_MV)
#define LOW_VOLTAGE         (BATTERY_LOW_VOLTAGE_MV - 1)
#define CRITICAL_VOLTAGE    (BATTERY_CRITICAL_VOLTAGE_MV - 1)

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

static int Setup(void **state);
static int check_event_id(unsigned long a, unsigned long b);
static void ExpectEvent(event_id_t id);

//////////////////////////////////////////////////////////////////////////
//INTERUPT SERVICE ROUTINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

event_t dummy_event;

static int Setup(void **state)
{
    dummy_event = (__typeof__(dummy_event)) {0};
    Power_Init();
    return 0;
}


static int check_event_id(unsigned long a, unsigned long b)
{
    if (((event_t *)a)->id == b)
    {
        return  1;
    }
    else
    {
        printf("Incorrect event triggered(0x2%x != 0x%lx\n)", ((event_t *)a)->id, b);
        return 0;
    }
}

static void ExpectEvent(event_id_t id)
{
    will_return(__wrap_Timer_GetMilliseconds, 0);
    expect_check(__wrap_Event_Trigger, event_p, check_event_id, id);
}

static void EnterLowState(void)
{
    will_return(__wrap_driverCharger_IsCharging, false);
    will_return(__wrap_driverCharger_GetBatteryVoltage, LOW_VOLTAGE);
    ExpectEvent(EVENT_BATTERY_LOW);
    Power_Update();
}

static void EnterCriticalState(void)
{
    EnterLowState();

    will_return(__wrap_driverCharger_IsCharging, false);
    will_return(__wrap_driverCharger_GetBatteryVoltage, CRITICAL_VOLTAGE);
    ExpectEvent(EVENT_BATTERY_CRITICAL);
    Power_Update();
}

static void EnterChargingState(void)
{
    will_return(__wrap_driverCharger_IsCharging, true);
    ExpectEvent(EVENT_BATTERY_CHARGING_STARTED);
    Power_Update();
}

static void EnterConnectedState(void)
{
    EnterChargingState();

    will_return(__wrap_driverCharger_IsCharging, false);
    ExpectEvent(EVENT_BATTERY_CHARGING_STOPPED);
    Power_Update();
}

//////////////////////////////////////////////////////////////////////////
//TESTS
//////////////////////////////////////////////////////////////////////////

static void test_Power_Update_NormalToNormal(void **state)
{
    will_return_always(__wrap_driverCharger_IsCharging, false);
    will_return_always(__wrap_driverCharger_GetBatteryVoltage, NORMAL_VOLTAGE);
    Power_Update();
    Power_Update();
}

static void test_Power_Update_NormalToCharging(void **state)
{
    will_return_always(__wrap_driverCharger_IsCharging, true);
    ExpectEvent(EVENT_BATTERY_CHARGING_STARTED);
    Power_Update();
}

static void test_Power_Update_NormalToLowBattery(void **state)
{
    will_return_always(__wrap_driverCharger_IsCharging, false);
    will_return_always(__wrap_driverCharger_GetBatteryVoltage, LOW_VOLTAGE);
    ExpectEvent(EVENT_BATTERY_LOW);
    Power_Update();
}

static void test_Power_Update_LowBatteryToLowBattery(void **state)
{
    EnterLowState();

    will_return_always(__wrap_driverCharger_IsCharging, false);
    will_return_always(__wrap_driverCharger_GetBatteryVoltage, LOW_VOLTAGE);
    Power_Update();
}

static void test_Power_Update_LowBatteryToCharging(void **state)
{
    EnterLowState();

    will_return_always(__wrap_driverCharger_IsCharging, true);
    ExpectEvent(EVENT_BATTERY_CHARGING_STARTED);
    Power_Update();
}

static void test_Power_Update_LowBatteryToCriticalBattery(void **state)
{
    EnterLowState();

    will_return_always(__wrap_driverCharger_IsCharging, false);
    will_return_always(__wrap_driverCharger_GetBatteryVoltage, CRITICAL_VOLTAGE);
    ExpectEvent(EVENT_BATTERY_CRITICAL);
    Power_Update();
}

static void test_Power_Update_CriticalBatteryToCriticalBattery(void **state)
{
    EnterCriticalState();

    will_return_always(__wrap_driverCharger_IsCharging, false);
    Power_Update();
}

static void test_Power_Update_CriticalBatteryToCharging(void **state)
{
    EnterCriticalState();

    will_return_always(__wrap_driverCharger_IsCharging, true);
    ExpectEvent(EVENT_BATTERY_CHARGING_STARTED);
    Power_Update();
}

static void test_Power_Update_ChargingToCharging(void **state)
{
    EnterChargingState();

    will_return_always(__wrap_driverCharger_IsCharging, true);
    Power_Update();
}

static void test_Power_Update_ChargingToConnected(void **state)
{
    EnterChargingState();

    will_return_always(__wrap_driverCharger_IsCharging, false);
    ExpectEvent(EVENT_BATTERY_CHARGING_STOPPED);
    Power_Update();
}

static void test_Power_Update_ConnectedToConnected(void **state)
{
    EnterConnectedState();

    will_return_always(__wrap_driverCharger_IsCharging, false);
    will_return_always(__wrap_driverCharger_IsConnected, true);
    Power_Update();
}

static void test_Power_Update_ConnectedToCharging(void **state)
{
    EnterConnectedState();

    will_return_always(__wrap_driverCharger_IsCharging, true);
    will_return_always(__wrap_driverCharger_IsConnected, true);
    ExpectEvent(EVENT_BATTERY_CHARGING_STARTED);
    Power_Update();
}

static void test_Power_Update_ConnectedToNormal(void **state)
{
    EnterConnectedState();

    will_return_always(__wrap_driverCharger_IsCharging, false);
    will_return_always(__wrap_driverCharger_IsConnected, false);
    ExpectEvent(EVENT_BATTERY_CHARGER_DISCONNECTED);
    Power_Update();
}

int main(void)
{
    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test_setup(test_Power_Update_NormalToNormal, Setup),
        cmocka_unit_test_setup(test_Power_Update_NormalToCharging, Setup),
        cmocka_unit_test_setup(test_Power_Update_NormalToLowBattery, Setup),
        cmocka_unit_test_setup(test_Power_Update_LowBatteryToLowBattery, Setup),
        cmocka_unit_test_setup(test_Power_Update_LowBatteryToCharging, Setup),
        cmocka_unit_test_setup(test_Power_Update_LowBatteryToCriticalBattery, Setup),
        cmocka_unit_test_setup(test_Power_Update_CriticalBatteryToCriticalBattery, Setup),
        cmocka_unit_test_setup(test_Power_Update_CriticalBatteryToCharging, Setup),
        cmocka_unit_test_setup(test_Power_Update_ChargingToCharging, Setup),
        cmocka_unit_test_setup(test_Power_Update_ChargingToConnected, Setup),
        cmocka_unit_test_setup(test_Power_Update_ConnectedToConnected, Setup),
        cmocka_unit_test_setup(test_Power_Update_ConnectedToCharging, Setup),
        cmocka_unit_test_setup(test_Power_Update_ConnectedToNormal, Setup)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
