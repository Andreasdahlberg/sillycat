/**
 * @file   guiNodes.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2020-03-11 (Last edit)
 * @brief  Implementation of GUI for remote nodes.
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
#include <stdio.h>
#include <stdlib.h>

#include "libDebug.h"
#include "libUI.h"

#include "Interface.h"
#include "Nodes.h"

#include "guiNodes.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define MAX_NR_NODE_VIEWS 3

#define BATT_INDICATOR_X                2
#define BATT_INDICATOR_Y                23
#define BATT_INDICATOR_ANIMATION_DELAY  400

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

struct node_view_t
{
    struct view overview;
    struct view details;
    struct view temperature;
    struct view humidity;
};

struct module_t
{
    struct node_view_t views[MAX_NR_NODE_VIEWS];
    struct
    {
        uint8_t nr_bars;
        uint32_t timer;
    } battery_indicator;
};

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static struct module_t module;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

static void DrawNodeView(uint16_t context);
static void DrawDetailedNodeView(uint16_t context);
static void DrawTemperatureMaxMinView(uint16_t context);
static void DrawHumidityMaxMinView(uint16_t context);
static void DrawBattery(uint8_t nr_bars);
static void DrawBatteryIndicator(const struct node_t *node_p);
static void ClearAction(uint16_t context __attribute__ ((unused)));
static uint8_t ContextToNodeID(uint16_t context);

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void guiNodes_Init(void)
{
    for (size_t i = 0; i < ElementsIn(module.views); ++i)
    {
        struct node_view_t *view_p = &module.views[i];

        Interface_InitView(&view_p->overview, DrawNodeView, i);
        Interface_AddView(&view_p->overview);

        Interface_InitView(&view_p->temperature, DrawTemperatureMaxMinView, i);
        Interface_AddAction(&view_p->temperature, ClearAction);
        Interface_AddChild(&view_p->overview, &view_p->temperature);

        Interface_InitView(&view_p->humidity, DrawHumidityMaxMinView, i);
        Interface_AddAction(&view_p->humidity, ClearAction);
        Interface_AddChild(&view_p->overview, &view_p->humidity);

        Interface_InitView(&view_p->details, DrawDetailedNodeView, i);
        Interface_AddChild(&view_p->overview, &view_p->details);
    }

    module.battery_indicator.nr_bars = 0;
    module.battery_indicator.timer = 0;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static void DrawNodeView(uint16_t context)
{
    /**
     * Add three to node index(context) so the indexing continues after the
     * two temperature sensors.
     */
    libUI_Print("%u", 1, 12, context + 3);

    struct node_t *node_p = Nodes_GetNodeFromID(ContextToNodeID(context));
    sc_assert(node_p != NULL);

    if (Node_IsActive(node_p))
    {
        const struct sensor_t *temperature_sensor_p = Node_GetTemperatureSensor(node_p);
        const struct sensor_t *humidity_sensor_p = Node_GetHumiditySensor(node_p);

        if (Sensor_IsValid(temperature_sensor_p) &&
                Sensor_IsValid(humidity_sensor_p))
        {
            int16_t temperature_scaled;
            int16_t humidity_scaled;

            Sensor_GetValue(temperature_sensor_p, &temperature_scaled);
            Sensor_GetValue(humidity_sensor_p, &humidity_scaled);

            struct div_t temperature;
            struct div_t humidity;

            temperature = Divide((int32_t)temperature_scaled, 10);
            humidity = Divide((int32_t)humidity_scaled, 10);

            libUI_Print(
                "%ld.+%d C",
                45,
                UI_DOUBLE_ROW_FIRST,
                temperature.quotient,
                abs(temperature.remainder)
            );
            libUI_Print(
                "%ld.+%d %%",
                45,
                UI_DOUBLE_ROW_SECOND,
                humidity.quotient,
                abs(humidity.remainder)
            );
        }
        else
        {
            libUI_Print("--.-- C", 40, UI_DOUBLE_ROW_FIRST);
            libUI_Print("--.-- %%", 40, UI_DOUBLE_ROW_SECOND);
        }

        DrawBatteryIndicator(node_p);
    }
    else
    {
        libUI_Print("--.-- C", 40, UI_DOUBLE_ROW_FIRST);
        libUI_Print("--.-- %%", 40, UI_DOUBLE_ROW_SECOND);
    }
}

static void DrawDetailedNodeView(uint16_t context)
{
    const struct node_t *node_p = Nodes_GetNodeFromID(ContextToNodeID(context));

    if (node_p != NULL && Node_IsActive(node_p))
    {
        uint16_t voltage = Node_GetBatteryVoltage(node_p);
        int16_t rssi = Node_GetRSSI(node_p);

        libUI_Print("Batt: %hu mV", 2, UI_DOUBLE_ROW_FIRST, voltage);
        libUI_Print("RSSI: %i dBm", 2, UI_DOUBLE_ROW_SECOND, rssi);
    }
    else
    {
        libUI_Print("Batt: -- mV", 2, UI_DOUBLE_ROW_FIRST);
        libUI_Print("RSSI: -- dBm", 2, UI_DOUBLE_ROW_SECOND);
    }
}

