/**
 * @file   libUI.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2015-11-18 (Last edit)
 * @brief  Header of UI-library.
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

#ifndef LIBUI_H_
#define LIBUI_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include "bitmap_db.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define libUI_Print(text, x_pos, y_pos, ...) libUI_Print_P(PSTR(text), x_pos, y_pos, ##__VA_ARGS__)

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

void libUI_Update(void);
void libUI_SetFont(FONT_INFO *font);
void libUI_PrintText(const char *buffer, uint8_t x_pos, uint8_t y_pos);
void libUI_Print_P(const char *text, uint8_t x_pos, uint8_t y_pos, ...);
void libUI_DrawLine(uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end);
void libUI_DrawRectangle(uint8_t x_start, uint8_t y_start, uint8_t width, uint8_t height);
void libUI_DrawCircle(uint8_t x_pos, uint8_t y_pos, uint8_t radius);

#endif /* LIBUI_H_ */