/**
 * @file   mock_libDisplay.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-09-22 (Last edit)
 * @brief  Mock functions for libDisplay module.
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

#ifndef WRAP_LIBDISPLAY_H_
#define WRAP_LIBDISPLAY_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include <stdbool.h>

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

void __wrap_libDisplay_Init(void);
void __wrap_libDisplay_On(void);
void __wrap_libDisplay_Off(void);
void __wrap_libDisplay_Clear(void);
void __wrap_libDisplay_ClearVRAM(void);
void __wrap_libDisplay_Reset(void);
void __wrap_libDisplay_Flush(void);
void __wrap_libDisplay_SetPixel(uint8_t x, uint8_t y);
void __wrap_libDisplay_SetBrightness(uint8_t brightness);
void __wrap_libDisplay_Rotate(bool state);

#endif
