/**
 * @file   guiSensor.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-09-01 (Last edit)
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
#include "driverNTC.h"
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
static struct sensor_t *temperature_sensor;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

void ClearAction(uint16_t context __attribute__ ((unused)));

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void guiSensor_Init(void)
{
    Interface_InitView(&temperature_view, guiSensor_DrawTemperatureView, 0);
    Interface_AddView(&temperature_view);

    Interface_InitView(&detailed_temperature_view, guiSensor_DrawDetailedTemperatureView, 0);
    Interface_AddAction(&detailed_temperature_view, ClearAction);
    Interface_AddChild(&temperature_view, &detailed_temperature_view);

    temperature_sensor = driverNTC_GetSensor(0);
    return;
}

void guiSensor_DrawDetailedTemperatureView(uint16_t context __attribute__ ((
            unused)))
{
    int16_t max_scaled;
    int16_t min_scaled;

    if (Sensor_GetMaxValue(temperature_sensor, &max_scaled) &&
            Sensor_GetMinValue(temperature_sensor, &min_scaled))
    {
        struct div_t max;
        struct div_t min;

        max = Divide((int32_t)max_scaled, 10);
        min = Divide((int32_t)min_scaled, 10);

        libUI_Print("Max: %ld.%d C", 2, UI_DOUBLE_ROW_FIRST,
                    max.quotient, abs(max.remainder));
        libUI_Print("Min: %ld.%d C", 2, UI_DOUBLE_ROW_SECOND,
                    min.quotient, abs(min.remainder));
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
    int16_t temperature_scaled;

    if (Sensor_GetValue(temperature_sensor, &temperature_scaled))
    {
        struct div_t temperature;
        temperature = Divide((int32_t)temperature_scaled, 10);

        libUI_Print("%ld.%d C", 45, UI_SINGLE_ROW,
                    temperature.quotient, abs(temperature.remainder));
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
    Sensor_Reset(temperature_sensor);
    Interface_Refresh();

    return;
}
