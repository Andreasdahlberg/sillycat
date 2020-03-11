/**
 * @file   Node.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2020-03-11 (Last edit)
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

#ifndef NODE_H_
#define NODE_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "Sensor.h"

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

struct node_t
{
    uint32_t last_active;
    struct
    {
        uint16_t voltage;
        int16_t temperature;
        bool charging;
        bool connected;
    } battery;
    bool connected;
    int8_t rssi;
    uint8_t id;
    struct
    {
        struct sensor_t temperature;
        struct sensor_t humidity;
    } sensor;
};

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

/**
 * Initialize the supplied node.
 *
 * @param self_p Pointer to node struct.
 * @param id     Node ID.
 */
void Node_Init(struct node_t *self_p, uint8_t id);

/**
 * Report that the node has shown some activity.
 *
 * @param self_p Pointer to node struct.
 */
void Node_ReportActivity(struct node_t *self_p);

/**
 * Check if the supplied node is active.
 *
 * A node is considered active if it has sent any packets within the
 * last two report points.
 *
 * @param self_p Pointer to node struct.
 *
 * @return True if active, otherwise false.
 */
bool Node_IsActive(const struct node_t *self_p);

/**
 * Get a pointer to the temperature sensor struct associated with the
 * supplied node.
 *
 * @param self_p Pointer to node struct.
 *
 * @return Pointer to temperature sensor struct.
 */
struct sensor_t *Node_GetTemperatureSensor(struct node_t *self_p);

/**
 * Get a pointer to the humidity sensor struct associated with the
 * supplied node.
 *
 * @param self_p Pointer to node struct.
 *
 * @return Pointer to humidity sensor struct.
 */
struct sensor_t *Node_GetHumiditySensor(struct node_t *self_p);

/**
 * Check if the battery voltage is above the charging required limit.
 *
 * @param self_p Pointer to node struct.
 *
 * @return True if charging is required, otherwise false.
 */
bool Node_IsBatteryOk(const struct node_t *self_p);

/**
 * Get the battery voltage.
 *
 * @param self_p Pointer to node struct.
 *
 * @return Battery voltage in mV.
 */
uint16_t Node_GetBatteryVoltage(const struct node_t *self_p);

/**
 * Get the battery temperature.
 *
 * @param self_p Pointer to node struct.
 *
 * @return Battery temperature in Celsius.
 */
int16_t Node_GetBatteryTemperature(const struct node_t *self_p);

/**
 * Check if the battery is charging.
 *
 * @param self_p Pointer to node struct.
 *
 * @return True if charging, otherwise false.
 */
bool Node_IsBatteryCharging(const struct node_t *self_p);

/**
 * Check if the battery charger is connected.
 *
 * @param self_p Pointer to node struct.
 *
 * @return True if charger is connected, otherwise false.
 */
bool Node_IsBatteryChargerConnected(const struct node_t *self_p);

/**
 * Get the signal strength.
 *
 * @param self_p Pointer to node struct.
 *
 * @return Signal strength in dBm.
 */
int8_t Node_GetRSSI(const struct node_t *self_p);

/**
 * Set the signal strength.
 *
 * @param self_p Pointer to node struct.
 * @param rssi   Signal strength in dBm.
 */
void Node_SetRSSI(struct node_t *self_p, int8_t rssi);

/**
 * Get the node ID.
 *
 * @param self_p Pointer to node struct.
 *
 * @return Node ID.
 */
uint8_t Node_GetID(const struct node_t *self_p);

/**
 * Update the node with data from a remote node.
 *
 * @param self_p Pointer to node struct.
 * @param data_p Pointer to new node data.
 * @param length Length of the node data.
 */
void Node_Update(struct node_t *self_p, const void *data_p, size_t length);

#endif
