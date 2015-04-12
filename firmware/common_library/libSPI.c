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

#define SS		DDB2
#define MOSI	DDB3
#define MISO	DDB4
#define SCK		DDB5

SPI_status spi_status;

uint8_t tx_length;
uint8_t rx_length;
uint8_t transmit_buffer[BUFFER_SIZE];
uint8_t receive_buffer[BUFFER_SIZE];
bool slave_active;

bool SPIBusy();

//TODO: Create functions for changing data order, clock freq and to enable/disable SPI

///
/// @brief Init the SPI module
///
/// @param  None
/// @return None
///
void libSPI_Init(uint8_t spi_mode)
{	
	//Always set SS as output even if not used, otherwise the device can't 
	//act as master.
	DDRB |= ((1 << MOSI)|(1 << SCK)|(1 << SS));
	DDRB &= ~(1 << MISO);	

	//SPCR = 0x00;
	SPCR = (1<<SPR0);
	
	libSPI_SetMaster(TRUE);
	
	if (libSPI_SetMode(spi_mode) == TRUE)
	{
		SPCR |= (1<<SPE);
		spi_status = IDLE;
	
		DEBUG_STR(LIBNAME, "Init done");
	}
	else
	{
		DEBUG_STR(LIBNAME, "Failed to init SPI, invalid mode");
		DEBUG_INT(LIBNAME, spi_mode);
	}
	
	slave_active = FALSE;
	return;
}


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
		
		case READ_DONE:
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
				byte_counter = 0;
				spi_status = READ_DONE;
			}
			break;
		
		case WRITE:
			if (byte_counter < tx_length)
			{
				if (!SPIBusy() || byte_counter == 0)
				{
					DEBUG_STR(LIBNAME, "Sending byte");
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
			DEBUG_STR(LIBNAME, "Fatal error, reinitializing module...");
			//libSPI_Init();
			break;
		
	}
	return;
}


void libSPI_WriteByte(uint8_t data_byte)
{
	SPDR = data_byte;
	while(!(SPSR & (1<<SPIF)))
	{	
	}
	return;
}


void libSPI_ReadByte(uint8_t *data_byte)
{
	SPDR = 0x00; //Dummy data to enable SPI-clock
	while(!(SPSR & (1<<SPIF)))
	{
	}
	*data_byte = SPDR;
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
	
	DEBUG_STR(LIBNAME, "Write started");
	
	if (libSPI_GetStatus() == IDLE && length <= BUFFER_SIZE)
	{
		memcpy(transmit_buffer, data_bytes, length);
		tx_length = length;
		spi_status = WRITE;
		status = TRUE;
	}
	return status;
}


//TODO: Rewrite function description
///
/// @brief Start reading data from the SPI-bus. The first time libSPI_Read() is called a read cycle is started. LibSPI_Read() then returns false until the read cycle
///        is done and the data in the data buffer is valid. 
///
/// @param  data_bytes Pointer to buffer where the data will be stored
/// @param  length Length of the expected data
/// @return bool Status of the operation
///
bool libSPI_Read(const uint8_t* data_bytes, uint8_t length)
{
	bool status = FALSE;
	
	//DEBUG_STR("Read started");
	
	if (libSPI_GetStatus() == IDLE && length <= BUFFER_SIZE)
	{
		memset(receive_buffer, 0, BUFFER_SIZE); //Clear the receive buffer
		rx_length = length;
		SPDR = 0x00; //Put dummy data in the SPI data register to enable SPI clock 
		spi_status = READ;
	}
	else if (libSPI_GetStatus() == READ_DONE)
	{
		status = TRUE;
		spi_status = IDLE;
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
/// @brief Select if master or slave mode
///
/// @param bool True if master, else slave  //TODO: Fix this description
/// @return None
///
void libSPI_SetMaster(bool master_value)
{
	if (master_value == TRUE)
	{
		SPCR |= (1<<MSTR);
	}
	else
	{
		SPCR &= ~(1<<MSTR);
	}
	return;
}


///
/// @brief Set the SPI transfer mode
///
/// @param mode SPI mode, (0-3)
/// @return bool Status of operation
///
bool libSPI_SetMode(uint8_t mode)
{
	bool status = TRUE;
	switch (mode)
	{
		case 0:
			//Clock low when idle, sample on rising edge
			SPCR &= ~((1 << CPOL) | (1 << CPHA));
			break;
		case 1:
			//Clock low when idle, sample on falling edge
			SPCR &= ~(1 << CPOL);
			SPCR |= (1 << CPHA);
			break;
		case 2:
			//Clock high when idle, sample on rising edge
			SPCR |= (1 << CPOL);
			SPCR &= ~(1 << CPHA);
			break;
		case 3:
			//Clock high when idle, sample on falling edge
			SPCR |= ((1 << CPOL) | (1 << CPHA));
			break;
		default:
			//Invalid mode
			status = FALSE;
			break;
	}
	return status;
}

//TODO: Fix this description
///
/// @brief Set slave to active, this prevents other modules to use the SPI-bus. Keep in mind that there is now control if its the same module...
///      
///
/// @param state True if SPI is active, otherwise False
/// @return bool Status of operation
///
bool SetActiveSlave(bool state)
{
	bool status = FALSE;
	if (slave_active == FALSE)
	{
		slave_active = TRUE;
		status = TRUE;
	}
	else if (slave_active == TRUE && state == FALSE)
	{
		slave_active = FALSE;
		status = TRUE;
	}
	return status;
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
