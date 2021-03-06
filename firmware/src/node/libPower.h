/**
 * @file   libPower.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-02-16 (Last edit)
 * @brief  Implementation of a low level power functions.
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

#ifndef LIBPOWER_H_
#define LIBPOWER_H_

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
 * Initialize the low level power module.
 */
void libPower_Init(void);

/**
 * Enter sleep mode.
 */
void libPower_Sleep(void);

/**
 * Get an approximation of the maximum output current from the buck-boost
 * converter at the specified input voltage.
 *
 * The approximation works best in the normal operating range(1.8V to 2.8V).
 *
 * @param vin Buck-boost input voltage in mV.
 *
 * @return Max output current in mA.
 */
uint32_t libPower_GetMaxOutputCurrent(uint16_t vin);

#endif
