/**
 * @file   libSensor.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2015-05-14 (Last edit)
 * @brief  Implementation of Sensor-library.
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

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "common.h"
#include "libADC.h"
#include "libSensor.h"
#include "libDebug.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define TABLE_OFFSET -30
#define TABLE_LENGTH 29

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static const uint16_t mf52_table[TABLE_LENGTH] PROGMEM = {
	54, //-30
	72, //-25
	95, //-20
	122, //-15
	156, //-10
	194, //-5
	244, //0
	288, //5
	342, //10
	398, //15
	455, //20
	512, //25
	568, //30
	620, //35
	669, //40
	714, //45
	754, //50
	790, //55
	821, //60
	849, //65
	873, //70
	893, //75
	911, //80
	926, //85
	939, //90
	950, //95
	960, //100
	968, //105
	975, //110
};

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

uint16_t RawValueToTemperature(uint16_t raw_value);

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void libSensor_Init()
{
	libADC_EnableInput(LIBSENSOR_EXTERNAL_LIGHT, TRUE);
	libADC_EnableInput(LIBSENSOR_EXTERNAL_TEMPERATURE, TRUE);
	libADC_EnableInput(LIBSENSOR_INTERNAL_TEMPERATURE, TRUE);	
}

function_status libSensor_GetSensorValue(uint8_t sensor, uint16_t *sensor_value)
{
	function_status status = ERROR;
	uint32_t tmp_value;
	
	if(libADC_GetSample(sensor, sensor_value) == SUCCESS)
	{
		switch(sensor)
		{
			case LIBSENSOR_EXTERNAL_LIGHT:
				//Using raw value, do nothing
				status = SUCCESS;			
				break;
				
			case LIBSENSOR_EXTERNAL_TEMPERATURE:
				*sensor_value = RawValueToTemperature(*sensor_value);
				status = SUCCESS;			
				break;
	
			case LIBSENSOR_INTERNAL_TEMPERATURE:
				tmp_value = *sensor_value;
				tmp_value *= 3133;
				tmp_value -= -280277;
				*sensor_value = (uint16_t)tmp_value / 1000;
				status = SUCCESS;
				break;
	
			default:
				WARNING("Unknown sensor type");				
				break;
		}		
	}
	return status;	
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

uint16_t RawValueToTemperature(uint16_t raw_value)
{
	uint8_t index = 0;
	uint32_t tmp;
	
	while(index < TABLE_LENGTH && pgm_read_byte(&mf52_table[index]) < raw_value)
	{
		++index;
	}
	
	//TODO: Can this be solved in a better way?
	tmp = 100 * (uint32_t)raw_value;
	tmp /= pgm_read_byte(&mf52_table[index]);
	tmp *= (TABLE_OFFSET + (index * 5));
	tmp /= 10;

	return  (uint16_t)tmp;
}