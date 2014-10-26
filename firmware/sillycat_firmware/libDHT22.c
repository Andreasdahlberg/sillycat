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
#include <string.h>
#include "libdht22.h"
#include "libDebug.h"
#include "libTimer.h"

#define LIBNAME "libDHT22"
#define DATAPIN PB6
#define MEASURE_INTERVAL 2000 //Time between sensor readings, must be atleast 2000 ms
#define PULSE_TIMEOUT 35      //Must be shorter then 127 ms to prevent overflow
#define TIMINGS_ARRAY_SIZE 85
#define LOW_HIGH_LIMIT 10


typedef enum
{
	DHT_UNINITIALIZED = 0,
	DHT_POWERUP,
	DHT_READING,
	DHT_DECODING,
} dht_status;


dht_status libDHT22_status = DHT_UNINITIALIZED;

uint32_t init_time;
uint8_t data[6];

float hum;
float temp;

void GetPulseTimings(int8_t* timings);
void RequestReading();
float ConvertToFloat(uint8_t integral, uint8_t fractional);
void DecodeTimings(int8_t* timings);
uint8_t IsDataValid();
int32_t GetPulse(uint8_t pin, uint32_t timeout);


void libDHT22_Init(void)
{
	libDebug_PrintString(LIBNAME, "Init library");
	
	DDRB |= (1 << DATAPIN);
	PORTB |= (1 << DATAPIN);
	
	libDHT22_status = DHT_POWERUP;
	init_time = libTimer_GetMilliseconds();
}


void libDHT22_Update(void)
{

	static int8_t timings[TIMINGS_ARRAY_SIZE];
	

	switch (libDHT22_status)
	{

		case DHT_POWERUP:
		//TODO: Add special case for timer rollover
		if (libTimer_TimeDifference(init_time) > MEASURE_INTERVAL)
		{
			libDebug_PrintString(LIBNAME, "DHT22 ready");
			libDHT22_status = DHT_READING;

			//Init timings with -1
			memset(timings, -1, (sizeof(int8_t) * TIMINGS_ARRAY_SIZE));
		}
		break;

		case DHT_READING:
		RequestReading();
		GetPulseTimings(timings);
		libDHT22_status = DHT_DECODING;
		break;
		

		case DHT_DECODING:

		//Decode pulse timings into data
		DecodeTimings(timings);

		
		//Convert data into humidity
		hum = ConvertToFloat(data[0], data[1]);

		//Convert data into temperature
		temp = ConvertToFloat(data[2], data[3]);
		
		if (IsDataValid())
		{
			libDebug_PrintString(LIBNAME, "Data valid");
		}
		else
		{
			libDebug_PrintString(LIBNAME, "Data invalid");
		}

		libDebug_PrintInteger(LIBNAME, (uint32_t)temp);	
	/*
		Serial.print("Humidity: ");
		Serial.println(hum, DEC);
		Serial.print("Temperatur: ");
		Serial.println(temp, DEC);
		*/

		init_time = libTimer_GetMilliseconds();
		libDHT22_status = DHT_POWERUP;
		break;

		case DHT_UNINITIALIZED:
		default:
		libDHT22_Init();
		return;
	}

}



//**********************Local functions**********************//

///
/// @brief Get all pulse timings
///
/// @param  timings Pointer to array where all timings will be saved
/// @return None
///
void GetPulseTimings(int8_t* timings)
{
	uint8_t  cnt = 0;
	//Save all pulse timings
	while(cnt < TIMINGS_ARRAY_SIZE)
	{
		timings[cnt] = GetPulse(DATAPIN, PULSE_TIMEOUT);
		if (timings[cnt] == -1)
		{
			break;
		}
		++cnt;
	}
	//Serial.println(cnt);
}

///
/// @brief Request a new reading from the DHT22 sensor
///
/// @param  None
/// @return None
///
void RequestReading()
{
	//Pull data pin low for minimum 1 ms to start communicating with the DHT22 sensor	
	DDRB |= (1 << DATAPIN);
	PORTB &= ~(1 << DATAPIN);
	_delay_ms(2);

	PORTB |= (1 << DATAPIN);
	_delay_us(25);
	
	DDRB &= ~(1 << DATAPIN);
	PORTB |= (1 << DATAPIN);
}


///
/// @brief Convert integral and fractional part into float
///
/// @param  intergral The integral part of the float
/// @param  fractional The fractional part of the float
/// @return float The resulting float
///
float ConvertToFloat(uint8_t integral, uint8_t fractional)
{
	float data = (integral << 8);
	data += fractional;
	data /= 10;
	return data;
}


///
/// @brief  Decode pulse timings and convert them into usable data
///
/// @param  timings Pointer to an array with all pulse timings
/// @return None
///
void DecodeTimings(int8_t* timings)
{
	uint8_t data_pulse = 0;
	uint8_t byte_cnt = 0;
	uint8_t bit = 0;
	uint8_t cnt;

	memset(data, 0, sizeof(uint8_t)*6);

	for (cnt = 0; cnt < 80; ++cnt)
	{
		//Toggle data_pulse since every other pulse is a bit
		if (data_pulse == 0)
		{
			data_pulse = 1;
		}
		else if (data_pulse == 1)
		{
			//libDebug_PrintUnsignedInteger(LIBNAME, timings[cnt+2]);
			if (timings[cnt+2] > LOW_HIGH_LIMIT)	//Add two to Skip the two first pulses
			{
				data[byte_cnt] |= (1 << (7-bit));
			}
			data_pulse = 0;
			++bit;
			if (bit == 8)
			{
				++byte_cnt;
				bit = 0;
			}

		}
	}
}


///
/// @brief Check if the sensor data is valid
///
/// @param  None
/// @return float The resulting float
///
uint8_t IsDataValid()
{
	return (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF));
}

///
/// @brief Get timings for the next pulse
///
/// @param  pin Pin to monitor for next pulse
/// @param  timeout Time to wait on pulse before timeout
/// @return int32_t Lenght of the pulse in ms
///
int32_t GetPulse(uint8_t pin, uint32_t timeout)
{
	uint8_t start_value = PINB & (1 << pin) ;
	int32_t pulse_time = 0;

	while ((PINB & (1 << pin)) == start_value)
	{
		++pulse_time;
		_delay_us(1);

		if (pulse_time > timeout)
		{
			return -1;
		}
	}
	return pulse_time;
}
