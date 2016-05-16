/**
 * @file   libDisplay.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2016-05-16 (Last edit)
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

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

//NOTE: Include before all other headers
#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>

#include "libDisplay.h"
#include "libNHD223.h"
#include "libADC.h"
#include "UART.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define VRAM_PAGES 4
#define VRAM_COLUMNS 128
#define VRAM_SIZE (VRAM_PAGES * VRAM_COLUMNS)

#define DISPLAY_HEIGHT 32
#define DISPLAY_WIDTH 128

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static uint8_t VRAM[VRAM_PAGES][VRAM_COLUMNS];
static bool dislay_rotated;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

///
/// @brief Init the display and clear VRAM.
///
/// @param  None
/// @return None
///
void libDisplay_Init(void)
{
    //Do not init display hardware when debug is active, the debug
    //UART is using the same pins.
#ifndef DEBUG_ENABLE
    libNHD223_Init();
#endif

    dislay_rotated = false;
    libDisplay_ClearVRAM();
    return;
}

///
/// @brief Clear display.
///
/// @param  None
/// @return None
///
void libDisplay_Clear(void)
{
    libDisplay_ClearVRAM();
    libDisplay_Flush();
    return;
}

///
/// @brief Set display brightness.
///
/// @param  brightness Brightness to set.
/// @return None
///
void libDisplay_SetBrightness(uint8_t brightness)
{
#ifndef DEBUG_ENABLE
    libNHD223_WriteCommand(0x81);
    libNHD223_WriteCommand(brightness);
#endif
    return;
}

///
/// @brief Turn display on.
///
/// @param  None
/// @return None
///
void libDisplay_On(void)
{
#ifndef DEBUG_ENABLE
    libNHD223_WriteCommand(SSD1305_DISPLAYON);
#endif
    return;
}

///
/// @brief Turn display off.
///
/// @param  None
/// @return None
///
void libDisplay_Off(void)
{
#ifndef DEBUG_ENABLE
    libNHD223_WriteCommand(SSD1305_DISPLAYOFF);
#endif
    return;
}

///
/// @brief Reset the display.
///
/// @param  None
/// @return None
///
void libDisplay_Reset(void)
{
#ifndef DEBUG_ENABLE
    libNHD223_ResetDisplay();
#endif
    libDisplay_Clear();
    return;
}

///
/// @brief Set a specific pixel on the display.
///
/// @param  x Pixel coordinate in the x-axis.
/// @param  y Pixel coordinate in the y-axis.
/// @return None
///
void libDisplay_SetPixel(uint8_t x, uint8_t y)
{
    uint8_t page_index;
    uint8_t page_offset;

    if (dislay_rotated == true)
    {
        x = DISPLAY_WIDTH - x - 1;
        y = DISPLAY_HEIGHT - y - 1;
    }

    //Shift 3 bits to the right to divide by 8(page height) and floor value
    page_index = (y >> 3);
    page_offset = (y - (page_index << 3));

    if (page_index < VRAM_PAGES && x < VRAM_COLUMNS)
    {
        VRAM[page_index][x] |= (1 << page_offset);
    }
    return;
}

///
/// @brief Flush content in VRAM to the display.
///
/// @param  None
/// @return None
///
void libDisplay_Flush(void)
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
    return;
}

///
/// @brief Clear VRAM.
///
/// @param  None
/// @return None
///
void libDisplay_ClearVRAM()
{
    memset(VRAM, 0x00, VRAM_SIZE);
}

///
/// @brief Rotate display contents.
///
/// Rotate display contents. VRAM is cleared after rotation.
///
/// @param  state Set to true if contents should be rotated.
/// @return None
///
void libDisplay_Rotate(bool state)
{
    dislay_rotated = state;
    libDisplay_ClearVRAM();
    return;
}

#ifdef DEBUG_ENABLE
///
/// @brief Dump content in VRAM to debug UART.
///
/// @param  None
/// @return None
///
void libDisplay_DumpVRAMToUART(void)
{
    uint8_t page;

    UART_Write("<VRAM>", 6);
    for (page = 0; page < VRAM_PAGES; ++page)
    {
        uint8_t cnt = 0;

        while (cnt != VRAM_COLUMNS)
        {
            cnt += UART_Write(&(VRAM[page][cnt]), VRAM_COLUMNS - cnt);
        }
    }
    UART_Write("\r\n", 2);

    return;
}
#endif

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////