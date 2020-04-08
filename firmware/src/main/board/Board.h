/**
 * @file   Board.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2020-04-08 (Last edit)
 * @brief  Board support package for the main unit.
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

#include "commonBoard.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define RTC_SPI_DDR     DDRB
#define RTC_SPI_PORT    PORTB
#define RTC_SPI_SS      DDB2
#define RTC_SPI_MODE    0

#define RFM69_SPI_DDR   DDRB
#define RFM69_SPI_PORT  PORTB
#define RFM69_SPI_SS    DDB6
#define RFM69_SPI_MODE  0

#define RFM69_RESET_DDR     DDRC
#define RFM69_RESET_PORT    PORTC
#define RFM69_RESET_PIN     DDC0

#define PEC11_DDR           DDRB
#define PEC11_PINR          PINB
#define PEC11_PORT          PORTB
#define PEC11_LATCH_PIN     DDB0
#define PEC11_DATA_PIN      DDB1
#define PEC11_BUTTON_PIN    DDB7

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
 * Initialize the pins required for the PEC11 rotary encoder.
 */
void Board_PEC11_Init(void);

#endif
