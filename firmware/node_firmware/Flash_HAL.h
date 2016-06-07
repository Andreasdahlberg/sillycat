/**
 * @file   Flash_HAL.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2016-03-20 (Last edit)
 * @brief  Hardware abstraction layer for the flash interface.
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


#ifndef FLASH_HAL_H_
#define FLASH_HAL_H_

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//TODO: Verify pins and mode

#define CS_PIN DDD2
#define ReleaseCS() PORTD |= (1 << CS_PIN)
#define PullCS() PORTD &= ~(1 << CS_PIN)
#define InitCS() DDRD |= (1 << CS_PIN)

#define WP_PIN DDC6
#define ReleaseWP() PORTC |= (1 << WP_PIN)
#define PullWP() PORTC &= ~(1 << WP_PIN)
#define InitWP() DDRC |= (1 << WP_PIN)

#define SPIMODE 0

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

#endif /* FLASH_HAL_H_ */