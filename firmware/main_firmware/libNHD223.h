/**
 * @file   libNHD223.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2015-10-03 (Last edit)
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

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define SSD1305_SETLOWCOLUMN		0x00
#define SSD1305_SETHIGHCOLUMN		0x10
#define SSD1305_SETSTARTCOLUMN		0x21
#define SSD1305_SETSTARTPAGE		0x22
#define SSD1305_MEMORYMODE			0x20
#define SSD1305_SETSTARTLINE		0x40
#define SSD1305_SETCONTRAST			0x81
#define SSD1305_SEGREMAP			0xA0
#define SSD1305_SEGREMAP1			0xA1
#define SSD1305_DISPLAYALLON_RESUME 0xA4
#define SSD1305_DISPLAYALLON		0xA5
#define SSD1305_NORMALDISPLAY		0xA6
#define SSD1305_INVERTDISPLAY		0xA7
#define SSD1305_SETMULTIPLEX		0xA8
#define SSD1305_DCVOLTCONVERT       0xAD
#define SSD1305_DISPLAYOFF			0xAE
#define SSD1305_DISPLAYON			0xAF
#define SSD1305_SET_PAGE			0xB0
#define SSD1305_COMSCANDEC			0xC8
#define SSD1305_SETDISPLAYOFFSET	0xD3
#define SSD1305_SETDISPLAYCLOCKDIV	0xD5
#define SSD1305_SETPRECHARGE		0xD9
#define SSD1305_SETCOMPINS			0xDA
#define SSD1305_SETVCOMDETECT		0xDB
#define SSD1305_SET_MODIFY			0xE0
#define SSD1305_CLR_MODIFY			0xEE
#define SSD1305_NOP					0xE3

#define SSD1305_SETHORISCROLL		0x26
#define SSD1305_SETVERTSCROLL		0xA3
#define SSD1305_SETCONTSCROLL		0x29 
#define SSD1305_CLRSCROLL			0x2E 
#define SSD1305_SETSCROLL			0x2F 

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

void libNHD223_Init();
void libNHD223_Update();
bool libNHD223_WriteCommand(uint8_t command);
bool libNHD223_WriteData(uint8_t data);
void libNHD223_ResetDisplay();
bool libNHD223_ReadByte(uint8_t *data);
bool libNHD223_SetPageAddress(uint8_t page_address);
bool libNHD223_SetPageAddressRange(uint8_t start_page, uint8_t end_page);
void libNHD223_SetColumnAddress(uint8_t column_address);
void libNHD223_SetColumnAddressRange(uint8_t start_address, uint8_t end_address);

#endif /* LIBNHD223_H_ */