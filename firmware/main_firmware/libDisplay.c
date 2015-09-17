/**
 * @file   libDisplay.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2015-09-13 (Last edit)
 * @brief  Implementation of display-library.
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


//TODO: Define this in one place
#define F_CPU 8000000UL // 8 MHz

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "libDisplay.h"
#include "libNHD223.h"
#include "libADC.h"
#include "libUART.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define LIBNAME "libDisplay"

//VRAM size in bytes
#define VRAM_PAGES 4
#define VRAM_COLUMNS 128
#define VRAM_SIZE (VRAM_PAGES * VRAM_COLUMNS)

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static uint8_t VRAM[VRAM_PAGES][VRAM_COLUMNS];

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void libDisplay_Init()
{
	//Do not init display hardware when debug is active, the debug
	//UART is using the same pins.
	#ifndef DEBUG_ENABLE
		libNHD223_Init();
	#endif	
	libDisplay_ClearVRAM();
}


void libDisplay_Clear()
{
	libDisplay_ClearVRAM();
	libDisplay_Flush();
}


void libDisplay_SetBrightness(uint8_t brightness)
{
	#ifndef DEBUG_ENABLE
		libNHD223_WriteCommand(0x81);
		libNHD223_WriteCommand(brightness);
	#endif
}


void libDisplay_On()
{
	#ifndef DEBUG_ENABLE	
		libNHD223_WriteCommand(SSD1305_DISPLAYON);
	#endif
}


void libDisplay_Off()
{
	#ifndef DEBUG_ENABLE	
		libNHD223_WriteCommand(SSD1305_DISPLAYOFF);
	#endif
}


void libDisplay_Reset()
{
	#ifndef DEBUG_ENABLE
		libNHD223_ResetDisplay();
	#endif
	libDisplay_Clear();
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
	#ifndef DEBUG_ENABLE
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
	#else
		libDisplay_DumpVRAMToUART();
	#endif
}


void libDisplay_ClearVRAM()
{
	memset(VRAM, 0x00, VRAM_SIZE);
}


void libDisplay_DumpVRAMToUART(void)
{
	uint8_t page;
	
	//TODO: Send VRAM asynchronous
	libUART_SendArray((uint8_t*)"<VRAM>", 6);
	for (page = 0; page < VRAM_PAGES; ++page)
	{
		libUART_SendArray(VRAM[page], VRAM_COLUMNS);
	}
	libUART_SendArray((uint8_t*)"\r\n", 2);		
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////