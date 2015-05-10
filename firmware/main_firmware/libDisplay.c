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

//TODO: Define this in one place
#define F_CPU 8000000UL // 8 MHz

#include <util/delay.h>
#include <string.h>
#include "common.h"
#include "libDisplay.h"
#include "libNHD223.h"

//Fonts
#include "font_ubuntu_mono.h"
#include "font_ubuntu_mono_large.h"

#define LIBNAME "libDisplay"

//VRAM size in bytes
#define VRAM_PAGES 4
#define VRAM_COLUMNS 128
#define VRAM_SIZE (VRAM_PAGES * VRAM_COLUMNS)

static void libDisplay_ClearVRAM();

static uint8_t VRAM[VRAM_PAGES][VRAM_COLUMNS];
const FONT_INFO *current_font;

void PrintText(char *buffer, uint8_t x_pos, uint8_t y_pos)
{
	uint8_t desc_offset;
	uint16_t char_offset;
	uint8_t byte_index;
	uint8_t bit_index;
	uint8_t buffer_index;
	uint8_t data_row = 0;
	uint8_t data_column= 0;
	
	
	for (buffer_index = 0; buffer_index < strlen(buffer); ++buffer_index)
	{
		if ((char)buffer[buffer_index] != ' ')
		{
			desc_offset = (buffer[buffer_index] - (char)current_font->startChar);
			char_offset = current_font->charInfo[desc_offset].offset;

			for (data_row = 0; data_row < current_font->heightPages; ++data_row)
			{
				for (data_column = 0; data_column < current_font->widthPages; ++data_column)
				{
					byte_index = (data_row * current_font->widthPages) + data_column;
					for (bit_index = 0; bit_index < 8; ++bit_index)
					{
						if (((current_font->data[char_offset+byte_index]) & (1 << (7-bit_index))) > 0)					
						{
							libDisplay_SetPixel(x_pos + bit_index+(data_column << 3), y_pos + data_row);
						}
					}			
				}	
			}
			x_pos += (current_font->charInfo[desc_offset].widthBits + 2);
		}
		else
		{
			x_pos += 6;
		}
	}
}


void libDisplay_Init()
{
	libNHD223_Init();
	libDisplay_ClearVRAM();
}

void libDisplay_Clear()
{
	libDisplay_ClearVRAM();
	libDisplay_Flush();
}


void libDisplay_On()
{
	libNHD223_WriteCommand(SSD1305_DISPLAYON);
}


void libDisplay_Off()
{
	libNHD223_WriteCommand(SSD1305_DISPLAYOFF);
}


void libDisplay_Reset()
{
	libNHD223_ResetDisplay();
	libDisplay_Clear();
}


void libDisplay_SetFont(FONT_INFO *font)
{
	current_font = font;
}


void libDisplay_TEST()
{
	uint8_t x;

	libDisplay_Clear();
	libDisplay_SetFont(&ubuntuMono_36ptFontInfo);
	
	for (x = 0; x < 32; ++x)
	{

		libDisplay_ClearVRAM();
		PrintText("23 C", x, 0);
		libDisplay_Flush();
	}
}


void libDisplay_SetPixel(uint8_t x, uint8_t y)
{
	uint8_t page_index;
	uint8_t page_offset;

	//Shift 3 bits to the right to divide by 8(page height) and floor value
	page_index = (y >> 3);
	page_offset = (y - (page_index << 3));
	
	if (page_index < VRAM_PAGES && x < VRAM_COLUMNS)
	{
		VRAM[page_index][x] |= (1 << page_offset);
	}
}


void libDisplay_Flush()
{
	uint8_t page;
	uint8_t column;

	for (page = 0; page < VRAM_PAGES; ++page)
	{
		libNHD223_SetPageAddress(page);
		for (column = 0; column < VRAM_COLUMNS; ++column)
		{
			libNHD223_WriteData(VRAM[page][column]);			
		}
	}
}


static void libDisplay_ClearVRAM()
{
	memset(VRAM, 0x00, VRAM_SIZE);
}