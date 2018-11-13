/**
 * @file   Board.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-11-13 (Last edit)
 * @brief  Board support package for the node unit.
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

#ifndef BOARD_H_
#define BOARD_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

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
 * Initialize low level board hardware.
 */
void Board_Init(void);

/**
 * Initialize the pins required for the RTC.
 */
void Board_RTC_Init(void);

/**
 * Prepare a RTC SPI transfer.
 *
 * Set the correct SPI-mode and select the RTC on the SPI-bus.
 */
void Board_RTC_SPIPreCallback(void);

/**
 * Clean up of a RTC SPI transfer.
 *
 * Release the RTC in the SPI-bus.
 */
void Board_RTC_SPIPostCallback(void);

#endif
