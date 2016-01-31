/**
 * @file   node_firmware.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2016-01-25 (Last edit)
 * @brief  Implementation of main
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

#include <avr/io.h>
#include <avr/wdt.h>

#include "libDebug.h"
#include "libADC.h"
#include "libSPI.h"
#include "libRFM69.h"
#include "libDHT22.h"
#include "libPower.h"

#include "Timer.h"
#include "LED.h"
#include "Sensor.h"
#include "Transceiver.h"
#include "ErrorHandler.h"
#include "Config.h"
#include "RTC.h"
#include "Power.h"
#include "Event.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define MAX_AWAKE_TIME_MS 3000

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

typedef struct
{
    uint32_t last_sleep_time;
    bool sleep_now;
} sleep_status_type;

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static sleep_status_type sleep_status = {0};

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

static void NotifyAndEnterSleep(void);
static bool IsTimeForSleep(void);
static void RHTSent(const event_type *event __attribute__ ((unused)));

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////


int main(void)
{
    uint8_t mcu_status = MCUSR;
    MCUSR = 0;
    wdt_disable();

    //Init hardware early to ensure all CS are disabled.
    libRFM69_HWInit();
    RTC_InitHW();

    libDebug_Init();
    INFO("Node unit started");
    INFO("Last reset: 0x%02X", mcu_status);

    libADC_Init();
    Timer_Init();
    libSPI_Init(1);
    libADC_Enable(TRUE);
    libDHT22_Init();
    libPower_Init();

    LED_Init();
    Config_Load();
    RTC_Init();
    ErrorHandler_Init();

    //NOTE: Log reset reasons, during normal conditions the device should never restart.
    //      Disabled during development to prevent filling the error log.
#ifndef DEBUG_ENABLE
    ErrorHandler_LogError(POWERON, mcu_status);
#endif

    Sensor_Init();
    Transceiver_Init();
    Power_Init();

    Event_AddListener(Sensor_WakeUp, EVENT_WAKEUP);
    Event_AddListener(LED_EventHandler, EVENT_ALL);
    //Event_AddListener(Transceiver_EventHandler, EVENT_ALL);
    Event_AddListener(RHTSent, EVENT_RHT_SENT);

    //Since RTC-alarms are persistent between restarts so we need to make
    //sure that they are disabled.
    RTC_EnableAlarm(FALSE);

#ifdef DEBUG_ENABLE
    //Add debug listener last to ensure all debug prints are flushed
    //before sleep.
    Event_AddListener(libDebug_Sleep, EVENT_SLEEP);
#endif

    INFO("Start up done");

    sleep_status.last_sleep_time = Timer_GetMilliseconds();

    while (1)
    {
        libADC_Update();
        libDHT22_Update();
        Sensor_Update();
        Transceiver_Update();
        LED_Update();
        Power_Update();

        if (IsTimeForSleep() == TRUE)
        {
            NotifyAndEnterSleep();
        }

    }
    sc_assert_fail();
    SoftReset();
}

static void RHTSent(const event_type *event __attribute__ ((unused)))
{
    sleep_status.sleep_now = TRUE;
    return;
}

static bool IsTimeForSleep(void)
{
    if (libPower_IsChargerConnected() == TRUE)
    {
        return FALSE;
    }

    return (sleep_status.sleep_now == TRUE ||
            Timer_TimeDifference(sleep_status.last_sleep_time) > MAX_AWAKE_TIME_MS);
}

static void NotifyAndEnterSleep(void)
{
    rtc_time_type time;
    event_type event = {0};

    RTC_GetCurrentTime(&time);
    DEBUG("Sleep: %u:%u:%u\r\n", time.hour, time.minute, time.second);

    //TODO: Compensate for time awake
    RTC_AddSeconds(&time, Config_GetReportInterval());
    RTC_SetAlarmTime(&time);
    RTC_EnableAlarm(TRUE);

    event = Event_New(EVENT_SLEEP);
    Event_Trigger(&event);

    //Enter sleep mode, execution will continue from this point
    //after sleep is done.
    libPower_Sleep();
    RTC_ClearAlarm();
    RTC_EnableAlarm(FALSE);

    sleep_status.last_sleep_time = Timer_GetMilliseconds();
    sleep_status.sleep_now = FALSE;

    event = Event_New(EVENT_WAKEUP);
    Event_Trigger(&event);

    RTC_GetCurrentTime(&time);
    DEBUG("Wake: %u:%u:%u\r\n", time.hour, time.minute, time.second);

    return;
}