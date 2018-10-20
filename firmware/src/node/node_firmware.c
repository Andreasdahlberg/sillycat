/**
 * @file   node_firmware.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-10-20 (Last edit)
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
#include <string.h>

#include "libDebug.h"
#include "libSPI.h"
#include "libRFM69.h"
#include "libDHT22.h"
#include "libPower.h"
#include "libS25FL1K.h"
#include "driverCharger.h"

#include "ADC.h"
#include "Timer.h"
#include "LED.h"
#include "Sensor.h"
#include "Transceiver.h"
#include "ErrorHandler.h"
#include "Config.h"
#include "RTC.h"
#include "Power.h"
#include "Event.h"
#include "Com.h"
#include "Packet.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define MAX_AWAKE_TIME_MS 1000

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
static void RHTAvailable(const event_t *event __attribute__ ((unused)));
static void CriticalBatteryVoltageHandler(const event_t *event __attribute__ ((unused)));
static bool TimePacketHandler(packet_frame_type *packet);
static void FillPacket(struct packet_t *packet_p);

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

int main(void)
{
    uint8_t mcu_status = MCUSR;
    MCUSR = 0;
    wdt_disable();

    //Init hardware early to ensure all CS are disabled.
    libRFM69_InitHW();
    libS25FL1K_InitHW();
    RTC_InitHW();
    libDebug_Init();

    INFO("Node unit started");
    INFO("Last reset: 0x%02X", mcu_status);

    Timer_Init();
    ADC_Init();
    libSPI_Init(0);
    libDHT22_Init();
    libPower_Init();
    driverCharger_Init();

    LED_Init();
    RTC_Init();
    ErrorHandler_Init();

    if (!Config_Load())
    {
        CRITICAL("Corrupt device configuration");
        ErrorHandler_LogError(CORRUPT_CONFIG, 0);
        ErrorHandler_PointOfNoReturn();
    }

    //NOTE: Log reset reasons, during normal conditions the device should never restart.
    //      Disabled during development to prevent filling the error log.
#ifndef DEBUG_ENABLE
    ErrorHandler_LogError(POWERON, mcu_status);
#endif

    Sensor_Init();
    Transceiver_Init();
    Com_Init();
    Power_Init();

    // The flash memory is not used so it's put into deep power down mode
    // to reduce current usage.
    libS25FL1K_EnterDeepPowerDown();

    Com_SetPacketHandler(TimePacketHandler, COM_PACKET_TYPE_TIME);

#ifdef DEBUG_ENABLE
    //IMPORTENT: The debug wakeup must be called first to enable debug prints
    //           in the other wakeup functions.
    Event_AddListener(libDebug_WakeUp, EVENT_WAKEUP);
#endif
    Event_AddListener(Sensor_WakeUp, EVENT_WAKEUP);
    Event_AddListener(Sensor_Sleep, EVENT_SLEEP);
    Event_AddListener(Power_WakeUp, EVENT_WAKEUP);
    Event_AddListener(LED_EventHandler, EVENT_ALL);
    Event_AddListener(Transceiver_EventHandler, EVENT_ALL);
    Event_AddListener(RHTAvailable, EVENT_RHT_AVAILABLE);
    Event_AddListener(CriticalBatteryVoltageHandler, EVENT_BATTERY_CRITICAL);

    //Since RTC-alarms are persistent between restarts we need to make
    //sure that they are disabled.
    RTC_EnableAlarm(false);

#ifdef DEBUG_ENABLE
    //Add debug listener last to ensure all debug prints are flushed
    //before sleep.
    Event_AddListener(libDebug_Sleep, EVENT_SLEEP);
#endif

    INFO("Start up done");
    sleep_status.last_sleep_time = Timer_GetMilliseconds();

    while (1)
    {
        driverCharger_Update();
        Power_Update();
        libDHT22_Update();
        Sensor_Update();
        Transceiver_Update();
        Com_Update();
        LED_Update();

        if (IsTimeForSleep() == true)
        {
            DEBUG("Unused stack: %u\r\n", StackCount());
            sc_assert(StackCount() > 0);
            NotifyAndEnterSleep();
        }
    }
    sc_assert_fail();
    SoftReset();
}

static bool TimePacketHandler(packet_frame_type *packet)
{
    sc_assert(packet != NULL);

    bool status = true;

    rtc_time_type current_time;
    RTC_GetCurrentTime(&current_time);

    uint32_t current_timestamp;
    current_timestamp = RTC_ConvertToTimestamp(&current_time);

    uint32_t received_timestamp;
    received_timestamp = RTC_ConvertToTimestamp(&packet->content.timestamp);

    if (current_timestamp != received_timestamp)
    {
        status = RTC_SetCurrentTime(&packet->content.timestamp);
        INFO("New time[%u]: %lu", (uint8_t)status, received_timestamp);
    }

    sleep_status.sleep_now = true;

    return status;
}

static void FillPacket(struct packet_t *packet_p)
{
    packet_p->battery.voltage = driverCharger_GetBatteryVoltage();
    packet_p->battery.temperature = driverCharger_GetBatteryTemperature();
    packet_p->battery.charging = driverCharger_IsCharging();
    packet_p->battery.connected = driverCharger_IsConnected();

    sensor_data_type reading;
    if (Sensor_GetReading(&reading))
    {
        packet_p->sensor.temperature = reading.temperature;
        packet_p->sensor.humidity = reading.humidity;
        packet_p->sensor.valid = true;
    }
    else
    {
        packet_p->sensor.valid = false;

        WARNING("Sending packet with no valid sensor data");
    }

    /**
     * Use a temporary variable since the packet struct is packed and thus
     * are pointers to the members not allowed.
     */
    rtc_time_type timestamp;
    RTC_GetCurrentTime(&timestamp);
    packet_p->timestamp = timestamp;
}

