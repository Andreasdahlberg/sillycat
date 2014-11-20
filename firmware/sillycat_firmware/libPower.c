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
#include "libPower.h"
#include "libDebug.h"
#include "libTimer.h"
#include "libADC.h"

#define LIBNAME "libPower"
#define DEBUG_STR(message) libDebug_PrintString(LIBNAME, message)
#define DEBUG_INT(num)     libDebug_PrintInteger(LIBNAME, num)

#define VREF 3300 // mV
#define R1_RESISTANCE 5600.0
#define R2_RESISTANCE 100000.0

/*
 | VBAT
 |
 -
| | R1 = 5.6 KOhm
| |
 -
 |
 ------- VSENSE
 |
 -
| | R2 = 100 KOhm
| |
 -
 |
 | GND
*/

//TODO: These values must be verified!
//Assuming two NiMh battery cells in series 
#define LOWVOLTAGE 2000
#define CRITICALVOLTAGE 1700

power_state_type power_state;

///
/// @brief Init....
///
/// @param  None
/// @return None
///
void libPower_Init()
{
	//Turn of unused modules
	//ADCSRA = 0x00;
	//PRR = (1 << PRTWI) | (1 << PRTIM0) | (1 << PRTIM2) | (1 << PRADC);

}


void libPower_Update()
{
	static uint32_t update_timer = 0;
		
	switch (power_state)
	{
		case CHARGING:
			break;
		
		case CHARGER_CONNECTED:
			break;
		
		case BATTERY:
			break;
		
		case LOW_VOLTAGE:
			break;
		
		case CRITICAL_VOLTAGE:	
			break;
			
		default:
			break;
	}
		

		
		if (libTimer_TimeDifference(update_timer) > 1000)
		{
			DEBUG_INT(libPower_GetBatteryVoltage());
			update_timer = libTimer_GetMilliseconds();
		}
	
	
		

	return;
}

void libPower_Standby()
{

	
}


uint32_t libPower_GetBatteryVoltage(void)
{
	uint32_t sample = libADC_GetSample();
	uint32_t voltage = (sample * VREF) / 1024;
	
	//Adjust voltage from voltage divider
	return (uint32_t)((float)voltage / (R2_RESISTANCE / (R1_RESISTANCE + R2_RESISTANCE)));
}
