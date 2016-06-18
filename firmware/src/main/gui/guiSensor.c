/**
 * @file   guiSensor.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2016-05-16 (Last edit)
 * @brief  Implementation of guiSensor
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

//NOTE: Include common.h before all other headers
#include "common.h"

#include <string.h>
#include <stdio.h>

#include "libDebug.h"
#include "libUI.h"

#include "Sensor.h"
#include "Interface.h"

#include "guiSensor.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static struct view detailed_temperature_view;
static struct view temperature_view;
static struct view sensor_information_view;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void guiSensor_Init(void)
{
    temperature_view.draw_function = guiSensor_DrawTemperatureView;
    temperature_view.child = NULL;
    temperature_view.prev = NULL;
    temperature_view.next = NULL;
    temperature_view.parent = NULL;

    detailed_temperature_view.draw_function = guiSensor_DrawDetailedTemperatureView;
    detailed_temperature_view.child = NULL;
    detailed_temperature_view.prev = NULL;
    detailed_temperature_view.next = NULL;
    detailed_temperature_view.parent = NULL;

    sensor_information_view.draw_function = guiSensor_DrawSensorInformation;
    sensor_information_view.child = NULL;
    sensor_information_view.prev = NULL;
    sensor_information_view.next = NULL;
    sensor_information_view.parent = NULL;

    Interface_AddChild(&temperature_view, &detailed_temperature_view);
    Interface_AddChild(&temperature_view, &sensor_information_view);
    Interface_AddView(&temperature_view);
    return;
}

void guiSensor_DrawSensorInformation(uint16_t context __attribute__ ((unused)))
{
    libUI_Print("Avg. period: %u min", 6, 2, (AVERAGE_WINDOW_S / 60));
    libUI_Print("Sample time: %u sec", 6, 16, (SAMPLE_FREQUENCY_MS / 1000));
    return;
}

void guiSensor_DrawDetailedTemperatureView(uint16_t context __attribute__ ((
            unused)))
{
    sensor_sample_type reading;

    Sensor_GetReading(SENSOR_EXTERNAL_TEMPERATURE, &reading);

    libUI_Print("max: %uC", 6, 2, reading.max / 10);
    libUI_Print("min: %uC", 70, 2, reading.min / 10);
    libUI_Print("now: %uC", 6, 16, reading.value / 10);
    libUI_Print("avg: %uC", 70, 16, reading.average / 10);
    return;
}

void guiSensor_DrawTemperatureView(uint16_t context __attribute__ ((unused)))
{
    sensor_sample_type reading;

    if (Sensor_GetReading(SENSOR_EXTERNAL_TEMPERATURE, &reading))
    {
        uint8_t temperature_integer = (uint8_t)(reading.value / 10);

        libUI_Print("Temperature: %u.%uC", 6, 10, temperature_integer,
                    reading.value % 10);
    }
    else
    {
        libUI_Print("Temperature: --", 6, 10);
    }
    return;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////