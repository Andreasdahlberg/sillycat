/**
 * @file   RFM69_HAL.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2016-05-08 (Last edit)
 * @brief  Header for RFM69 HAL.
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

#ifndef RFM69_HAL_H_
#define RFM69_HAL_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define CS_PIN DDB2
#define ReleaseCS() PORTB |= (1 << CS_PIN)
#define PullCS() PORTB &= ~(1 << CS_PIN)
#define InitCS() DDRB |= (1 << CS_PIN)
#define SPIMODE 0

#define RESET_PIN DDC0
#define PullReset() PORTC |= (1 << RESET_PIN); \
                    DDRC |= (1 << RESET_PIN)
#define InitReset() DDRC &= ~(1 << RESET_PIN); \
                    PORTC &= ~(1 << RESET_PIN)
#define ReleaseReset() InitReset()

#define IO_PIN DDC2
#define InitIO()  DDRC &= ~(1 << IO_PIN);

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

#endif /* RFM69_HAL_H_ */