static void DrawTemperatureMaxMinView(uint16_t context)
{
    struct node_t *node_p = Nodes_GetNodeFromID(ContextToNodeID(context));
    sc_assert(node_p != NULL);

    const struct sensor_t *temperature_sensor_p = Node_GetTemperatureSensor(node_p);

    if (Sensor_IsStatisticsValid(temperature_sensor_p))
    {
        int16_t temperature_scaled;
        struct div_t temperature;

        Sensor_GetMaxValue(temperature_sensor_p, &temperature_scaled);
        temperature = Divide((int32_t)temperature_scaled, 10);
        libUI_Print(
            "Max: %ld.%d C",
            2,
            UI_DOUBLE_ROW_FIRST,
            temperature.quotient,
            abs(temperature.remainder));

        Sensor_GetMinValue(temperature_sensor_p, &temperature_scaled);
        temperature = Divide((int32_t)temperature_scaled, 10);
        libUI_Print(
            "Min: %ld.%d C",
            2,
            UI_DOUBLE_ROW_SECOND,
            temperature.quotient,
            abs(temperature.remainder));
    }
    else
    {
        libUI_Print("Max: --.-- C", 2, UI_DOUBLE_ROW_FIRST);
        libUI_Print("Min: --.-- C", 2, UI_DOUBLE_ROW_SECOND);
    }
}

static void DrawHumidityMaxMinView(uint16_t context)
{
    struct node_t *node_p = Nodes_GetNodeFromID(ContextToNodeID(context));
    sc_assert(node_p != NULL);

    const struct sensor_t *humidity_sensor_p = Node_GetHumiditySensor(node_p);

    if (Sensor_IsStatisticsValid(humidity_sensor_p))
    {
        int16_t humidity_scaled;
        struct div_t humidity;

        Sensor_GetMaxValue(humidity_sensor_p, &humidity_scaled);
        humidity = Divide((int32_t)humidity_scaled, 10);
        libUI_Print(
            "Max: %ld.%d %%",
            2, UI_DOUBLE_ROW_FIRST,
            humidity.quotient,
            abs(humidity.remainder));

        Sensor_GetMinValue(humidity_sensor_p, &humidity_scaled);
        humidity = Divide((int32_t)humidity_scaled, 10);
        libUI_Print(
            "Min: %ld.%d %%",
            2,
            UI_DOUBLE_ROW_SECOND,
            humidity.quotient,
            abs(humidity.remainder));
    }
    else
    {
        libUI_Print("Max: --.-- %%", 2, UI_DOUBLE_ROW_FIRST);
        libUI_Print("Min: --.-- %%", 2, UI_DOUBLE_ROW_SECOND);
    }
}

static void DrawBattery(uint8_t nr_bars)
{
    libUI_DrawRectangle(BATT_INDICATOR_X, BATT_INDICATOR_Y, 8, 5);
    libUI_DrawLine(BATT_INDICATOR_X + 9,
                   BATT_INDICATOR_Y + 1,
                   BATT_INDICATOR_X + 9,
                   BATT_INDICATOR_Y + 4);

    for (size_t i = 0; i < nr_bars; ++i)
    {
        libUI_DrawLine(BATT_INDICATOR_X + 2 + i,
                       BATT_INDICATOR_Y + 2,
                       BATT_INDICATOR_X + 2 + i,
                       BATT_INDICATOR_Y + 3);
    }
}

static void DrawBatteryIndicator(const struct node_t *node_p)
{
    if (Node_IsBatteryCharging(node_p))
    {
        if (Timer_TimeDifference(module.battery_indicator.timer) >
                BATT_INDICATOR_ANIMATION_DELAY)
        {
            ++module.battery_indicator.nr_bars;
            module.battery_indicator.timer = Timer_GetMilliseconds();
        }
        DrawBattery(module.battery_indicator.nr_bars % 6);
    }
    else if (Node_IsBatteryChargerConnected(node_p))
    {
        DrawBattery(5);
    }
    else if (!Node_IsBatteryOk(node_p))
    {
        DrawBattery(0);
    }
}

static void ClearAction(uint16_t context __attribute__ ((unused)))
{
    struct node_t *node_p = Nodes_GetNodeFromID(ContextToNodeID(context));

    Sensor_Reset(Node_GetTemperatureSensor(node_p));
    Sensor_Reset(Node_GetHumiditySensor(node_p));

    Interface_Refresh();

    return;
}

static uint8_t ContextToNodeID(uint16_t context)
{
    return (uint8_t)context + 128;
}
