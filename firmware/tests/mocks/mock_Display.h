/**
 * @file   mock_Display.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2021-02-03 (Last edit)
 * @brief  Mock functions for Display module.
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

#ifndef WRAP_DISPLAY_H_
#define WRAP_DISPLAY_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include <stdbool.h>
#include <stdint.h>

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

void __wrap_Display_Init(void);
void __wrap_Display_On(void);
void __wrap_Display_Off(void);
void __wrap_Display_Clear(void);
void __wrap_Display_ClearVRAM(void);
void __wrap_Display_Reset(void);
void __wrap_Display_Flush(void);
void __wrap_Display_SetPixel(uint8_t x, uint8_t y);
void __wrap_Display_SetBrightness(uint8_t brightness);
void __wrap_Display_Rotate(bool state);

#endif
