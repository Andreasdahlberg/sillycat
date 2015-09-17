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


#ifndef COMMON_H_
#define COMMON_H_


#include <stdint.h>
#include <stddef.h>

typedef enum bool
{	
	FALSE = 0,
	TRUE
}bool;


typedef enum function_status
{
	ERROR = 0,
	SUCCESS,
	BUSY,
	TIMEOUT
}function_status;


void SetBit(uint8_t bit_index, bool state, uint8_t *data);
uint8_t BCDToDecimal(uint8_t bcd_data);
uint16_t DecimalToBCD(uint8_t decimal_data);
int GetFreeRam();

#endif /* COMMON_H_ */