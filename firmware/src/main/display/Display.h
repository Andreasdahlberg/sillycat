/**
 * @file   Display.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2021-02-03 (Last edit)
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

#ifndef DISPLAY_H_
#define DISPLAY_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stdbool.h>

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

/**
 * Initialize the display.
 *
 * The VRAM will be cleared.
 */
void Display_Init(void);

/**
 * Turn the display on.
 */
void Display_On(void);

/**
 * Turn the display off.
 */
void Display_Off(void);

/**
 * Clear the display.
 *
 * VRAM is cleared and flushed to the display.
 */
void Display_Clear(void);

/**
 * Clear VRAM.
 */
void Display_ClearVRAM(void);

/**
 * Flush the VRAM to the display.
 */
void Display_Flush(void);

/**
 * Reset the display.
 *
 * The display is set to it's initial state and the VRAM is cleared.
 */
void Display_Reset(void);

/**
 * Set a specific pixel on the display.
 *
 * @param x Pixel coordinate on the x-axis.
 * @param y Pixel coordinate on the y-axis.
 */
void Display_SetPixel(uint8_t x, uint8_t y);

/**
 * Set the display brightness.
 *
 * @param brightness Brightness to set.
 */
void Display_SetBrightness(uint8_t brightness);

/**
 * Rotate display contents.
 *
 * The VRAM is cleared after the rotation.
 *
 * @param state Set to true if contents should be rotated.
 */
void Display_Rotate(bool state);

#ifdef DEBUG_ENABLE
/**
 * Dump the data in VRAM to debug UART.
 */
void Display_DumpVRAMToUART(void);
#endif

#endif
