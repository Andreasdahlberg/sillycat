/**
 * @file   driverNTC.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-02-08 (Last edit)
 * @brief  NTC sensor driver
 *
 * Driver for NTC sensors.
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

#ifndef DRIVERNTC_H_
#define DRIVERNTC_H_

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
 * Initialize all available NTC sensors.
 */
void driverNTC_Init(void);

/**
 * Get a pointer to the sensor matching the supplied ID.
 *
 * @param id ID of sensor.
 *
 * @return Pointer to sensor struct.
 */
struct sensor_t *driverNTC_GetSensor(size_t id);

#endif
