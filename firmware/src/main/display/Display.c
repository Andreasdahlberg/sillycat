/**
 * @file   Display.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2021-02-04 (Last edit)
 * @brief  Display interface.
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

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include "common.h"
#include <string.h>
#include "driverNHD223.h"
#include "Display.h"

#ifdef DEBUG_ENABLE
#include "UART.h"
#endif

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define VRAM_SIZE (NHD223_NUMBER_OF_PAGES * NHD223_NUMBER_OF_COLUMNS)

#define DISPLAY_HEIGHT 32
#define DISPLAY_WIDTH 128

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

struct module_t
{
    uint8_t VRAM[NHD223_NUMBER_OF_PAGES][NHD223_NUMBER_OF_COLUMNS];
    bool rotated;
};

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static struct module_t module;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void Display_Init(void)
{
    //Do not init display hardware when debug is active, the debug
    //UART is using the same pins.
#ifndef DEBUG_ENABLE
    driverNHD223_Init();
    driverNHD223_SetHorizontalAddressingMode();
    driverNHD223_SetColumnAddressRange(0, NHD223_NUMBER_OF_COLUMNS - 1);
    driverNHD223_SetPageAddressRange(0, NHD223_NUMBER_OF_PAGES - 1);
#endif

    module.rotated = false;
    Display_ClearVRAM();
}

void Display_On(void)
{
#ifndef DEBUG_ENABLE
    driverNHD223_WriteCommand(SSD1305_DISPLAYON);
#endif
}

void Display_Off(void)
{
#ifndef DEBUG_ENABLE
    driverNHD223_WriteCommand(SSD1305_DISPLAYOFF);
#endif
}

void Display_Clear(void)
{
    Display_ClearVRAM();
    Display_Flush();
}

void Display_ClearVRAM()
{
    memset(module.VRAM, 0x00, VRAM_SIZE);
}

void Display_Flush(void)
{
#ifndef DEBUG_ENABLE
    for (uint8_t page = 0; page < NHD223_NUMBER_OF_PAGES; ++page)
    {
        for (uint8_t column = 0; column < NHD223_NUMBER_OF_COLUMNS; ++column)
        {
            driverNHD223_WriteData(module.VRAM[page][column]);
        }
    }
#else
    Display_DumpVRAMToUART();
#endif
}

void Display_Reset(void)
{
#ifndef DEBUG_ENABLE
    driverNHD223_ResetDisplay();
#endif
    Display_Clear();
}

void Display_SetBrightness(uint8_t brightness)
{
#ifndef DEBUG_ENABLE
    driverNHD223_WriteCommand(0x81);
    driverNHD223_WriteCommand(brightness);
#else
    UNUSED(brightness);
#endif
}

void Display_SetPixel(uint8_t x, uint8_t y)
{
    uint8_t page_index;
    uint8_t page_offset;

    if (module.rotated == true)
    {
        x = DISPLAY_WIDTH - x - 1;
        y = DISPLAY_HEIGHT - y - 1;
    }

    //Shift 3 bits to the right to divide by 8(page height) and floor value
    page_index = (y >> 3);
    page_offset = (y - (page_index << 3));

    if (page_index < NHD223_NUMBER_OF_PAGES && x < NHD223_NUMBER_OF_COLUMNS)
    {
        module.VRAM[page_index][x] |= (1 << page_offset);
    }
}

void Display_Rotate(bool state)
{
    module.rotated = state;
    Display_ClearVRAM();
}

#ifdef DEBUG_ENABLE
void Display_DumpVRAMToUART(void)
{
    UART_Write("<VRAM>", 6);
    for (uint8_t page = 0; page < NHD223_NUMBER_OF_PAGES; ++page)
    {
        uint8_t cnt = 0;

        while (cnt != NHD223_NUMBER_OF_COLUMNS)
        {
            cnt += UART_Write(&(module.VRAM[page][cnt]), NHD223_NUMBER_OF_COLUMNS - cnt);
        }
    }
    UART_Write("\r\n", 2);
}
#endif

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////
