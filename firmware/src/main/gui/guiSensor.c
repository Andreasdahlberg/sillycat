/**
 * @file   guiSensor.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2020-03-19 (Last edit)
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

#define NR_SENSOR_VIEWS 2

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

struct sensor_view_t
{
    struct view overview;
    struct view details;
    struct sensor_t *sensor_p;
};

struct module_t
{
    struct sensor_view_t views[NR_SENSOR_VIEWS];
};

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static struct module_t module;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

void DrawDetailsView(uint16_t context);
void DrawOverviewView(uint16_t context);
void ClearAction(uint16_t context);
struct sensor_view_t *GetViewPointerFromContext(uint16_t context);

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void guiSensor_Init(void)
{
    for  (size_t i = 0; i < ElementsIn(module.views); ++i)
    {
        struct sensor_view_t *view_p = GetViewPointerFromContext(i);

        Interface_InitView(&view_p->overview, DrawOverviewView, i);
        Interface_AddView(&view_p->overview);

        Interface_InitView(&view_p->details, DrawDetailsView, i);
        Interface_AddAction(&view_p->details, ClearAction);
        Interface_AddChild(&view_p->overview, &view_p->details);

        view_p->sensor_p = driverNTC_GetSensor(i);
    }
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void DrawDetailsView(uint16_t context)
{
    const struct sensor_view_t *view_p = GetViewPointerFromContext(context);

    int16_t max_scaled;
    int16_t min_scaled;

    if (Sensor_GetMaxValue(view_p->sensor_p, &max_scaled) &&
            Sensor_GetMinValue(view_p->sensor_p, &min_scaled))
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
}

void DrawOverviewView(uint16_t context)
{
    /**
     * Add one to index(context) since end users are more familiar with
     * indexing starting at 1.
     */
    libUI_Print("%u", 1, 12, context + 1);

    const struct sensor_view_t *view_p = GetViewPointerFromContext(context);
    int16_t temperature_scaled;

    if (Sensor_GetValue(view_p->sensor_p, &temperature_scaled))
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
}

void ClearAction(uint16_t context)
{
    const struct sensor_view_t *view_p = GetViewPointerFromContext(context);

    Sensor_Reset(view_p->sensor_p);
    Interface_Refresh();
}

struct sensor_view_t *GetViewPointerFromContext(uint16_t context)
{
    sc_assert(context < ElementsIn(module.views));

    return &module.views[context];
}
