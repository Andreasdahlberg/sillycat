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
along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef LIBUART_H_
#define LIBUART_H_

#include <stdint.h>

#define UBRRn 12		//Baud:38.4k Fosc: 8 MHz Error: 0.2%

void libUART_Init(void);
void libUART_SendByte(uint8_t data);
void libUART_SendArray(uint8_t* data, uint8_t len);

#endif /* LIBUART_H_ */