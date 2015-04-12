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


#include "common.h"
#include "libRFM69.h"
#include "libSPI.h"
#include "libDebug.h"
#include "RFM69Registers.h"

#define LIBNAME "libRFM69"
#define SS DDB2


static void selectDevice(bool state);
static bool WriteRegister(uint8_t address, uint8_t register_data);
static bool ReadRegister(uint8_t address, uint8_t *register_data);


void libRFM69_Init()
{
	DDRB |= (1 << SS);	
	selectDevice(FALSE);
	
	DEBUG_STR(LIBNAME, "Init done");
	
}

void libRFM69_Update()
{
	
}

bool libRFM69_SetMode(libRFM69_mode_type mode)
{
	bool status = FALSE;
	
	switch (mode)
	{
		case RFM_SLEEP:
			break;
		
		case RFM_STANDBY:
			break;
			
		case RFM_SYNTHESIZER:
			break;
			
		case RFM_TRANSMITTER:
			break;
			
		case RFM_RECEIVER:
			break;
			

	}
	
	return status;
}

void libRFM69_Test()
{
	uint8_t data = 1;
	
	ReadRegister(REG_OPMODE, &data);
	
	DEBUG_HEX(LIBNAME, data);
}

static bool WriteRegister(uint8_t address, uint8_t register_data)
{
	bool status = FALSE;
	
	selectDevice(TRUE);
	libSPI_WriteByte(address | WRITE_REG);
	libSPI_WriteByte(register_data);
	selectDevice(FALSE);
		
	status = TRUE;
	
	return status;
}



static bool ReadRegister(uint8_t address, uint8_t *register_data)
{
	bool status = FALSE;

	selectDevice(TRUE);
	libSPI_WriteByte(address & READ_REG);
	libSPI_ReadByte(register_data);
	selectDevice(FALSE);
		
	status = TRUE;
	
	return status;
}



static void selectDevice(bool state)
{
	if (state == TRUE)
	{
		PORTB &= ~(1 << SS);
	}
	else
	{
		PORTB |= (1 << SS);
	}
}


