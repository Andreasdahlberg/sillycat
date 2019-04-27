/**
 * @file   libUI.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2019-04-27 (Last edit)
 * @brief  Implementation of UI library.
 *
 * The UI library contains functions for drawing simple shapes and for
 * printing text.
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
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "libDisplay.h"
#include "libUI.h"
#include "font.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define PRINT_BUFFER_SIZE 24

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

static inline bool IsPixelBitSet(uint8_t data, uint8_t bit);
static inline void PrintChar(const glyph_info_t *char_ptr, uint8_t x_pos,
                             uint8_t y_pos);

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void libUI_Update(void)
{
    libDisplay_Flush();
    libDisplay_ClearVRAM();
}

void libUI_DrawLine(uint8_t x_start, uint8_t y_start, uint8_t x_end,
                    uint8_t y_end)
{
    int dx = abs(x_end - x_start);
    int sx = x_start < x_end ? 1 : -1;
    int dy = abs(y_end - y_start);
    int sy = y_start < y_end ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2;
    int e2;

    while (1)
    {
        libDisplay_SetPixel(x_start, y_start);
        if (x_start == x_end && y_start == y_end)
        {
            break;
        }
        e2 = err;

        if (e2 > -dx)
        {
            err -= dy;
            x_start += sx;
        }

        if (e2 < dy)
        {
            err += dx;
            y_start += sy;
        }
    }
}

void libUI_DrawRectangle(uint8_t x_start, uint8_t y_start, uint8_t width,
                         uint8_t height)
{
    libUI_DrawLine(x_start, y_start, x_start + width, y_start);
    libUI_DrawLine(x_start, y_start, x_start, y_start + height);
    libUI_DrawLine(x_start, y_start + height, x_start + width, y_start + height);
    libUI_DrawLine(x_start + width, y_start, x_start + width, y_start + height);
}

void libUI_DrawCircle(uint8_t x_pos, uint8_t y_pos, uint8_t radius)
{
    int x = radius;
    int y = 0;
    int decisionOver2 = 1 - x; // Decision criterion divided by 2 evaluated at x=r, y=0

    while (x >= y)
    {
        libDisplay_SetPixel( x + x_pos,  y + y_pos);
        libDisplay_SetPixel( y + x_pos,  x + y_pos);
        libDisplay_SetPixel(-x + x_pos,  y + y_pos);
        libDisplay_SetPixel(-y + x_pos,  x + y_pos);
        libDisplay_SetPixel(-x + x_pos, -y + y_pos);
        libDisplay_SetPixel(-y + x_pos, -x + y_pos);
        libDisplay_SetPixel( x + x_pos, -y + y_pos);
        libDisplay_SetPixel( y + x_pos, -x + y_pos);
        y++;
        if (decisionOver2 <= 0)
        {
            decisionOver2 += 2 * y + 1;   // Change in decision criterion for y -> y+1
        }
        else
        {
            x--;
            decisionOver2 += 2 * (y - x) + 1;   // Change for y -> y+1, x -> x-1
        }
    }
}

///
/// @brief Print string from FLASH to the display
///
/// @param  text Text to print
/// @param  x_pos Vertical position in pixels
/// @param  y_pos Horizontal position in pixels
/// @param  variable number of arguments
/// @return None
///
void libUI_Print_P(const char *text, uint8_t x_pos, uint8_t y_pos, ...)
{
    va_list args;
    char buffer[PRINT_BUFFER_SIZE];

    va_start(args, y_pos);

    vsnprintf_P(buffer, sizeof(buffer), text, args);

    //Make sure that the buffer is null terminated.
    buffer[sizeof(buffer) - 1] = '\0';

    libUI_PrintText(buffer, x_pos, y_pos);
    va_end(args);
}

void libUI_PrintText(const char *buffer, uint8_t x_pos, uint8_t y_pos)
{
    sc_assert(buffer != NULL);
    const char *char_ptr = buffer;

    while(*char_ptr != '\0')
    {
        glyph_info_t glyph;
        if (Font_GetGlyphInfo(*char_ptr, &glyph))
        {
            PrintChar(&glyph, x_pos, y_pos);
            x_pos += Font_GetAdvance(&glyph);
        }

        ++char_ptr;
    }
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static inline bool IsPixelBitSet(uint8_t data, uint8_t bit)
{
    return data & (1 << (7 - bit));
}

static inline void PrintChar(const glyph_info_t *glyph, uint8_t x_base,
                             uint8_t y_base)
{
    sc_assert(glyph != NULL);

    x_base += glyph->horizontal_bearing;

    for (uint8_t y_pos = 0; y_pos < glyph->height; ++y_pos)
    {
        for (uint8_t x_pos = 0; x_pos < glyph->width; ++x_pos)
        {
            uint16_t bit_count = ((uint16_t)y_pos * (uint16_t)glyph->width +
                                  (uint16_t)x_pos);
            uint8_t byte_idx = bit_count / 8;
            uint8_t bit_idx = bit_count % 8;

            if (IsPixelBitSet(pgm_read_byte(&glyph->data[byte_idx]), bit_idx))
            {
                libDisplay_SetPixel(
                    x_pos + x_base,
                    y_pos + y_base - glyph->baseline_offset - glyph->height
                );
            }
        }
    }
}
