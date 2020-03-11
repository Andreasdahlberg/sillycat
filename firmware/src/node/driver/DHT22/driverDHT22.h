/**
 * @file   driverDHT22.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2020-03-11 (Last edit)
 * @brief  DHT22 RHT sensor driver.
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

#ifndef DRIVERDHT22_H_
#define DRIVERDHT22_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stdbool.h>

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

/**
 * Initialize the DHT22 hardware and driver.
 */
void driverDHT22_Init(void);

/**
 * Update the internal state of the driver.
 */
void driverDHT22_Update(void);

/**
 * Get the temperature.
 *
 * @return Temperature in °C.
 */
int16_t driverDHT22_GetTemperature(void);

/**
 * Get the relative humidity.
 *
 * @return Relative humidity in percent.
 */
int16_t driverDHT22_GetHumidity(void);

/**
 * Check if the last measurement was valid.
 *
 * @return True if valid, otherwise false.
 */
bool driverDHT22_IsMeasurmentValid(void);

/**
 * Clear the measurement valid flag.
 */
void driverDHT22_ClearValidFlag(void);

/**
 * Check if the sensor is idle.
 *
 * When the sensor is idle it's ready to start a new measurements.
 *
 * @return True if idle, otherwise false.
 */
bool driverDHT22_IsIdle(void);

/**
 * Start a new measurement.
 */
void driverDHT22_StartMeasurement(void);

#endif
