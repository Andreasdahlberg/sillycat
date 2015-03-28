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
#include <string.h>
#include "libDebug.h"
#include "libSPI.h"
#include "libDS3234.h"

#define LIBNAME "libDS3234"
#define DEBUG_STR(message)	libDebug_PrintString(LIBNAME, message)
#define DEBUG_INT(num)		libDebug_PrintInteger(LIBNAME, num)
#define DEBUG_HEX(num)		libDebug_PrintHex(LIBNAME, num)

#define SS	DDC0

void selectDevice(bool state);
bool ReadRegister(uint8_t address, uint8_t *register_data);
bool WriteRegister(uint8_t address, uint8_t register_data);
bool RegisterAddressValid(uint8_t address);


///
/// @brief Init the DS3234 RTC
///
/// @param  None
/// @return None
///
void libDS3234_Init()
{
	//Set slave select as output
	DDRC |= (1 << SS);
	selectDevice(FALSE);
	
	libDebug_PrintString(LIBNAME, "Init done");
	return;
}


void libDS3234_Update()
{

	return;
}

void libDS3234_Test()
{
	uint8_t data;
	
	
	libDS3234_GetSeconds(&data);
	
	DEBUG_INT(data);

}

void libDS3234_GetTemperature(uint16_t *temperature)
{
	uint8_t data;
	
	ReadRegister(REG_TEMP_MSB, &data);
	*temperature = data * 100;
	
	ReadRegister(REG_TEMP_LSB, &data);
	*temperature += (data >> 6);
}

void libDS3234_GetSeconds(uint8_t *seconds)
{
	uint8_t data;
	
	ReadRegister(REG_SECONDS, &data);
	*seconds = ((data & 0xF0)>>4)*10;
	*seconds += (data & 0x0F);
}


bool WriteRegister(uint8_t address, uint8_t register_data)
{
	bool status = FALSE;
	
	if (RegisterAddressValid(address))
	{
		selectDevice(TRUE);
		libSPI_WriteByte(address|WRITE_ADDRESS);
		libSPI_WriteByte(register_data);
		selectDevice(FALSE);
		
		status = TRUE;
	}
	return status;
}

bool ReadRegister(uint8_t address, uint8_t *register_data)
{
	bool status = FALSE;
	
	if (RegisterAddressValid(address))
	{
		selectDevice(TRUE);
		libSPI_WriteByte(address|READ_ADDRESS);
		libSPI_ReadByte(register_data);
		selectDevice(FALSE);
		
		status = TRUE;
	}
	return status;
}

bool RegisterAddressValid(uint8_t address)
{
	return (address >= REG_SECONDS && address <= REG_SRAM_DATA);
}

//This function is needed by the RTC interface!
void selectDevice(bool state)
{
	if (state == TRUE)
	{
		PORTC &= ~(1 << SS);
	}
	else
	{
		PORTC |= (1 << SS);
	}
}


