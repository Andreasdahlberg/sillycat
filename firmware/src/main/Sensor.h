/**
 * @file   Sensor.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-02-21 (Last edit)
 * @brief  Header of Sensor module
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

#ifndef SENSOR_H_
#define SENSOR_H_

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

struct sensor_t
{
    void (*Update)(struct sensor_t *);
    uint16_t id;
    int16_t value;
    bool valid;
    struct {
        int16_t max;
        int16_t min;
        bool valid;
    } statistics;
};

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

/**
 * Initialize the sensor module.
 */
void Sensor_Init(void);

/**
 * Update all sensors registered in the sensor module.
 */
void Sensor_Update(void);

/**
 * Register a sensor to the sensor module. This sensor will be updated when
 * `Sensor_Update()` is called.
 *
 * @param self Pointer to sensor struct.
 */
void Sensor_Register(struct sensor_t *self);

/**
 * Get the latest value recorded for the supplied sensor.
 *
 * @param self  Pointer to sensor struct.
 * @param value Pointer to variable where the value will be stored.
 *
 * @return True if the sensor has a valid value, otherwise false.
 */
bool Sensor_GetValue(struct sensor_t *self, int16_t *value);

/**
 * Get the maximum value recorded for the supplied sensor.
 *
 * @param self  Pointer to sensor struct.
 * @param value Pointer to variable where the value will be stored.
 *
 * @return True if the sensor has a valid maximum value, otherwise false.
 */
bool Sensor_GetMaxValue(struct sensor_t *self, int16_t *value);

/**
 * Get the minimum value recorded for the supplied sensor.
 *
 * @param self  Pointer to sensor struct.
 * @param value Pointer to variable where the value will be stored.
 *
 * @return True if the sensor has a valid minimum value, otherwise false.
 */
bool Sensor_GetMinValue(struct sensor_t *self, int16_t *value);

/**
 * Check if the supplied sensor has a valid value.
 *
 * @param self Pointer to sensor struct.
 *
 * @return True if the sensor has a valid value, otherwise false.
 */
bool Sensor_IsValid(struct sensor_t *self);

/**
 * Check if the supplied sensor has any valid statistics.
 *
 * @param self Pointer to sensor struct.
 *
 * @return True if the sensor has valid statistics.
 */
bool Sensor_IsStatisticsValid(struct sensor_t *self);

/**
 * Reset the recorded values for the supplied sensor.
 *
 * @param self Pointer to sensor struct.
 */
void Sensor_Reset(struct sensor_t *self);

#endif
