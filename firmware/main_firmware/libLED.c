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

#include <avr/io.h>
#include "libDebug.h"
#include "libLED.h"
#include "libTimer.h"

#define LIBNAME "libLED"


//uint32_t LED_timer;

void FlashLED();

///
/// @brief Init the LED library
///
/// @param  None
/// @return None
///
void libLED_Init()
{
	//Set led pin as output and pull it low
	DDRB |= (1 << STATUS_LED);
	PORTB &= ~(1 << STATUS_LED); 
	//LED_timer = libTimer_GetMilliseconds();
	return;
}

void libLED_Toggle()
{
	PORTB ^= (1 << STATUS_LED);
	return;
}



void libLED_Update()
{
	FlashLED();
	return;
}


void FlashLED()
{	
	static uint32_t flash_timer = 0;
	
	if (libTimer_TimeDifference(flash_timer) > 1000)
	{
		libLED_Toggle();
		flash_timer = libTimer_GetMilliseconds();
	}
}

