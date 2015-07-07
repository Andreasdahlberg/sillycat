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
#include "common.h"

#define LIBNAME "libDS3234"
#define SS	DDC0

static void selectDevice(bool state);
static bool ReadRegister(uint8_t address, uint8_t *register_data);
static bool WriteRegister(uint8_t address, uint8_t register_data);
bool RegisterAddressValid(uint8_t address);
bool GetDecimalRegisterValue(uint8_t address, uint8_t *value);
bool SetDecimalRegisterValue(uint8_t address, uint8_t value);


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
	
	DEBUG_STR(LIBNAME, "Init done");
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
	DEBUG_INT(LIBNAME, data);

}

void libDS3234_GetTemperature(uint16_t *temperature)
{
	uint8_t data;
	
	ReadRegister(REG_TEMP_MSB, &data);
	*temperature = data * 100;
	
	ReadRegister(REG_TEMP_LSB, &data);
	*temperature += (data >> 6);
}


bool libDS3234_GetMonth(uint8_t *month)
{
	bool status = FALSE;
	uint8_t register_data;

	if (ReadRegister(REG_MONTH_CENTURY, &register_data) == TRUE)
	{
		*month = BCDToDecimal(register_data & MONTH_MASK);
		status = TRUE;
	}
	return status;
}

bool libDS3234_GetDate(uint8_t *date)
{
	return GetDecimalRegisterValue(REG_DATE, date);
}

bool libDS3234_GetDay(uint8_t *day)
{
	return GetDecimalRegisterValue(REG_DAY, day);
}


bool libDS3234_GetHour(uint8_t *hour)
{
	bool status = FALSE;
	uint8_t register_data;

	if(ReadRegister(REG_HOUR, &register_data) == TRUE)
	{
		*hour = BCDToDecimal(register_data);		
		status = TRUE;
	}
	return status;
}

bool libDS3234_GetMinutes(uint8_t *minutes)
{
	return GetDecimalRegisterValue(REG_MINUTES, minutes);
}

bool libDS3234_SetMinutes(uint8_t minutes)
{
	return SetDecimalRegisterValue(REG_MINUTES, minutes);
}

bool libDS3234_GetSeconds(uint8_t *seconds)
{
	return GetDecimalRegisterValue(REG_SECONDS, seconds);
}

bool libDS3234_GetHourMode(libDS3234_hour_mode_type *hour_mode)
{
	bool status = FALSE;
	uint8_t register_value;

	if(ReadRegister(REG_HOUR, &register_value)== TRUE)
	{
		if((register_value & HOUR_MODE_BIT) == 0x00)
		{
			*hour_mode = LIBDS3234_24HOUR_MODE;
			status = TRUE;
		}		
		else
		{
			*hour_mode = LIBDS3234_12HOUR_MODE;
			status = TRUE;
		}
	}
	return status;
}

bool libDS3234_SetHourMode(libDS3234_hour_mode_type hour_mode)
{
	bool status = FALSE;
	uint8_t register_value;
	
	if(ReadRegister(REG_HOUR, &register_value)== TRUE)
	{
		if(hour_mode == LIBDS3234_24HOUR_MODE)
		{
			register_value &= ~(1 << HOUR_MODE_BIT);
			status = WriteRegister(REG_HOUR, register_value);	
		}
		else
		{
			register_value |= (1 << HOUR_MODE_BIT);
			status = WriteRegister(REG_HOUR, register_value);
		}
	}
	return status;
}

/// Local functions

///
/// @brief	Get the value of a register as a decimal value. The register must
///			contain packed BCD-data.
///
///
/// @param	address Address to register
/// @param	value Pointer to variable where the value will be stored.
/// @return bool Status of operation
///
bool GetDecimalRegisterValue(uint8_t address, uint8_t *value)
{
	bool status = FALSE;
	uint8_t register_data;
		
	if (ReadRegister(address, &register_data) == TRUE)
	{
		*value = BCDToDecimal(register_data);
		status = TRUE;
	}
	return status;
}


bool SetDecimalRegisterValue(uint8_t address, uint8_t value)
{
	bool status = FALSE;
	uint8_t register_data;
	
	//Only values smaller then 100 is accepted since a byte
	//only can hold two-digits of BCD data.
	if(value < 100)
	{
		register_data = (uint8_t)DecimalToBCD(value);
		status = WriteRegister(address, register_data);		 
	}
	return status;
}


static bool WriteRegister(uint8_t address, uint8_t register_data)
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


static bool ReadRegister(uint8_t address, uint8_t *register_data)
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


static void selectDevice(bool state)
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


