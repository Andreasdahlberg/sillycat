/**
 * @file   Board.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2020-01-23 (Last edit)
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

#include "commonBoard.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define RTC_SPI_DDR     DDRD
#define RTC_SPI_PORT    PORTD
#define RTC_SPI_MFP     DD3
#define RTC_SPI_SS      DDD4
#define RTC_SPI_MODE 0

#define RFM69_SPI_DDR   DDRB
#define RFM69_SPI_PORT  PORTB
#define RFM69_SPI_SS    DDB2
#define RFM69_SPI_MODE  0

#define RFM69_RESET_DDR     DDRC
#define RFM69_RESET_PORT    PORTC
#define RFM69_RESET_PIN     DDC0

#define RFM69_IO_DDR    DDRC
#define RFM69_IO_PIN    DDC2

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

void Board_RFM69_Init(void);

#endif
