/**
 * @file   libS25FL1K_HAL.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2019-04-27 (Last edit)
 * @brief  Header with HAL defines for the S25FL1-K low level driver.
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

#ifndef LIBS25FL1K_HAL_H_
#define LIBS25FL1K_HAL_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include <avr/io.h>

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define CS_PIN DDD2
#define ReleaseCS() PORTD |= (1 << CS_PIN)
#define PullCS() PORTD &= ~(1 << CS_PIN)
#define InitCS() DDRD |= (1 << CS_PIN)

#define WP_PIN DDC6
#define InitWP()  DDRC &= ~(1 << WP_PIN)
#define PullWP() PORTC &= ~(1 << WP_PIN)

#define SPIMODE 0

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

#endif
