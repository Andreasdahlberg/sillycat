/**
 * @file   Sensor.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2015-12-13 (Last edit)
 * @brief  Implementation of Sensor module
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

#include <string.h>

#include "libDebug.h"
#include "libDHT22.h"

#include "Sensor.h"
#include "Timer.h"
#include "RTC.h"
#include "LED.h"
#include "Transceiver.h"

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

static void SendCallback(bool status);

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void Sensor_Init(void)
{
    INFO("Init done");
}

void Sensor_Update(void)
{
    if (libDHT22_IsReadingValid())
    {
        dht22_data_type sample;
        packet_content_type sample_packet;

        sample = libDHT22_GetSensorReading();
        INFO("Temperature: %uC", (uint32_t)sample.temperature);
        INFO("Humidity: %u%%", (uint32_t)sample.humidity);

        rtc_time_type timestamp;
        RTC_GetCurrentTime(&timestamp);

        sample_packet.type = TR_PACKET_TYPE_READING;
        sample_packet.size = sizeof(dht22_data_type) + 1;
        sample_packet.timestamp = timestamp;
        memcpy(sample_packet.data, (uint8_t *)&sample, sizeof(dht22_data_type));

        LED_ChangeState(LED_STATE_SENDING);
        Transceiver_SendPacket(0xAA, FALSE, &sample_packet, SendCallback);
    }
}

///
/// @brief Handle events
///
/// @param  *event Pointer to triggered event
/// @return None
///
void Sensor_EventHandler(const event_type *event)
{
    sc_assert(event != NULL);

    switch (event->id)
    {
        case EVENT_SLEEP:
            INFO("Entering sleep");
            break;

        case EVENT_WAKEUP:
            INFO("Exiting sleep");
            libDHT22_StartReading();

            break;

        default:
            //Do nothing if an unknown event is received
            break;
    }
    return;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static void SendCallback(bool status)
{
    INFO("Sensor callback: %u\r\n", (uint8_t)status);
}
