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

#define MOSI		PB3
#define MISO		PB4
#define SCK			PB5

SPI_status spi_status;

uint8_t tx_length;
uint8_t rx_length;
uint8_t transmit_buffer[BUFFER_SIZE];
uint8_t receive_buffer[BUFFER_SIZE];

bool SPIBusy();


///
/// @brief Init the SPI module
///
/// @param  None
/// @return None
///
void libSPI_Init(void)
{	

	DDRB |= ((1 << MOSI)|(1 << SCK)|(1 << PB2));
	DDRB &= ~(1 << MISO);
	
	PORTB |= (1 << MOSI);
	PORTB &= ~(1 << SCK);
	
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
	spi_status = IDLE;
	
	DEBUG_STR("Init done");
	return;
}

//TODO: Implement timeout for read operations, maybe for write also?

///
/// @brief Update the internal state
///
/// @param  None
/// @return None
///
void libSPI_Update(void)
{
	static uint8_t byte_counter = 0;
	
	switch(spi_status)
	{
		case IDLE:
			break;
		
		//TODO: Remove the DONE-states?
		case READ_DONE:
			byte_counter = 0;
			spi_status = IDLE;
			break;
		
		case READ:
			if (byte_counter < rx_length)
			{
				if (!SPIBusy())
				{
					receive_buffer[byte_counter] = SPDR;
					SPDR = 0x00; //Put dummy data in the SPI data register to enable SPI clock 
					++byte_counter;
				}
			}
			else if (byte_counter == tx_length && !SPIBusy())
			{
				spi_status = READ_DONE;
			}
			break;
		
		case WRITE:
			if (byte_counter < tx_length)
			{
				if (!SPIBusy() || byte_counter == 0)
				{
					SPDR = transmit_buffer[byte_counter];
					++byte_counter;
				}
			}
			else if (byte_counter == tx_length && !SPIBusy())
			{
				spi_status = WRITE_DONE;
			}	
			break;
		
		case WRITE_DONE:
			byte_counter = 0;
			spi_status = IDLE;
			break;
		
		default:
			DEBUG_STR("Fatal error, reinitializing module...");
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
	bool status = FALSE;
	if (libSPI_GetStatus() == IDLE && length <= BUFFER_SIZE)
	{
		memcpy(transmit_buffer, data_bytes, length);
		tx_length = length;
		spi_status = WRITE;
		status = TRUE;
	}
	return status;
}

//TODO: Change the behavior of the read function. The first time read is called during a read cycle a SPI read transfer should be
//      started and false should be returned. The following calls should return false and do nothing until the transfer is complete
//      and the rx_buffer is ready. Then read should return true.  

///
/// @brief Start reading data from the SPI-bus
///
/// @param  data_bytes Pointer to buffer where the data will be stored
/// @param  length Length of the expected data
/// @param  timeout Timeout in ms, time before read is aborted if slave is not responding //TODO: Fix this description!
/// @return bool Status of the operation
///
bool libSPI_Read(const uint8_t* data_bytes, uint8_t length, uint8_t timeout)
{
	bool status = FALSE;
	if (libSPI_GetStatus() == IDLE && length <= BUFFER_SIZE)
	{
		memset(receive_buffer, 0, BUFFER_SIZE);
		rx_length = length;
		SPDR = 0x00; //Put dummy data in the SPI data register to enable SPI clock 
		spi_status = READ;
		status = TRUE;
	}
	return status;
}


///
/// @brief Get the status of the SPI-bus
///
/// @param None
/// @return SPI_status SPI-bus status
///
SPI_status libSPI_GetStatus(void)
{
	return spi_status;
}


///
/// @brief Set the SPI transfer mode
///
/// @param mode SPI mode, (0-3)
/// @return bool Status of operation
///
bool libSPI_SetMode(uint8_t mode)
{
	//TODO: Implement this function
	switch (mode)
	{
		case 0:
			break;
		case 1:
			break;
		case 2:
			break;
		case 3:
			break;
		default:
			break;
	}
	
	return TRUE;
}


//**********************Local functions**********************//


///
/// @brief Check if the SPI-bus is busy
///
/// @param None
/// @return bool Status of SPI-bus
///
bool SPIBusy()
{
	return ((SPSR & (1<<SPIF)) == 0);
}
