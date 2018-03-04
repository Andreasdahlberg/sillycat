/**
 * @file   guiNodes.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-03-04 (Last edit)
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

#include "libDebug.h"
#include "libUI.h"

#include "Interface.h"
#include "Nodes.h"

#include "guiNodes.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define MAX_NR_NODE_VIEWS 3

#define BATT_INDICATOR_X 1
#define BATT_INDICATOR_Y 26

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
static void DrawBatteryIndicator(void);
static void ClearAction(uint16_t context __attribute__ ((unused)));

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
        view_p->temperature.action_function = ClearAction;
        Interface_AddChild(&view_p->overview, &view_p->temperature);

        Interface_InitView(&view_p->humidity, DrawHumidityMaxMinView, i);
        view_p->humidity.action_function = ClearAction;
        Interface_AddChild(&view_p->overview, &view_p->humidity);

        Interface_InitView(&view_p->details, DrawDetailedNodeView, i);
        Interface_AddChild(&view_p->overview, &view_p->details);
    }

    return;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static void DrawNodeView(uint16_t context)
{
    //Add one to node index(context) since end users are more familiar
    //with indexing starting at 1.
    libUI_Print("%u", 1, 12, context + 1);

    struct node_t *node_p = Nodes_GetNodeFromID((uint8_t)context+128);

    if (node_p != NULL && Node_IsActive(node_p))
    {
        struct sensor_t *temperature_sensor_p = Node_GetTemperatureSensor(node_p);
        struct sensor_t *humidity_sensor_p = Node_GetHumiditySensor(node_p);

        int16_t temperature;
        int16_t humidity;

        Sensor_GetValue(temperature_sensor_p, &temperature);
        Sensor_GetValue(humidity_sensor_p, &humidity);

        libUI_Print("%li.%u C", 45, UI_DOUBLE_ROW_FIRST, temperature / 10, 0);
        libUI_Print("%li.%u %%", 45, UI_DOUBLE_ROW_SECOND, humidity / 10, 0);

        if (!Node_IsBatteryOk(node_p))
        {
            DrawBatteryIndicator();
        }
    }
    else
    {
        libUI_Print("--.-- C", 40, UI_DOUBLE_ROW_FIRST);
        libUI_Print("--.-- %%", 40, UI_DOUBLE_ROW_SECOND);
    }
}

static void DrawDetailedNodeView(uint16_t context)
{
    struct node_t *node_p = Nodes_GetNodeFromID((uint8_t)context+128);

    if (node_p != NULL && Node_IsActive(node_p))
    {
        uint16_t voltage = Node_GetBatteryVoltage(node_p);
        int16_t rssi = Node_GetRSSI(node_p);

        libUI_Print("%hu mV", 40, UI_DOUBLE_ROW_FIRST, voltage);
        libUI_Print("%i dBm", 40, UI_DOUBLE_ROW_SECOND, rssi);
    }
    else
    {
        libUI_Print("-- mV", 40, UI_DOUBLE_ROW_FIRST);
        libUI_Print("-- dBm", 40, UI_DOUBLE_ROW_SECOND);
    }
}

static void DrawTemperatureMaxMinView(uint16_t context)
{
    struct node_t *node_p = Nodes_GetNodeFromID((uint8_t)context+128);

    if (node_p != NULL && Node_IsActive(node_p))
    {
        struct sensor_t *temperature_sensor_p = Node_GetTemperatureSensor(node_p);
        int16_t temperature;

        Sensor_GetMaxValue(temperature_sensor_p, &temperature);
        libUI_Print("Max: %li.%u C", 2, UI_DOUBLE_ROW_FIRST, temperature / 10, 0);

        Sensor_GetMinValue(temperature_sensor_p, &temperature);
        libUI_Print("Min: %li.%u C", 2, UI_DOUBLE_ROW_SECOND, temperature / 10, 0);
    }
    else
    {
        libUI_Print("Max: --.-- C", 2, UI_DOUBLE_ROW_FIRST);
        libUI_Print("Min: --.-- C", 2, UI_DOUBLE_ROW_SECOND);
    }
}

static void DrawHumidityMaxMinView(uint16_t context)
{
    struct node_t *node_p = Nodes_GetNodeFromID((uint8_t)context+128);

    if (node_p != NULL && Node_IsActive(node_p))
    {
        struct sensor_t *humidity_sensor_p = Node_GetHumiditySensor(node_p);
        int16_t humidity;

        Sensor_GetMaxValue(humidity_sensor_p, &humidity);
        libUI_Print("Max: %li.%u %%", 2, UI_DOUBLE_ROW_FIRST, humidity / 10, 0);

        Sensor_GetMinValue(humidity_sensor_p, &humidity);
        libUI_Print("Min: %li.%u %%", 2, UI_DOUBLE_ROW_SECOND, humidity / 10, 0);
    }
    else
    {
        libUI_Print("Max: --.-- %%", 2, UI_DOUBLE_ROW_FIRST);
        libUI_Print("Min: --.-- %%", 2, UI_DOUBLE_ROW_SECOND);
    }
}

static void DrawBatteryIndicator(void)
{

    libUI_DrawRectangle(BATT_INDICATOR_X, BATT_INDICATOR_Y, 6, 3);

    libUI_DrawLine(BATT_INDICATOR_X + 7, BATT_INDICATOR_Y + 1, BATT_INDICATOR_X + 7,
                   BATT_INDICATOR_Y + 2);

    return;
}

static void ClearAction(uint16_t context __attribute__ ((unused)))
{
    struct node_t *node_p = Nodes_GetNodeFromID((uint8_t)context+128);

    Sensor_Reset(Node_GetTemperatureSensor(node_p));
    Sensor_Reset(Node_GetHumiditySensor(node_p));

    Interface_Refresh();

    return;
}
