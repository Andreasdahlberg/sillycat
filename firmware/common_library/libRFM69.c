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

#define REG_OPMODE_MODE_MASK 0x1C

#define REG_DATA_MODUL_DATA_MODE_MASK			0x60
#define REG_DATA_MODUL_MODULATION_TYPE_MASK		0x18
#define REG_DATA_MODUL_MODULATION_SHAPING_MASK	0x03

#define REG_PA_LEVEL_PA_MASK 0xE0
#define REG_PA_LEVEL_POUT_MASK 0x1F

#define RFM_FXOSC 32000000 //32MHz
#define RFM_FSTEP (float)61.03515625 // FSTEP = FXOSC / 2^19


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
	
	libRFM69_SetFrequencyDeviation(5000);
	libRFM69_SetCarrierFrequency(868000000);
	
	DEBUG_STR(LIBNAME, "Init done");
	
	
	
	DEBUG("Bit rate: %u bps\r\n", libRFM69_GetBitrate());
	DEBUG("Chip: 0x%02X\r\n", libRFM69_GetChipVersion());
	DEBUG("PA mode: 0x%02X\r\n", libRFM69_GetPowerAmplifierMode());
	DEBUG("High power: 0x%02X\r\n", libRFM69_IsHighPowerEnabled());	
	DEBUG("Output power: %i dBm\r\n", libRFM69_GetOutputPower());	
	DEBUG("RSSI: %i dBm\r\n", libRFM69_GetRSSI());		
	
	
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
		register_content = (register_content & ~REG_OPMODE_MODE_MASK) | (mode << 2);
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
	uint32_t frequency_value = frequency / RFM_FSTEP;
	
	DEBUG("Freq: %lu\r\n", frequency);
	DEBUG("Freq value: 0x%03lX\r\n", frequency_value);
	
	WriteRegister(REG_FRFMSB, (frequency_value >> 16) & 0xFF);
	WriteRegister(REG_FRFMID, (frequency_value >> 8) & 0xFF);
	WriteRegister(REG_FRFLSB, frequency_value & 0xFF);
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
	SetBit(PACKETCONFIG2_BIT_AESON, enable, &register_content);
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

uint32_t libRFM69_GetBitrate()
{
	uint16_t bit_rate_value;
	uint8_t tmp;
	
	ReadRegister(REG_BITRATEMSB, &tmp);
	bit_rate_value = (uint16_t)(tmp << 8);
	
	ReadRegister(REG_BITRATELSB, &tmp);
	bit_rate_value |= tmp;
	
	if(bit_rate_value > 0)
	{
		return (RFM_FXOSC / bit_rate_value);
	}
	return 0;
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

bool libRFM69_SetFrequencyDeviation(uint16_t frequency_deviation)
{
	bool status = FALSE;
	uint16_t frequency_deviation_value = (uint16_t)((float)frequency_deviation/RFM_FSTEP);
	
	DEBUG("Freq deviation: %u\r\n", frequency_deviation);
	DEBUG("Freq deviation value: 0x%04X\r\n", frequency_deviation_value);
	
	if(WriteRegister(REG_FDEVMSB, ((frequency_deviation_value >> 8) & 0xFF)))
	{
		status = WriteRegister(REG_FDEVLSB, (frequency_deviation_value & 0xFF));
	}
	
	return status;
}

bool libRFM69_SetPowerAmplifierMode(uint8_t mode)
{
	bool status = FALSE;
	uint8_t register_content;
	
	DEBUG("PA mode: 0x%02X\r\n", mode);
	
	if(mode < 0x04 && mode > 0x01 && ReadRegister(REG_PALEVEL, &register_content))
	{
		register_content &= ~REG_PA_LEVEL_PA_MASK;
		register_content |= (mode << 5);
		
		status = WriteRegister(REG_PALEVEL, register_content);
	}
	return status;
}

bool libRFM69_SetPowerLevel(uint8_t power_level)
{
	bool status = FALSE;
	uint8_t register_content;
	
	if(power_level <= 31 && ReadRegister(REG_PALEVEL, &register_content))
	{
		register_content &= ~REG_PA_LEVEL_POUT_MASK;
		register_content |= power_level;
		
		status = WriteRegister(REG_PALEVEL, register_content);
	}
	return status;
}

bool libRFM69_IsHighPowerEnabled(void)
{
	bool status = FALSE;
	
	//Not implemented
	
	return status;
}


int8_t libRFM69_GetOutputPower(void)
{
	int8_t pout = 0;
	uint8_t register_content;
	
	if(ReadRegister(REG_PALEVEL, &register_content) == TRUE)
	{	
		register_content &= REG_PA_LEVEL_POUT_MASK;	
		
		switch(libRFM69_GetPowerAmplifierMode())
		{
			case RFM_PWR_1:
			case RFM_PWR_2:
				pout = -18 + (int8_t)register_content;
				break;
			
			case RFM_PWR_3_4:
			
				if(libRFM69_IsHighPowerEnabled() == TRUE)
				{
					pout = -11 + (int8_t)register_content;
				}
				else
				{
					pout = -14 + (int8_t)register_content;				
				}
				break;
				
			default:
				break;
		}
	}
	return pout;
}

uint8_t libRFM69_GetPowerAmplifierMode(void)
{
	uint8_t register_content;
	
	if(ReadRegister(REG_PALEVEL, &register_content) == TRUE)	
	{
		return (register_content >> 5);
	}
	return 0;
}

//Overload Current Protection
bool libRFM69_EnableOCP(bool enabled)
{
	bool status = FALSE;
	uint8_t register_content;
	
	if(ReadRegister(REG_OCP, &register_content) == TRUE)
	{
		SetBit(4, enabled, &register_content);
		status = WriteRegister(REG_OCP, register_content);
	}
	
	return status;
}

int8_t libRFM69_GetRSSI(void)
{
	int8_t rssi_value = 0;
	uint8_t register_content;
	
	//Start RSSI measurement
	WriteRegister(REG_RSSICONFIG, 0x01);
	
	do
	{
		ReadRegister(REG_RSSICONFIG, &register_content);
	} while (register_content == 0);
	
	ReadRegister(REG_RSSIVALUE, &register_content);
	
	return (-1 * (int8_t)(register_content >> 1));
}

bool libRFM69_ClearFIFOOverrun(void)
{
	bool status = FALSE;
	uint8_t register_content;
	
	if(ReadRegister(REG_IRQFLAGS2, &register_content) == TRUE)
	{
		SetBit(4, TRUE, &register_content);
		status = WriteRegister(REG_IRQFLAGS2, register_content);
	}
	return status;
}


bool libRFM69_CalibrateRCOscillator(void)
{
	//Not implemented
	return FALSE;
}

uint8_t libRFM69_GetChipVersion(void)
{
	uint8_t register_content;
	if(ReadRegister(REG_VERSION, &register_content) == TRUE)
	{		
		return register_content;
	}
	return 0;
}

void libRFM69_DumpRegisterValues(void)
{
	uint8_t register_address;
	uint8_t register_content;
	
	for(register_address = 0; register_address < 0x4F; ++register_address)
	{
		ReadRegister(register_address, &register_content);
		DEBUG("REG ADDR: 0x%02x	REG VALUE: 0x%02x\r\n", register_address, register_content);
	}
	return;
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


