/**
 * @file   guiSensor.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2017-08-26 (Last edit)
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

#include <stdlib.h>

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

void ClearAction(uint16_t context __attribute__ ((unused)));

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void guiSensor_Init(void)
{
    temperature_view.draw_function = guiSensor_DrawTemperatureView;
    temperature_view.action_function = NULL;
    temperature_view.child = NULL;
    temperature_view.prev = NULL;
    temperature_view.next = NULL;
    temperature_view.parent = NULL;

    detailed_temperature_view.draw_function = guiSensor_DrawDetailedTemperatureView;
    detailed_temperature_view.action_function = ClearAction;
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
    if (Sensor_GetReading(SENSOR_EXTERNAL_TEMPERATURE, &reading))
    {
        struct div_t max_temperature;
        struct div_t min_temperature;
        max_temperature = Divide((int32_t)reading.max, 10);
        min_temperature = Divide((int32_t)reading.min, 10);

        libUI_Print("Max: %ld.%d C", 2, UI_DOUBLE_ROW_FIRST,
                    max_temperature.quotient, abs(max_temperature.remainder));
        libUI_Print("Min: %ld.%d C", 2, UI_DOUBLE_ROW_SECOND,
                    min_temperature.quotient, abs(min_temperature.remainder));
    }
    else
    {
        libUI_Print("Max: -.- C", 2, UI_DOUBLE_ROW_FIRST);
        libUI_Print("Min: -.- C", 2, UI_DOUBLE_ROW_SECOND);
    }
    return;
}

void guiSensor_DrawTemperatureView(uint16_t context __attribute__ ((unused)))
{
    sensor_sample_type reading;

    if (Sensor_GetReading(SENSOR_EXTERNAL_TEMPERATURE, &reading))
    {
        struct div_t temperature;
        temperature = Divide((int32_t)reading.value, 10);

        libUI_Print("%ld.%d C", 45, UI_SINGLE_ROW, temperature.quotient,
                    abs(temperature.remainder));
    }
    else
    {
        libUI_Print("-.- C", 45, UI_SINGLE_ROW);
    }
    return;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void ClearAction(uint16_t context __attribute__ ((unused)))
{
    Sensor_ClearReadings();
    Interface_Refresh();

    return;
}
