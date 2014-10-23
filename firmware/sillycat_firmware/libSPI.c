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
#include <stdio.h>
#include <string.h>

#include "libDebug.h"
#include "libSPI.h"

#define LIBNAME "libSPI"
#define BUFFER_SIZE 48

#define MEM_SS		PD2
#define RTC_SS		PD3
#define RADIO_SS	PB2
#define MOSI		PB3
#define MISO		PB4
#define SCK			PB5

uint8_t active_slave;
SPI_status spi_status;

uint8_t transmit_buffer[BUFFER_SIZE];
uint8_t receive_buffer[BUFFER_SIZE];




void libSPI_Init(void)
{	
	//Set outputs/inputs
	DDRD |= ((1 << MEM_SS)|(1 << RTC_SS));
	PORTD |= ((1 << MEM_SS)|(1 << RTC_SS));
	DDRB |= ((1 << MOSI)|(1 << RADIO_SS)|(1 << SCK));
	DDRB &= ~(1 << MISO);
	PORTB |= ((1 << MOSI)|(1 << RADIO_SS));
	PORTB &= ~(1 << SCK);
	
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
	active_slave = RTC_SS;
	spi_status = IDLE;
	
	libDebug_PrintString(LIBNAME, "Init done");
	return;
}


void libSPI_Service(void)
{
	static uint8_t byte_counter = 0;
	
	switch(spi_status)
	{
		case IDLE:
			break;
		
		case RECEIVE:
			break;
		
		case RECEIVE_DONE:
			break;
		
		case TRANSMIT:
			
			if (byte_counter == 0)
			{
				SPDR = transmit_buffer[byte_counter];
				++byte_counter;
			}
			else if ((SPSR & (1 << SPIF)) > 0)
			{
				
			}
		
			spi_status = TRANSMIT_DONE;
	
			break;
		
		case TRANSMIT_DONE:
		break;
		
		default:
		libDebug_PrintString(LIBNAME, "Some clever message");
		libSPI_Init();
		break;
		
	}

	return;
}


uint8_t libSPI_TransmitBytes(const uint8_t* data_bytes, uint8_t length)
{
	int8_t return_status;
	if (libSPI_GetStatus() == IDLE && length <= BUFFER_SIZE) //NOTE: Length?
	{
		memcpy(transmit_buffer, data_bytes, length);
		spi_status = TRANSMIT;
		return_status = 1;
	}
	else
	{
		return_status = 0;
	}
	return return_status;	
}


uint8_t libSPI_TransmitByte(uint8_t data_byte)
{
	int8_t return_status;
	if (libSPI_GetStatus() == IDLE)
	{
		SPDR = data_byte;
		spi_status = TRANSMIT;
		return_status = 1;
	}
	else
	{
		return_status = 0;
	}
	
	return return_status;
}

SPI_status libSPI_GetStatus(void)
{
	return spi_status;
}


void libSPI_SetActiveSlave(uint8_t slave_unit)
{
	
	return;
}

