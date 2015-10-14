/**
 * @file   guiSensor.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2015-10-14 (Last edit)
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

#include <string.h>

#include "common.h"
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
    
    Interface_AddView(&temperature_view);
    Interface_AddView(&detailed_temperature_view);    
    
    return;
}

void guiSensor_DrawDetailedTemperatureView(void)
{
    sensor_sample_type reading;
    char text[16];
    
    Sensor_GetReading(SENSOR_EXTERNAL_TEMPERATURE, &reading);
    
    sprintf(text, "max: %uC", reading.max/10);
    libUI_PrintText(text, 6, 2);
    
    sprintf(text, "min: %uC", reading.min/10);
    libUI_PrintText(text, 70, 2);
    
    sprintf(text, "now: %uC", reading.value/10);
    libUI_PrintText(text, 6, 16);
    
    sprintf(text, "avg: %uC", reading.average/10);
    libUI_PrintText(text, 70, 16);
    return;
}

void guiSensor_DrawTemperatureView(void)
{
    sensor_sample_type reading;
    char text[20];
    
    if (Sensor_GetReading(SENSOR_EXTERNAL_TEMPERATURE, &reading))
    {
        sprintf(text, "Temperature: %uC", reading.value/10);
        libUI_PrintText(text, 10,10);
    }
    else
    {
        libUI_PrintText("Temperature: --", 10,10);
    }
    
    return;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////