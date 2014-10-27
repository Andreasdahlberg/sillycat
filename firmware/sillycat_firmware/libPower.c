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
#include <avr/sleep.h>
#include "libDebug.h"
#include "libTimer.h"

#define LIBNAME "libPower"


///
/// @brief Init....
///
/// @param  None
/// @return None
///
void libPower_Init()
{
	//Turn of unused modules
	ADCSRA = 0x00;
	PRR = (1 << PRTWI) | (1 << PRTIM0) | (1 << PRTIM2) | (1 << PRADC);

}


void libPower_Update()
{

	return;
}

void libPower_Standby()
{

	
	
}



