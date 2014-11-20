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

#define F_CPU 8000000UL // 8 MHz

#include <avr/io.h>
#include <util/delay.h>
#include "libDebug.h"
#include "libLED.h"
#include "libTimer.h"
#include "libDHT22.h"
#include "libADC.h"
#include "libPower.h"

#define LIBNAME "Main"
#define DEBUG_STR(message) libDebug_PrintString(LIBNAME, message)
#define DEBUG_INT(num)     libDebug_PrintInteger(LIBNAME, num)


int main(void)
{

	libDebug_Init();
		DEBUG_STR("Init system");
	
	libADC_Init();
	libPower_Init();  
	libTimer_Init();
	libLED_Init();
	libDHT22_Init();		
	
    while(1)
    {
		
	
		
		//libDebug_PrintString(LIBNAME, "I love silly cats");
		//libDebug_PrintUnsignedInteger(LIBNAME, libTimer_GetSeconds());
		libADC_Update();
		libPower_Update();
		libLED_Update();
		libDHT22_Update();
    }
}