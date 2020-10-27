/**
 * @file   mock_Sensor.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2020-10-27 (Last edit)
 * @brief  Mock functions for Node module.
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
#include "mock_Sensor.h"

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

void __wrap_Sensor_Init(void)
{
}

void __wrap_Sensor_Update(void)
{
}

void __wrap_Sensor_Register(struct sensor_t *self)
{
}

bool __wrap_Sensor_GetValue(const struct sensor_t *self, int16_t *value)
{
    mock_type(bool);
}

bool __wrap_Sensor_GetMaxValue(const struct sensor_t *self, int16_t *value)
{
    mock_type(bool);
}

bool __wrap_Sensor_GetMinValue(const struct sensor_t *self, int16_t *value)
{
    mock_type(bool);
}

bool __wrap_Sensor_IsValid(const struct sensor_t *self)
{
    mock_type(bool);
}

bool __wrap_Sensor_IsStatisticsValid(const struct sensor_t *self)
{
    mock_type(bool);
}

void __wrap_Sensor_Reset(struct sensor_t *self)
{
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////
