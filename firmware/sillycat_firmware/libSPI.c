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
#define BUFFER_SIZE 8

#define DEBUG_STR(message) libDebug_PrintString(LIBNAME, message)
#define DEBUG_INT(num)     libDebug_PrintInteger(LIBNAME, num)


//TODO: Move defines for SS pins to header?
#define MEM_SS		PD2
#define RTC_SS		PD3
#define RADIO_SS	PB2
#define MOSI		PB3
#define MISO		PB4
#define SCK			PB5

uint8_t active_slave;
SPI_status spi_status;

uint8_t tx_length;
uint8_t rx_length;
uint8_t transmit_buffer[BUFFER_SIZE];
uint8_t receive_buffer[BUFFER_SIZE];

bool TXIsBusy();


///
/// @brief Init....
///
/// @param  None
/// @return None
///
void libSPI_Init(void)
{	
	//Set outputs/inputs
	//DDRD |= ((1 << MEM_SS)|(1 << RTC_SS));
	//PORTD |= ((1 << MEM_SS)|(1 << RTC_SS));
	//DDRB |= ((1 << MOSI)|(1 << RADIO_SS)|(1 << SCK));
	//PORTB |= ((1 << MOSI)|(1 << RADIO_SS));
	
	DDRB |= ((1 << MOSI)|(1 << SCK)|(1 << RADIO_SS));
	DDRB &= ~(1 << MISO);
	
	PORTB |= (1 << MOSI);
	PORTB &= ~(1 << SCK);
	
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
	spi_status = IDLE;
	
	DEBUG_STR("Init done");
	return;
}


void libSPI_Update(void)
{
	static uint8_t byte_counter = 0;
	
	switch(spi_status)
	{
		case IDLE:
			break;
		
		case READ:
			break;
		
		case READ_DONE:
			break;
		
		case WRITE:
			if (byte_counter < tx_length)
			{
				if (!TXIsBusy() || byte_counter == 0)
				{
					DEBUG_INT(byte_counter);
					SPDR = transmit_buffer[byte_counter];
					++byte_counter;
				}
			}
			else if (byte_counter == tx_length && !TXIsBusy())
			{
				spi_status = WRITE_DONE;
			}	
			break;
		
		case WRITE_DONE:
			DEBUG_STR("TX done!");
			byte_counter = 0;
			spi_status = IDLE;
			break;
		
		default:
			DEBUG_STR("Some clever message");
			libSPI_Init();
			break;
		
	}

	return;
}


///
/// @brief Start writing data to the SPI-bus
///
/// @param  data_bytes Pointer to buffer with data to write
/// @param length Length of the data buffer
/// @return bool Status of the operation
///
bool libSPI_Write(const uint8_t* data_bytes, uint8_t length)
{
	bool return_status = FALSE;
	if (libSPI_GetStatus() == IDLE && length <= BUFFER_SIZE) //NOTE: Length?
	{
		memcpy(transmit_buffer, data_bytes, length);
		tx_length = length;
		spi_status = WRITE;
		return_status = TRUE;
	}
	return return_status;
}

///
/// @brief Start reading data from the SPI-bus
///
/// @param  data_bytes Pointer to buffer where the data will be stored
/// @param  length Length of the expected data
/// @param  timeout Timeout in ms, time before read is aborted if slave is not respondig //TODO: Fix this description!
/// @return bool Status of the operation
///
bool libSPI_Read(const uint8_t* data_bytes, uint8_t length, uint8_t timeout)
{
	bool return_status = FALSE;
	if (libSPI_GetStatus() == IDLE && length <= BUFFER_SIZE) //NOTE: Length?
	{
		memset(receive_buffer, 0, BUFFER_SIZE);
		rx_length = length;
		spi_status = READ;
		return_status = TRUE;
	}
	return return_status;
}


SPI_status libSPI_GetStatus(void)
{
	return spi_status;
}


bool libSPI_SetActiveSlave(uint8_t port, uint8_t pin)
{
	bool status = FALSE;
	if (libSPI_GetStatus() == IDLE)
	{

		status = TRUE;
	} 
	return status;
}

bool libSPI_SetMode()
{
	
	return TRUE;
}

//**********************Local functions**********************//

bool TXIsBusy()
{
	return ((SPSR & (1<<SPIF)) == 0);
}
