/**
 * @file   libNHD223.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2021-01-25 (Last edit)
 * @brief  NHD2.23(SSD1305) graphic OLED display driver.
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

#ifndef LIBNHD223_H_
#define LIBNHD223_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include "SSD1305_Commands.h"

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
 * Init the display hardware.
 */
void libNHD223_Init(void);

/**
 * Write a command to the display.
 *
 * @param command Command to write.
 */
void libNHD223_WriteCommand(uint8_t command);

/**
 * Write data to the display.
 *
 * @param data Data to write.
 */
void libNHD223_WriteData(uint8_t data);

/**
 * Reset the display.
 *
 * Reset the display by pulling the reset pin high for 5 µs.
 */
void libNHD223_ResetDisplay(void);

/**
 * Configure the display to use horizontal addressing mode.
 *
 * In this mode both the column- and page-address are incremented
 * automatically.
 */
void libNHD223_SetHorizontalAddressingMode(void);

/**
 * Set the active page address.
 *
 * @param address Page address to set.
 */
void libNHD223_SetPageAddress(uint8_t address);

/**
 * Set the page address range.
 *
 * @param start_address Address to starting page.
 * @param end_address   Address to ending page.
 */
void libNHD223_SetPageAddressRange(uint8_t start_address, uint8_t end_address);

/**
 * Set the active column address.
 *
 * @param address Column address to set.
 */
void libNHD223_SetColumnAddress(uint8_t address);

/**
 * Set the column address range.
 *
 * @param start_address Address to starting column.
 * @param end_address   Address to ending column.
 */
void libNHD223_SetColumnAddressRange(uint8_t start_address, uint8_t end_address);

#endif
