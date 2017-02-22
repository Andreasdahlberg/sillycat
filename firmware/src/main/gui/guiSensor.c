/**
 * @file   guiSensor.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2017-02-22 (Last edit)
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

    Interface_AddChild(&temperature_view, &detailed_temperature_view);
    Interface_AddView(&temperature_view);
    return;
}

void guiSensor_DrawDetailedTemperatureView(uint16_t context __attribute__ ((
            unused)))
{
    sensor_sample_type reading;

    Sensor_GetReading(SENSOR_EXTERNAL_TEMPERATURE, &reading);

    libUI_Print("Max: %u C", 2, UI_DOUBLE_ROW_FIRST, reading.max / 10);
    libUI_Print("Min: %u C", 2, UI_DOUBLE_ROW_SECOND, reading.min / 10);
    return;
}

void guiSensor_DrawTemperatureView(uint16_t context __attribute__ ((unused)))
{
    sensor_sample_type reading;

    if (Sensor_GetReading(SENSOR_EXTERNAL_TEMPERATURE, &reading))
    {
        uint8_t temperature_integer = (uint8_t)(reading.value / 10);

        libUI_Print("%u.%u C", 45, UI_SINGLE_ROW, temperature_integer,
                    reading.value % 10);
    }
    else
    {
        libUI_Print("Temperature: -.- C", 1, UI_SINGLE_ROW);
    }
    return;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////
