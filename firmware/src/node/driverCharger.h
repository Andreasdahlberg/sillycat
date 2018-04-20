/**
 * @file   driverCharger.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-04-20 (Last edit)
 * @brief  LTC4060 charger driver
 *
 * Driver for the LTC4060 NiMH/NICd fast battery charger.
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

#ifndef DRIVERCHARGER_H_
#define DRIVERCHARGER_H_

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
 * Initialize the charger driver.
 */
void driverCharger_Init(void);

/**
 * Update the internal state of the driver.
 */
void driverCharger_Update(void);

/**
 * Check if the charger is connected.
 *
 * Use `driverCharger_IsCharging()` to check if the charger is actually
 * charging the battery.
 *
 * @return True if charger is connected, otherwise false.
 */
bool driverCharger_IsConnected(void);

/**
 * Check if the charger is charging the battery.
 *
 * If this functions returns true, it's implied that the charger is connected.
 * If not charging, the battery is either fully charged or to hot.
 *
 * @return True if charging, otherwise false.
 */
bool driverCharger_IsCharging(void);

/**
 * Get the current battery voltage.
 *
 * @return Battery voltage in mV,
 */
uint16_t driverCharger_GetBatteryVoltage(void);

/**
 * Get the current battery temperature.
 *
 * @return Battery temperature in Celsius.
 */
int16_t driverCharger_GetBatteryTemperature(void);

#endif
