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

#define RFM_MODE_MASK 0x1C

#define REG_DATA_MODUL_DATA_MODE_MASK 0x60
#define REG_DATA_MODUL_MODULATION_TYPE_MASK 0x18
#define REG_DATA_MODUL_MODULATION_SHAPING_MASK 0x03

#define RFM_FXOSC 32000000 //32MHz


static void selectDevice(bool state);
static bool WriteRegister(uint8_t address, uint8_t register_data);
static bool ReadRegister(uint8_t address, uint8_t *register_data);

static char aes_key[17] = "1DUMMYKEYFOOBAR1";


void libRFM69_Init()
{
	DDRB |= (1 << SS);	
	selectDevice(FALSE);
	

	
	//NOTE: Encryption is disabled during development only!
	libRFM69_EnableEncryption(FALSE);
	libRFM69_SetMode(RFM_STANDBY);
	libRFM69_WaitForModeReady();
	
	libRFM69_SetBitRate(4800);
	libRFM69_SetDataMode(RFM_PACKET_DATA);
	libRFM69_SetModulationType(RFM_FSK);
	libRFM69_SetModulationShaping(0x00);
	
	DEBUG_STR(LIBNAME, "Init done");

	
}

void libRFM69_Update()
{
	
}

bool libRFM69_SetMode(libRFM69_mode_type mode)
{
	bool status = FALSE;
	uint8_t register_content;
	
	//TODO: Check if mode is valid!
	
	if (ReadRegister(REG_OPMODE, &register_content))
	{
		register_content = (register_content & ~RFM_MODE_MASK) | (mode << 2);
		status = WriteRegister(REG_OPMODE, register_content);
	}
	return status;
}


void libRFM69_WaitForModeReady()
{
	while (!libRFM69_IsModeReady())
	{
	}
}


bool libRFM69_IsModeReady()
{
	uint8_t register_content;
	
	ReadRegister(REG_IRQFLAGS1, &register_content);
	
	return (register_content & RF_IRQFLAGS1_MODEREADY) == RF_IRQFLAGS1_MODEREADY;
}



void libRFM69_SetCarrierFrequency(uint32_t frequency)
{
	WriteRegister(REG_FRFMSB, (frequency >> 16) & 0xFF);
	WriteRegister(REG_FRFMID, (frequency >> 8) & 0xFF);
	WriteRegister(REG_FRFLSB, frequency & 0xFF);
}

void libRFM69_EnableListenMode(bool enable)
{
	uint8_t register_content;

	ReadRegister(REG_OPMODE, &register_content);
	
	register_content = (register_content & 0xBF) | (uint8_t)enable;
	WriteRegister(REG_OPMODE, register_content);	
}

void libRFM69_EnableSequencer(bool enable)
{
	uint8_t register_content;

	ReadRegister(REG_OPMODE, &register_content);
	
	register_content = (register_content & 0x7F) | ((uint8_t)!enable << 7);
	WriteRegister(REG_OPMODE, register_content);	
}




void libRFM69_EnableEncryption(bool enable)
{
	uint8_t register_content;

	ReadRegister(REG_PACKETCONFIG2, &register_content);
	setBit(PACKETCONFIG2_BIT_AESON, enable, &register_content);
	WriteRegister(REG_PACKETCONFIG2, register_content);
}

void libRFM69_Test()
{
	uint8_t data = 1;
	
	ReadRegister(REG_OPMODE, &data);
	
	DEBUG_HEX(LIBNAME, data);	
}

void libRFM69_GetTemperature(uint8_t *temperature)
{
	uint8_t register_data;
	
	//Enter standby mode, temperature can only be read in standby or synthesizer mode
	libRFM69_SetMode(RFM_STANDBY);
	libRFM69_WaitForModeReady();
	
	WriteRegister(REG_TEMP1, RF_TEMP1_MEAS_START);
	
	//Wait for temperature measurement to finish
	do
	{
		ReadRegister(REG_TEMP1, &register_data);
	}
	while(register_data & RF_TEMP1_MEAS_RUNNING);		
	
	ReadRegister(REG_TEMP2, &register_data);
	*temperature = register_data;
}

bool libRFM69_SetBitRate(uint32_t bit_rate)
{
	bool status = FALSE;
	uint16_t bit_rate_value = (uint16_t)(RFM_FXOSC / bit_rate);
	
	if(WriteRegister(REG_BITRATEMSB, (uint8_t)(bit_rate_value >> 8)) == TRUE)
	{
		status = WriteRegister(REG_BITRATELSB, (uint8_t)(bit_rate_value));
	}
	return status;
}

bool libRFM69_SetDataMode(libRFM69_data_mode_type data_mode)
{
	bool status = FALSE;
	uint8_t register_content;
	
	if (ReadRegister(REG_DATAMODUL, &register_content) && data_mode != RFM_RESERVED)
	{
		register_content = (register_content & ~REG_DATA_MODUL_DATA_MODE_MASK) |
			(data_mode << 5);
		status = WriteRegister(REG_DATAMODUL, register_content);
	}
	return status;
}

bool libRFM69_SetModulationType(libRFM69_modulation_type_type modulation_type)
{
	bool status = FALSE;
	uint8_t register_content;
	
	if (ReadRegister(REG_DATAMODUL, &register_content) &&
		(modulation_type == RFM_FSK || modulation_type == RFM_OOK))
	{
		register_content = (register_content & ~REG_DATA_MODUL_MODULATION_TYPE_MASK) |
			(modulation_type << 3);
		status = WriteRegister(REG_DATAMODUL, register_content);
	}
	return status;
}

bool libRFM69_SetModulationShaping(uint8_t modulation_shaping)
{
	bool status = FALSE;
	uint8_t register_content;
	
	if (ReadRegister(REG_DATAMODUL, &register_content) && modulation_shaping < 0x4)
	{
		register_content = (register_content & ~REG_DATA_MODUL_MODULATION_SHAPING_MASK) | modulation_shaping;
		status = WriteRegister(REG_DATAMODUL, register_content);
	}
	return status;
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


