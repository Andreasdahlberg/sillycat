/**
 * @file   libUI.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2016-03-08 (Last edit)
 * @brief  Implementation of UI-library.
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

#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "libDisplay.h"
#include "libUI.h"

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

static FONT_INFO *current_font;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void libUI_Update(void)
{
    libDisplay_Flush();
    return;
}

void libUI_SetFont(FONT_INFO *font)
{
    current_font = font;
    return;
}

void libUI_DrawLine(uint8_t x_start, uint8_t y_start, uint8_t x_end,
                    uint8_t y_end)
{
    //TODO: Fix this function, does not follow code style!
    int dx = abs(x_end - x_start), sx = x_start < x_end ? 1 : -1;
    int dy = abs(y_end - y_start), sy = y_start < y_end ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;

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
    return;
}

void libUI_DrawRectangle(uint8_t x_start, uint8_t y_start, uint8_t width,
                         uint8_t height)
{
    libUI_DrawLine(x_start, y_start, x_start + width, y_start);
    libUI_DrawLine(x_start, y_start, x_start, y_start + height);
    libUI_DrawLine(x_start, y_start + height, x_start + width, y_start + height);
    libUI_DrawLine(x_start + width, y_start, x_start + width, y_start + height);
    return;
}

void libUI_DrawCircle(uint8_t x_pos, uint8_t y_pos, uint8_t radius)
{
    int x = radius;
    int y = 0;
    int decisionOver2 = 1 -
                        x;   // Decision criterion divided by 2 evaluated at x=r, y=0

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
    return;
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
    buffer[sizeof(buffer) - 1] = '\n';

    libUI_PrintText(buffer, x_pos, y_pos);
    va_end(args);
    return;
}

void libUI_PrintText(const char *buffer, uint8_t x_pos, uint8_t y_pos)
{
    uint8_t desc_offset;
    uint16_t char_offset;
    uint8_t byte_index;
    uint8_t bit_index;
    uint8_t buffer_index;
    uint8_t data_row = 0;
    uint8_t data_column = 0;

    //TODO: Fix this very bad code!
    for (buffer_index = 0; buffer_index < strlen(buffer); ++buffer_index)
    {
        if ((char)buffer[buffer_index] != ' ')
        {
            desc_offset = (buffer[buffer_index] - (char)current_font->startChar);
            char_offset = pgm_read_word(&current_font->charInfo[desc_offset].offset);

            for (data_row = 0; data_row < current_font->heightPages; ++data_row)
            {
                for (data_column = 0; data_column < current_font->widthPages; ++data_column)
                {
                    byte_index = (data_row * current_font->widthPages) + data_column;
                    for (bit_index = 0; bit_index < 8; ++bit_index)
                    {
                        if ((pgm_read_byte(&(current_font->data[char_offset + byte_index])) & (1 <<
                                (7 - bit_index))) > 0)
                        {
                            libDisplay_SetPixel(x_pos + bit_index + (data_column << 3), y_pos + data_row);
                        }
                    }
                }
            }
            x_pos += (pgm_read_byte(&current_font->charInfo[desc_offset].widthBits) + 2);
        }
        else
        {
            x_pos += 6;
        }
    }
    return;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////
