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

void libDisplay_Init();
void libDisplay_On();
void libDisplay_Off();
void libDisplay_Clear();
static void libDisplay_ClearVRAM();
void libDisplay_Reset();
void libDisplay_Flush();
void libDisplay_SetPixel(uint8_t x, uint8_t y);
void libDisplay_TEST();


#endif /* LIBDISPLAY_H_ */