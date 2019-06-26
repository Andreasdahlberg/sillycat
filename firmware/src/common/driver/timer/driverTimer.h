/**
 * @file   driverTimer.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2019-06-27 (Last edit)
 * @brief  Implementation of ATmega328 timer driver.
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

#ifndef DRIVERTIMER_H_
#define DRIVERTIMER_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include <stdint.h>

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
 * Initialize the timer hardware to trigger an interrupt every 1 ms
 */
void driverTimer_Init(void);

/**
 * Start the system timer.
 */
void driverTimer_Start();

/**
 * Stop the system timer.
 */
void driverTimer_Stop();

/**
 * Reset the system timer.
 */
void driverTimer_Reset(void);

/**
 *  Get the current time(ms).
 *
 * @return The current system time in milliseconds.
 */
uint32_t driverTimer_GetMilliseconds(void);

#endif