static void RHTAvailable(const event_t *event __attribute__ ((unused)))
{
    struct packet_t packet;

    FillPacket(&packet);
    Com_Send(Config_GetMasterAddress(),
             COM_PACKET_TYPE_READING, &packet, sizeof(packet));
}

static void CriticalBatteryVoltageHandler(const event_t *event __attribute__ ((unused)))
{
    event_t sleep_event = Event_New(EVENT_SLEEP);
    Event_Trigger(&sleep_event);

    /**
     * Enter sleep mode, execution will continue from this point
     * after sleep is done.
     */
    libPower_Sleep();

    sleep_event = Event_New(EVENT_WAKEUP);
    Event_Trigger(&sleep_event);
}

static bool IsTimeForSleep(void)
{
    return (sleep_status.sleep_now == true ||
            Timer_TimeDifference(sleep_status.last_sleep_time) > MAX_AWAKE_TIME_MS);
}

static void NotifyAndEnterSleep(void)
{
    rtc_time_type time;

    RTC_GetCurrentTime(&time);
    INFO("Sleep: %u:%u:%u", time.hour, time.minute, time.second);

    //TODO: Compensate for time awake
    RTC_AddSeconds(&time, Config_GetReportInterval());
    RTC_SetAlarmTime(&time);
    RTC_EnableAlarm(true);

    event_t event = Event_New(EVENT_SLEEP);
    Event_Trigger(&event);

    //Enter sleep mode, execution will continue from this point
    //after sleep is done.
    libPower_Sleep();
    RTC_EnableAlarm(false);
    RTC_ClearAlarm();

    sleep_status.last_sleep_time = Timer_GetMilliseconds();
    sleep_status.sleep_now = false;

    event = Event_New(EVENT_WAKEUP);
    Event_Trigger(&event);

    RTC_GetCurrentTime(&time);
    INFO("Wake: %u:%u:%u", time.hour, time.minute, time.second);

    return;
}
