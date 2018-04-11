/**
 * @file   Battery.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-04-11 (Last edit)
 * @brief  Defines describing the battery parameters.
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

#ifndef BATTERY_H_
#define BATTERY_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define BATTERY_NUMBER_OF_CELLS             2
#define BATTERY_LOW_CELL_VOLTAGE_MV         1100
#define BATTERY_CRITICAL_CELL_VOLTAGE_MV    950

#define BATTERY_LOW_VOLTAGE_MV      (BATTERY_NUMBER_OF_CELLS * BATTERY_LOW_CELL_VOLTAGE_MV)
#define BATTERY_CRITICAL_VOLTAGE_MV (BATTERY_NUMBER_OF_CELLS * BATTERY_CRITICAL_CELL_VOLTAGE_MV)

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

#endif
