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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "libDisplay.h"
#include "libNHD223.h"
#include "libADC.h"



#define LIBNAME "libDisplay"

//VRAM size in bytes
#define VRAM_PAGES 4
#define VRAM_COLUMNS 128
#define VRAM_SIZE (VRAM_PAGES * VRAM_COLUMNS)

static uint8_t VRAM[VRAM_PAGES][VRAM_COLUMNS];




void libDisplay_SetBrightness(uint8_t brightness)
{
	libNHD223_WriteCommand(0x81);
	libNHD223_WriteCommand(brightness);	
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

void libDisplay_TEST()
{
	char data_buffer[12];
	uint16_t value;
	
	value = libADC_GetSample();
	

	ultoa(123, data_buffer, 10);

	//libDisplay_SetFont(&ubuntuMono_36ptFontInfo);
	
	libDisplay_SetBrightness((value >> 2));
	

	libDisplay_ClearVRAM();
	libDisplay_PrintText(data_buffer, 32, 0);
	libDisplay_Flush();
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


void libDisplay_ClearVRAM()
{
	memset(VRAM, 0x00, VRAM_SIZE);
}