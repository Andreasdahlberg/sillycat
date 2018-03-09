/**
 * @file   Sensor.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-03-09 (Last edit)
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
#include "LED.h"
#include "Transceiver.h"
#include "FIFO.h"
#include "ErrorHandler.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define READING_BUFFER_SIZE 4

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static dht22_data_type reading_buffer[READING_BUFFER_SIZE];
static fifo_type reading_fifo;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

///
/// @brief Init the sensor module.
///
/// @param  None
/// @return None
///
void Sensor_Init(void)
{
    reading_fifo = FIFO_New(reading_buffer);

    INFO("Init done");
}

///
/// @brief Update the internal state of the sensor module.
///
/// @param  None
/// @return None
///
void Sensor_Update(void)
{
    if (libDHT22_IsReadingValid())
    {
        dht22_data_type dht22_reading;
        dht22_reading = libDHT22_GetSensorReading();

        // No need to check the reading status since it is already done in
        // the call to libDHT22_IsReadingValid().

        sensor_data_type reading;
        reading.temperature = dht22_reading.temperature;
        reading.humidity = dht22_reading.humidity;
        if (!RTC_GetCurrentTime(&reading.timestamp))
        {
            // Log error and continue, an invalid timestamp is not critical.
            ErrorHandler_LogError(RTC_FAIL, 0);

            ERROR("Failed to get timestamp.");
        }

        // Discard the oldest reading if the buffer is full.
        if (FIFO_IsFull(&reading_fifo))
        {
            sensor_data_type dummy_reading;
            FIFO_Pop(&reading_fifo, &dummy_reading);

            WARNING("FIFO is full, discarding oldest entry.");
        }
        FIFO_Push(&reading_fifo, &reading);

        event_type event;
        event = Event_New(EVENT_RHT_AVAILABLE);
        Event_Trigger(&event);
    }
    return;
}

///
/// @brief Get a sensor reading from the reading FIFO.
///
/// @param  *reading Pointer to struct where the reading will be stored.
/// @return bool True if a reading is available.
///
bool Sensor_GetReading(sensor_data_type *reading)
{
    sc_assert(reading != NULL);

    return FIFO_Pop(&reading_fifo, reading);
}

///
/// @brief Handle events
///
/// @param  *event Pointer to triggered event
/// @return None
///
void Sensor_WakeUp(const event_type *event __attribute__ ((unused)))
{
    sc_assert(event != NULL);

    libDHT22_StartReading();
    return;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////
