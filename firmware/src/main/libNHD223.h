/**
 * @file   libNHD223.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2016-04-03 (Last edit)
 * @brief  Header of NHD223-library.
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

#ifndef LIBNHD223_H_
#define LIBNHD223_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

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

void libNHD223_Init(void);
void libNHD223_WriteCommand(uint8_t command);
void libNHD223_WriteData(uint8_t data);
void libNHD223_ResetDisplay(void);
void libNHD223_ReadByte(uint8_t *data);
void libNHD223_SetPageAddress(uint8_t page_address);
void libNHD223_SetPageAddressRange(uint8_t start_page, uint8_t end_page);
void libNHD223_SetColumnAddress(uint8_t column_address);
void libNHD223_SetColumnAddressRange(uint8_t start_address,
                                     uint8_t end_address);

#endif /* LIBNHD223_H_ */