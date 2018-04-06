/**
 * @file   Node.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-04-06 (Last edit)
 * @brief  Implementation of remote node abstraction layer.
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

#include "common.h"
#include <string.h>
#include "Node.h"
#include "Timer.h"
#include "libDebug.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define LOW_BATTERY_LIMIT           2200
#define NODE_INACTIVE_TIME_LIMIT    20000

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void Node_Init(struct node_t *self_p, uint8_t id)
{
    sc_assert(self_p != NULL);

    self_p->id = id;
    self_p->connected = false;

    /**
     * Since this module is considered as an sensor driver it's OK to access
     * the members of the sensors structs directly.
     *
     * Set the update function to NULL since the remote sensors are not updated
     * periodically. Instead they are updated when a packet with sensor
     * readings are received.
     */
    self_p->sensor.temperature.Update = NULL;
    self_p->sensor.humidity.Update = NULL;

    self_p->sensor.temperature.valid = false;
    self_p->sensor.humidity.valid = false;

    Sensor_Register(&self_p->sensor.temperature);
    Sensor_Register(&self_p->sensor.humidity);
}

void Node_ReportActivity(struct node_t *self_p)
{
    self_p->connected = true;
    self_p->last_active = Timer_GetMilliseconds();
}

bool Node_IsActive(struct node_t *self_p)
{
    sc_assert(self_p != NULL);

    return self_p->connected &&
           Timer_TimeDifference(self_p->last_active) <= NODE_INACTIVE_TIME_LIMIT;
}

struct sensor_t *Node_GetTemperatureSensor(struct node_t *self_p)
{
    sc_assert(self_p != NULL);

    return &self_p->sensor.temperature;
}

struct sensor_t *Node_GetHumiditySensor(struct node_t *self_p)
{
    sc_assert(self_p != NULL);

    return &self_p->sensor.humidity;
}

bool Node_IsBatteryOk(struct node_t *self_p)
{
    sc_assert(self_p != NULL);

    return self_p->battery.voltage > LOW_BATTERY_LIMIT;
}

uint16_t Node_GetBatteryVoltage(struct node_t *self_p)
{
    sc_assert(self_p != NULL);

    return self_p->battery.voltage;
}

int16_t Node_GetBatteryTemperature(struct node_t *self_p)
{
    sc_assert(self_p != NULL);

    return self_p->battery.temperature;
}

bool Node_IsBatteryCharging(struct node_t *self_p)
{
    sc_assert(self_p != NULL);

    return self_p->battery.charging;
}

bool Node_IsBatteryChargerConnected(struct node_t *self_p)
{
    sc_assert(self_p != NULL);

    return self_p->battery.connected;
}

int8_t Node_GetRSSI(struct node_t *self_p)
{
    sc_assert(self_p != NULL);

    return self_p->rssi;
}

void Node_SetRSSI(struct node_t *self_p, int8_t rssi)
{
    sc_assert(self_p != NULL);

    self_p->rssi = rssi;
}

uint8_t Node_GetID(struct node_t *self_p)
{
    sc_assert(self_p != NULL);

    return self_p->id;
}

void Node_Update(struct node_t *self_p, void *data_p, size_t length)
{
    sc_assert(self_p != NULL);
    sc_assert(data_p != NULL);

    if (length >= 2 * sizeof(uint16_t))
    {
        uint8_t *content_p = (uint8_t *)data_p;

        memcpy(&self_p->sensor.humidity.value, content_p, sizeof(uint16_t));
        self_p->sensor.humidity.valid = true;

        content_p += sizeof(uint16_t);

        memcpy(&self_p->sensor.temperature.value, content_p, sizeof(uint16_t));
        self_p->sensor.temperature.valid = true;
    }
    else
    {
        WARNING(
            "Incorrect data length[%zu] from 0x%x",
            length,
            Node_GetID(self_p)
        );
    }
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////
