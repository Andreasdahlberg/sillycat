/**
 * @file   libDisplay.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2016-04-03 (Last edit)
 * @brief  Header of display-library.
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

#ifndef LIBDISPLAY_H_
#define LIBDISPLAY_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

void libDisplay_Init(void);
void libDisplay_On(void);
void libDisplay_Off(void);
void libDisplay_Clear(void);
void libDisplay_ClearVRAM(void);
void libDisplay_Reset(void);
void libDisplay_Flush(void);
void libDisplay_SetPixel(uint8_t x, uint8_t y);
void libDisplay_SetBrightness(uint8_t brightness);
void libDisplay_Rotate(bool state);

#ifdef DEBUG_ENABLE
void libDisplay_DumpVRAMToUART(void);
#endif

#endif /* LIBDISPLAY_H_ */
