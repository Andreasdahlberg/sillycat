/**
 * @file   mock_Node.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2020-10-21 (Last edit)
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

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include "mock_Node.h"

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

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void __wrap_Node_Init(struct node_t *self_p, uint8_t id)
{
}

void __wrap_Node_ReportActivity(struct node_t *self_p)
{
    function_called();
}

bool __wrap_Node_IsActive(struct node_t *self_p)
{
    mock_type(bool);
}

struct sensor_t *__wrap_Node_GetTemperatureSensor(struct node_t *self_p)
{
    mock_type(struct sensor_t *);
}

struct sensor_t *__wrap_Node_GetHumiditySensor(struct node_t *self_p)
{
    mock_type(struct sensor_t *);
}

bool __wrap_Node_IsBatteryOk(struct node_t *self_p)
{
    mock_type(bool);
}

uint16_t __wrap_Node_GetBatteryVoltage(struct node_t *self_p)
{
    mock_type(uint16_t);
}

int16_t __wrap_Node_GetBatteryTemperature(struct node_t *self_p)
{
    mock_type(int16_t);
}

bool __wrap_Node_IsBatteryCharging(struct node_t *self_p)
{
    mock_type(bool);
}

bool __wrap_Node_IsBatteryChargerConnected(struct node_t *self_p)
{
    mock_type(bool);
}

int8_t __wrap_Node_GetRSSI(struct node_t *self_p)
{
    mock_type(int8_t);
}

void __wrap_Node_SetRSSI(struct node_t *self_p, int8_t rssi)
{
    check_expected(rssi);
}

uint8_t __wrap_Node_GetID(struct node_t *self_p)
{
    mock_type(uint8_t);
}

void __wrap_Node_Update(struct node_t *self_p, void *data_p, size_t length)
{
    function_called();
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////
