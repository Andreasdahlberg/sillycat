/**
 * @file   Node.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-11-12 (Last edit)
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
#include "Packet.h"
#include "Battery.h"
#include "Config.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

static inline uint32_t InactivityTimeoutTime(void);

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
           Timer_TimeDifference(self_p->last_active) <= InactivityTimeoutTime();
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

    return self_p->battery.voltage > BATTERY_LOW_VOLTAGE_MV;
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

    if (length == sizeof(struct packet_t))
    {
        struct packet_t packet;
        memcpy(&packet, data_p, sizeof(packet));

        self_p->battery.voltage = packet.battery.voltage;
        self_p->battery.temperature = packet.battery.temperature;
        self_p->battery.charging = packet.battery.charging;
        self_p->battery.connected = packet.battery.connected;

        if (packet.sensor.valid)
        {
            self_p->sensor.humidity.value = packet.sensor.humidity;
            self_p->sensor.humidity.valid = true;
            self_p->sensor.temperature.value = packet.sensor.temperature;
            self_p->sensor.temperature.valid = true;
        }
        else
        {
            self_p->sensor.humidity.valid = false;
            self_p->sensor.temperature.valid = false;

            WARNING("Invalid sensor data from 0x%x", Node_GetID(self_p));
        }
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

static inline uint32_t InactivityTimeoutTime(void)
{
    return Config_GetReportInterval() * 1000;
}
