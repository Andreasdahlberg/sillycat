/**
 * @file   commonBoard.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2020-01-26 (Last edit)
 * @brief  Common board support package.
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

#ifndef COMMON_BOARD_H_
#define COMMON_BOARD_H_

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
void Board_Init(void) __attribute__((weak));

/**
 * Initialize the pins required for the RTC.
 */
void Board_RTC_Init(void) __attribute__((weak));

/**
 * Prepare a RTC SPI transfer.
 *
 * Set the correct SPI-mode and select the RTC on the SPI-bus.
 */
void Board_RTC_SPIPreCallback(void) __attribute__((weak));

/**
 * Clean up of a RTC SPI transfer.
 *
 * Release the RTC in the SPI-bus.
 */
void Board_RTC_SPIPostCallback(void) __attribute__((weak));

/**
 * Initialize the pins required for the RFM69 transceiver.
 */
void Board_RFM69_Init(void) __attribute__((weak));

/**
 * Prepare a RFM69 SPI transfer.
 *
 * Set the correct SPI-mode and select the RFM69 on the SPI-bus.
 */
void Board_RFM69_SPIPreCallback(void) __attribute__((weak));

/**
 * Clean up of a RFM69 SPI transfer.
 *
 * Release the RFM69 in the SPI-bus.
 */
void Board_RFM69_SPIPostCallback(void) __attribute__((weak));

/**
 * Pull the RFM69 reset pin high.
 */
void Board_RFM69_PullReset(void) __attribute__((weak));

/**
 * Release the RFM69 reset pin.
 */
void Board_RFM69_ReleaseReset(void) __attribute__((weak));

/**
 * Perform a soft reset of the main MCU.
 */
void Board_SoftReset(void) __attribute__((weak, noreturn));

#endif
