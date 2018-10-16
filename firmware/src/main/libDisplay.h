/**
 * @file   libDisplay.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-10-16 (Last edit)
 * @brief  Implementation of display-library.
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

#ifndef LIBDISPLAY_H_
#define LIBDISPLAY_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

/**
 * Initialize the display.
 *
 * The VRAM will be cleared.
 */
void libDisplay_Init(void);

/**
 * Turn the display on.
 */
void libDisplay_On(void);

/**
 * Turn the display off.
 */
void libDisplay_Off(void);

/**
 * Clear the display.
 *
 * VRAM is cleared and flushed to the display.
 */
void libDisplay_Clear(void);

/**
 * Clear VRAM.
 */
void libDisplay_ClearVRAM(void);

/**
 * Flush the VRAM to the display.
 */
void libDisplay_Flush(void);

/**
 * Reset the display.
 *
 * The display is set to it's initial state and the VRAM is cleared.
 */
void libDisplay_Reset(void);

/**
 * Set a specific pixel on the display.
 *
 * @param x Pixel coordinate on the x-axis.
 * @param y Pixel coordinate on the y-axis.
 */
void libDisplay_SetPixel(uint8_t x, uint8_t y);

/**
 * Set the display brightness.
 *
 * @param brightness Brightness to set.
 */
void libDisplay_SetBrightness(uint8_t brightness);

/**
 * Rotate display contents.
 *
 * The VRAM is cleared after the rotation.
 *
 * @param state Set to true if contents should be rotated.
 */
void libDisplay_Rotate(bool state);

#ifdef DEBUG_ENABLE
/**
 * Dump the data in VRAM to debug UART.
 */
void libDisplay_DumpVRAMToUART(void);
#endif

#endif
