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

#include "libUART.h"
#include <avr/io.h>


///
/// @brief Init UART
///
/// @param  None
/// @return None
///
void libUART_Init()
{
	//Set baud rate
	UBRR0H = (uint8_t)(UBRRn>>8);
	UBRR0L = (uint8_t)UBRRn;
		
	//Enable RX & TX
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	
	//Set frame format: 1 stop bit 8 data
	UCSR0C =  (3 << UCSZ00);
	return;
}

///
/// @brief Send an array via UART
///
/// @param  data Pointer to array with data to send
/// @param  len Length of array
/// @return None
///
void libUART_SendArray(uint8_t* data, uint8_t len)
{
	uint8_t byte_counter;
	
	for(byte_counter = 0; byte_counter < len; ++byte_counter)
	{
		libUART_SendByte(data[byte_counter]);
	}
	return;
}


///
/// @brief Send a byte via UART
///
/// @param  data Byte to send
/// @return None
///
void libUART_SendByte(uint8_t data)
{
	//wait for empty transmit buffer
	while(!(UCSR0A & (1 << UDRE0)))
	{	
	}
	//put data in buffer
	UDR0 = data;
}
