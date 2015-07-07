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

#include "common.h"


uint8_t BCDToDecimal(uint8_t bcd_data)
{
	uint8_t data;
		
	data = ((bcd_data & 0xF0)>>4)*10;
	data += (bcd_data & 0x0F);
	
	return data;
}

uint16_t DecimalToBCD(uint8_t decimal_data)
{
	uint16_t data = 0;
	uint8_t shift = 0;
	
	do
	{
		data |= (decimal_data % 10) << shift;
		shift += 4;
		decimal_data = decimal_data / 10;
	}
	while (decimal_data > 0);
	
	
	return data;
}

void setBit(uint8_t bit_index, bool state, uint8_t *data)
{
	if (state == TRUE)
	{
		
		*data |= (1 << bit_index);
	}
	else
	{
		*data &= ~(1 << bit_index);
	}
}

int GetFreeRam()
{
	extern int __heap_start, *__brkval;
	int v;
	return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
