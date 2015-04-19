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

//TODO: Define this in one place
#define F_CPU 8000000UL // 8 MHz

#include <util/delay.h>
#include "common.h"
#include "libNHD223.h"
#include "libSPI.h"
#include "libDebug.h"

#define LIBNAME "libNHD223"

#define ENABLE		DDC1
#define RESET		DDC2
#define READWRITE	DDC3
#define REGSELECT	DDC4
#define CHIPSELECT	DDC5

typedef enum
{
	INPUT = 0,
	OUTPUT
} data_direction_type;

static void selectDevice(bool state);
static void enableDataLatch(bool state);
static void enableCommandMode(bool state);
static void enableControlPin(bool state, uint8_t pin_index);
static void enableReset(bool state);
void setDataDirection(data_direction_type direction);


void libNHD223_Init()
{
	//Configure data pins as outputs and pull low
	DDRD = 0xFF;
	PORTD = 0x00;
	
	DDRC |= ((1 << ENABLE) | (1 << RESET) | (1 << READWRITE) |
			 (1 << REGSELECT) | (1 << CHIPSELECT));
			 
	enableReset(TRUE);
	_delay_us(5);
	enableReset(FALSE);
			 
	selectDevice(FALSE);
	enableDataLatch(FALSE);
}

void libNHD223_Update()
{
	libNHD223_WriteByte(0xAF);
	
	
}


bool libNHD223_WriteCommand(uint8_t command)
{
	
}


bool libNHD223_WriteByte(uint8_t data)
{
	setDataDirection(OUTPUT);
	
	//Set write mode
	enableControlPin(FALSE, READWRITE);
	
	//Write data
	enableCommandMode(TRUE);
	
	selectDevice(TRUE);
	PORTD = data;
		_delay_ms(500);
	enableDataLatch(TRUE);
	_delay_ms(50);
	enableDataLatch(FALSE);
	selectDevice(FALSE);
	
	return TRUE;
}

bool libNHD223_ReadByte(uint8_t *data)
{
	setDataDirection(INPUT);
	
	//Set read mode
	enableControlPin(TRUE, READWRITE);
	

	selectDevice(TRUE);
	*data = PINC;
	enableDataLatch(TRUE);
	enableDataLatch(FALSE);
	selectDevice(FALSE);
	
	return TRUE;
}

void setDataDirection(data_direction_type direction)
{
	if (direction == INPUT)
	{
		DDRD = 0x00;
	}
	else
	{
		//Output
		DDRD = 0xFF;
		PORTD = 0x00;
	}
}

static void enableCommandMode(bool state)
{
	//0: Command, 1: Data
	enableControlPin(!state, REGSELECT);
}


static void enableDataLatch(bool state)
{
	enableControlPin(state, ENABLE);
}

static void enableReset(bool state)
{
	//Reset is active low
	enableControlPin(!state, RESET);
}


static void selectDevice(bool state)
{
	//CS is active low
	enableControlPin(!state, CHIPSELECT);
}


static void enableControlPin(bool state, uint8_t pin_index)
{
	if (state == FALSE)
	{
		PORTC &= ~(1 << pin_index);
	}
	else
	{
		PORTC |= (1 << pin_index);
	}
}

