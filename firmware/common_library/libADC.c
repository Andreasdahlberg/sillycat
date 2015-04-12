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
#include <avr/interrupt.h>
#include "common.h"
#include "libADC.h"
#include "libDebug.h"

#define LIBNAME "libADC"

typedef enum ADCState
{
	IDLE = 0,
	RUNNING,
}ADCState;


static ADCState adc_state;
static uint16_t sample_value = 1023;



void libADC_Init(void)
{
	//Set the prescaler to 128(115 KHz) and enable interupt
	ADCSRA |= ((1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0));
	
	//Set the reference voltage to AREF, select ADC0
	ADMUX = (1 << REFS0);
	
	//Enable the ADC and start a conversion
	ADCSRA |= (1 << ADEN);
	
	DEBUG_STR(LIBNAME, "Init done");
		
}

void libADC_Update(void)
{
	switch (adc_state)
	{
		case IDLE:
			//Start a new conversion
			ADCSRA |= (1 << ADSC);
			adc_state = RUNNING;
			break;
			
		case RUNNING:
			if (ADCSRA & (1 << ADIF))
			{
				sample_value = ADCL;
				sample_value |= (ADCH << 8);
				ADCSRA & ~(1 << ADIF);
				adc_state = IDLE;
			}
			break;
			
		default:
			break;
	}
}


uint16_t libADC_GetSample(void)
{
	return sample_value;
}



function_status libADC_StartConversion(void)
{
	function_status status = BUSY;
	if (adc_state == IDLE)
	{
		ADCSRA |= (1 << ADSC);
		adc_state = RUNNING;
	}
	
	return status;
}




