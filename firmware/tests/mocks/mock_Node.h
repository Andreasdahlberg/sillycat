/**
 * @file   mock_Node.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2019-05-08 (Last edit)
 * @brief  Mock functions for the node module.
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

#ifndef WRAP_NODE_H_
#define WRAP_NODE_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include <stdbool.h>
#include "Node.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

void __wrap_Node_Init(struct node_t *self_p, uint8_t id);
void __wrap_Node_ReportActivity(struct node_t *self_p);
bool __wrap_Node_IsActive(struct node_t *self_p);
struct sensor_t *__wrap_Node_GetTemperatureSensor(struct node_t *self_p);
struct sensor_t *__wrap_Node_GetHumiditySensor(struct node_t *self_p);
bool __wrap_Node_IsBatteryOk(struct node_t *self_p);
uint16_t __wrap_Node_GetBatteryVoltage(struct node_t *self_p);
int16_t __wrap_Node_GetBatteryTemperature(struct node_t *self_p);
bool __wrap_Node_IsBatteryCharging(struct node_t *self_p);
bool __wrap_Node_IsBatteryChargerConnected(struct node_t *self_p);
int8_t __wrap_Node_GetRSSI(struct node_t *self_p);
void __wrap_Node_SetRSSI(struct node_t *self_p, int8_t rssi);
uint8_t __wrap_Node_GetID(struct node_t *self_p);
void __wrap_Node_Update(struct node_t *self_p, void *data_p, size_t length);

#endif
