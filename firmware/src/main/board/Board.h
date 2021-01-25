/**
 * @file   Board.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2021-01-25 (Last edit)
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
#define RTC_SPI_MODE    1

#define RTC_EXTERNAL_BATTERY true

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

#define NHD223_DATA_DDR        DDRD
#define NHD223_DATA_PORT       PORTD
#define NHD223_CTRL_DDR        DDRC
#define NHD223_CTRL_PORT       PORTC
#define NHD223_ENABLE_PIN      DDC1
#define NHD223_RESET_PIN       DDC2
#define NHD223_READWRITE_PIN   DDC3
#define NHD223_REGSELECT_PIN   DDC4
#define NHD223_CHIPSELECT_PIN  DDC5

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

/**
 * Initialize the pins required for the NHD223 OLED display.
 */
void Board_NHD223_Init(void);

void Board_NHD223_SetDataPinsAsOutputs(void);
void Board_NHD223_SetDataPinsAsInputs(void);
void Board_NHD223_SetDataPins(uint8_t data);
void Board_NHD223_SetRegSelectLow(void);
void Board_NHD223_SetRegSelectHigh(void);
void Board_NHD223_SetEnableLow(void);
void Board_NHD223_SetEnableHigh(void);
void Board_NHD223_SetResetLow(void);
void Board_NHD223_SetResetHigh(void);
void Board_NHD223_SetChipSelectLow(void);
void Board_NHD223_SetChipSelectHigh(void);
void Board_NHD223_SetReadWriteLow(void);
void Board_NHD223_SetReadWriteHigh(void);

#endif
