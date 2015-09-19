/**
 * @file   libRFM69.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2015-09-10 (Last edit)
 * @brief  Implementation of RFM69HW-library.
 *
 * Detailed description of file.
 */

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

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////
#define F_CPU 8000000UL // 8 MHz
#include <util/delay.h>

#include "common.h"
#include "libRFM69.h"
#include "libSPI.h"
#include "libTimer.h"
#include "libDebug.h"
#include "RFM69Registers.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define LIBNAME "libRFM69"
#define SS DDB2
#define SPI_MODE 0

#define REG_OPMODE_MODE_MASK 0x1C

#define REG_DATA_MODUL_DATA_MODE_MASK			0x60
#define REG_DATA_MODUL_MODULATION_TYPE_MASK		0x18
#define REG_DATA_MODUL_MODULATION_SHAPING_MASK	0x03

#define REG_PA_LEVEL_PA_MASK 0xE0
#define REG_PA_LEVEL_POUT_MASK 0x1F

#define RFM_FXOSC 32000000 //32MHz
#define RFM_FSTEP (float)61.03515625 // FSTEP = FXOSC / 2^19
#define RFM_FIFO_SIZE 66

#define WAIT_TIMEOUT_MS 10

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

typedef enum
{
    REG_IRQFLAGS2_BIT_CRCOK = 1,
    REG_IRQFLAGS2_BIT_PAYLOADREADY,
    REG_IRQFLAGS2_BIT_PACKETSENT,
    REG_IRQFLAGS2_BIT_FIFOOVERRUN,
    REG_IRQFLAGS2_BIT_FIFOLEVEL,
    REG_IRQFLAGS2_BIT_FIFONOTEMPTY,
    REG_IRQFLAGS2_BIT_FIFOFULL,
}irqflags2_bit_type;

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static char aes_key[17] = "1DUMMYKEYFOOBAR1";
static uint8_t net_id[6] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

static bool WriteRegister(uint8_t address, uint8_t register_data);
static bool ReadRegister(uint8_t address, uint8_t *register_data);
static bool IsBitSet(uint8_t address, uint8_t bit);

static void PreCallback(void);
static void PostCallback(void);

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

///
/// @brief Init libRFM69 and set default configuration. Prints the current
///		   configuration.
///
/// @param  None
/// @return None
///
void libRFM69_Init()
{
 
	uint8_t sync_word[8];
	uint8_t idx;
	
	DDRB |= (1 << SS);	
	PORTB |= (1 << SS); //Pull SS high to release device
		
	//NOTE: Encryption is disabled during development only!
	libRFM69_EnableEncryption(FALSE);
    libRFM69_EnableSequencer(TRUE);
    libRFM69_EnableListenMode(FALSE);
	libRFM69_SetMode(RFM_STANDBY);
	libRFM69_WaitForModeReady();
	
	libRFM69_SetBitRate(4800);
	libRFM69_SetDataMode(RFM_PACKET_DATA);
	libRFM69_SetModulationType(RFM_FSK);
	libRFM69_SetModulationShaping(0x00);
	
	libRFM69_SetFrequencyDeviation(5000);
	libRFM69_SetCarrierFrequency(868000000);
    
    libRFM69_EnableSyncWordGeneration(TRUE);
    libRFM69_SetFIFOFillCondition(RFM_FIFO_FILL_AUTO);
    libRFM69_SetSyncWordSize(5);
    libRFM69_SetSyncWord(net_id, 6);
    
    
    libRFM69_ClearFIFOOverrun();
    
    libRFM69_SetPacketFormat(RFM_PACKET_VARIABLE_LEN);
		
	DEBUG("Bit rate: %u bps\r\n", libRFM69_GetBitrate());
	DEBUG("Chip: 0x%02X\r\n", libRFM69_GetChipVersion());
	DEBUG("PA mode: 0x%02X\r\n", libRFM69_GetPowerAmplifierMode());
	DEBUG("High power: 0x%02X\r\n", libRFM69_IsHighPowerEnabled());	
	DEBUG("Output power: %i dBm\r\n", libRFM69_GetOutputPower());	
	DEBUG("RSSI: %i dBm\r\n", libRFM69_GetRSSI());		
	DEBUG("Preamble length: %u\r\n", libRFM69_GetPreambleLength());	
	DEBUG("Sync length: %u\r\n", libRFM69_GetSyncWordSize());	
	
	libRFM69_GetSyncWord(sync_word, libRFM69_GetSyncWordSize());
	DEBUG("Sync word: 0x");
	for (idx = 0; idx < libRFM69_GetSyncWordSize(); ++idx)
	{
		DEBUG("%02X", sync_word[idx]);
	}
	DEBUG("\r\n");
    
    DEBUG("FIFO full: 0x%02X\r\n", libRFM69_IsFIFOFull());
    DEBUG("FIFO not empty: 0x%02X\r\n", libRFM69_IsFIFONotEmpty());
    DEBUG("FIFO level: 0x%02X\r\n", libRFM69_IsFIFOLevel());
    DEBUG("FIFO overrun: 0x%02X\r\n", libRFM69_IsFIFOOverrun());         
    DEBUG("Packet sent: 0x%02X\r\n", libRFM69_IsPacketSent());
    DEBUG("Payload ready: 0x%02X\r\n", libRFM69_IsPayloadReady());
    DEBUG("CRC ok: 0x%02X\r\n", libRFM69_IsCRCOk());
    
	INFO("Init done!");        
    
    libRFM69_DumpRegisterValues();
    //libRFM69_Send();
    
    

}

///
/// @brief Update the internal state of libRFM69. NOT IMPLEMENTED
///
/// @param  None
/// @return None
///
void libRFM69_Update()
{
	
}

void libRFM69_Send(void)
{
    uint8_t test_data[6] = {0x05, 0x11, 0x22, 0x22, 0x33, 0x33};
    //WriteRegister(REG_FIFOTHRESH, 0x8F);        
        
    INFO("Sending...");
    DEBUG("Change to standby...");
    libRFM69_SetMode(RFM_STANDBY);
    libRFM69_WaitForModeReady();
    DEBUG(" done\r\n");
    

    
    libRFM69_WriteToFIFO(test_data, 6);
    DEBUG("TX ready: %u\r\n", libRFM69_IsTxReady());
    DEBUG("Changed to transmitter\r\n");
    libRFM69_SetMode(RFM_TRANSMITTER);
    DEBUG("TX ready: %u\r\n", libRFM69_IsTxReady());
    
    while(libRFM69_IsFIFONotEmpty())
    {
        DEBUG("FIFO not empty: 0x%02X\r\n", libRFM69_IsFIFONotEmpty());
        DEBUG("Packet sent: 0x%02X\r\n", libRFM69_IsPacketSent());
        DEBUG("Payload ready: 0x%02X\r\n", libRFM69_IsPayloadReady());
            DEBUG("TX ready: %u\r\n", libRFM69_IsTxReady());
        _delay_ms(1000);
    }
    DEBUG("Packet sent: 0x%02X\r\n", libRFM69_IsPacketSent());
    libRFM69_SetMode(RFM_STANDBY);
     DEBUG("All done!\r\n");
    
    
}

bool libRFM69_IsFIFOFull(void)
{
    return IsBitSet(REG_IRQFLAGS2, REG_IRQFLAGS2_BIT_FIFOFULL);
}

bool libRFM69_IsFIFONotEmpty(void)
{
    return IsBitSet(REG_IRQFLAGS2, REG_IRQFLAGS2_BIT_FIFONOTEMPTY);
}

bool libRFM69_IsFIFOLevel(void)
{
    return IsBitSet(REG_IRQFLAGS2, REG_IRQFLAGS2_BIT_FIFOLEVEL);
}

bool libRFM69_IsFIFOOverrun(void)
{
    return IsBitSet(REG_IRQFLAGS2, REG_IRQFLAGS2_BIT_FIFOOVERRUN);
}

bool libRFM69_IsPacketSent(void)
{
    return IsBitSet(REG_IRQFLAGS2, REG_IRQFLAGS2_BIT_PACKETSENT);
}

bool libRFM69_IsPayloadReady(void)
{
    return IsBitSet(REG_IRQFLAGS2, REG_IRQFLAGS2_BIT_PAYLOADREADY);
}

bool libRFM69_IsCRCOk(void)
{
    return IsBitSet(REG_IRQFLAGS2, REG_IRQFLAGS2_BIT_CRCOK);
}

bool libRFM69_WriteToFIFO(uint8_t *data, uint8_t length)
{
    uint8_t index;

    if (length > RFM_FIFO_SIZE)
    {
        WARNING("Not enough space in FIFO");
        return FALSE;
    }

    for (index = 0; index < length; ++index)
    {
        if (libRFM69_IsFIFOFull())
        {
            ERROR("FIFO is full");
            break;
        }
        
        if (!WriteRegister(REG_FIFO, data[index]))
        {
            ERROR("Failed write to FIFO");
            break;
        }
    }
    //If index not is equal to length something went wrong
    return (index == length);
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


bool libRFM69_WaitForModeReady()
{
    bool status = TRUE;
    uint32_t timeout_timer = libTimer_GetMilliseconds();

    while (!libRFM69_IsModeReady())
    {
        if (libTimer_TimeDifference(timeout_timer) > WAIT_TIMEOUT_MS)
        {
            ERROR("Timeout!");
            status = FALSE;
            break;
        }
    }
    return status;
}


bool libRFM69_IsModeReady()
{
	uint8_t register_content;
	
	ReadRegister(REG_IRQFLAGS1, &register_content);
	return (register_content & RF_IRQFLAGS1_MODEREADY) == RF_IRQFLAGS1_MODEREADY;
}

bool libRFM69_IsRxReady()
{
	uint8_t register_content;
	
	ReadRegister(REG_IRQFLAGS1, &register_content);
	return (register_content & RF_IRQFLAGS1_RXREADY) == RF_IRQFLAGS1_RXREADY;
}

bool libRFM69_IsTxReady()
{
	uint8_t register_content;
	
	ReadRegister(REG_IRQFLAGS1, &register_content);
	return (register_content & RF_IRQFLAGS1_TXREADY) == RF_IRQFLAGS1_TXREADY;
}

bool libRFM69_IsRSSIThresholdExceeded()
{
	uint8_t register_content;
	
	ReadRegister(REG_IRQFLAGS1, &register_content);
	return (register_content & RF_IRQFLAGS1_RSSI) == RF_IRQFLAGS1_RSSI;
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

bool libRFM69_SetPacketFormat(libRFM69_packet_format_type packet_format)
{
	bool status = FALSE;
	uint8_t register_content;
	
	if (packet_format == RFM_PACKET_FIXED_LEN ||
		packet_format == RFM_PACKET_VARIABLE_LEN)
	{
		if (ReadRegister(REG_PACKETCONFIG1, &register_content))
		{
			SetBit(PACKETCONFIG1_BIT_PCKFORMAT, (bool)packet_format,
				&register_content);
			status = WriteRegister(REG_PACKETCONFIG1, register_content);
		}
	}
	return status;
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
	while (register_data & RF_TEMP1_MEAS_RUNNING);		
	
	ReadRegister(REG_TEMP2, &register_data);
	*temperature = register_data;
}

bool libRFM69_SetBitRate(uint32_t bit_rate)
{
	bool status = FALSE;
	uint16_t bit_rate_value = (uint16_t)(RFM_FXOSC / bit_rate);
	
	if (WriteRegister(REG_BITRATEMSB, (uint8_t)(bit_rate_value >> 8)) == TRUE)
	{
		status = WriteRegister(REG_BITRATELSB, (uint8_t)(bit_rate_value));
	}
	return status;
}

uint32_t libRFM69_GetBitrate(void)
{
	uint16_t bit_rate_value;
	uint8_t tmp;
	
	ReadRegister(REG_BITRATEMSB, &tmp);
	bit_rate_value = (uint16_t)(tmp << 8);
	
	ReadRegister(REG_BITRATELSB, &tmp);
	bit_rate_value |= tmp;
	
	if (bit_rate_value > 0)
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
	
	if (WriteRegister(REG_FDEVMSB, ((frequency_deviation_value >> 8) & 0xFF)))
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
	
	if (mode < 0x04 && mode > 0x01 && ReadRegister(REG_PALEVEL, &register_content))
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
	
	if (power_level <= 31 && ReadRegister(REG_PALEVEL, &register_content))
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
	
	if (ReadRegister(REG_PALEVEL, &register_content) == TRUE)
	{	
		register_content &= REG_PA_LEVEL_POUT_MASK;	
		
		switch (libRFM69_GetPowerAmplifierMode())
		{
			case RFM_PWR_1:
			case RFM_PWR_2:
				pout = -18 + (int8_t)register_content;
				break;
			
			case RFM_PWR_3_4:
			
				if (libRFM69_IsHighPowerEnabled() == TRUE)
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
	
	if (ReadRegister(REG_PALEVEL, &register_content) == TRUE)	
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
	
	if (ReadRegister(REG_OCP, &register_content) == TRUE)
	{
		SetBit(4, enabled, &register_content);
		status = WriteRegister(REG_OCP, register_content);
	}
	
	return status;
}

int8_t libRFM69_GetRSSI(void)
{
	uint8_t register_content;
	
	//Start RSSI measurement
	WriteRegister(REG_RSSICONFIG, 0x01);
	
	do
	{
		ReadRegister(REG_RSSICONFIG, &register_content);
	}while (register_content == 0);
	
	ReadRegister(REG_RSSIVALUE, &register_content);
	
	return (-1 * (int8_t)(register_content >> 1));
}

bool libRFM69_ClearFIFOOverrun(void)
{
	bool status = FALSE;
	uint8_t register_content;
	
	if (ReadRegister(REG_IRQFLAGS2, &register_content) == TRUE)
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

bool libRFM69_SetSyncWordSize(uint8_t size)
{
	bool status = FALSE;
	uint8_t register_content;
	
	if (size <= MAX_SYNC_WORD_SIZE)
	{
		if (ReadRegister(REG_SYNCCONFIG, &register_content))
		{
			register_content &= ~(MAX_SYNC_WORD_SIZE << 3);
        
			register_content |= (size << 3);
			status = WriteRegister(REG_SYNCCONFIG, register_content);	
            
		}
	}
	return status;
}

bool libRFM69_SetPreambleLength(uint16_t length)
{
	bool status;
	
	status = (WriteRegister(REG_PREAMBLEMSB, (length >> 8)) &&
			  WriteRegister(REG_PREAMBLELSB, (length & 0xFF)));
	return status;			  	
}

uint16_t libRFM69_GetPreambleLength(void)
{
	uint16_t length = 0;
	uint8_t register_content;
	
	if (ReadRegister(REG_PREAMBLEMSB, &register_content) == FALSE)
	{
        ERROR("Failed to read MSB of preamble");
		return 0;
	}
	
	length = (uint16_t)(register_content << 8);
	
	if (ReadRegister(REG_PREAMBLELSB, &register_content) == FALSE)
	{
        ERROR("Failed to read LSB of preamble");
		return 0;
	}	
	
	length |= register_content;
	return length;	
}

uint8_t libRFM69_GetSyncWordSize(void)
{
	uint8_t register_content;
	uint8_t size = 0;
	
	if (ReadRegister(REG_SYNCCONFIG, &register_content))
	{
		//From RFM69HW datasheet, table 28: sync word size = RegSyncConfig[5:2] + 1
		size = ((register_content >> 3) & 0x07) + 1;
	}
	
	return size;
}

bool libRFM69_SetSyncWord(uint8_t *sync_word, uint8_t length)
{
	bool status = FALSE;
	uint8_t sync_word_size;
	
	sync_word_size = libRFM69_GetSyncWordSize();
	
	if (length == sync_word_size)
	{
		uint8_t index;
		for (index = 0; index < length; ++index)
		{
			//Abort if write fails, this can leave an incomplete sync word
			//so this must be handle by the caller.			
			
			//From RFM69HW datasheet, note on page 56: sync word values equal
			//to 0x00 is forbidden.
			if (sync_word[index] == 0x00)
			{
                WARNING("Sync word value 0x00 is not valid");
				break;
			}
			
			if (WriteRegister(REG_SYNCVALUE1 + index, sync_word[index]) == FALSE)
			{
                ERROR("Failed to write sync word value");
				break;
			}
		}
		//Check if a complete sync word was written.
		status = (index == length);
	}
	
	return status;
}

//TODO: Return number of bytes read?
bool libRFM69_GetSyncWord(uint8_t *sync_word, uint8_t length)
{
	bool status = FALSE;
	uint8_t sync_word_size;
	
	sync_word_size = libRFM69_GetSyncWordSize();
	
	if (length <= sync_word_size)
	{
		uint8_t index;
		for (index = 0; index < sync_word_size; ++index)
		{
			//Abort if read fails
			if (ReadRegister(REG_SYNCVALUE1 + index, &sync_word[index]) == FALSE)
			{
				WARNING("Failed to read from REG_SYNCVALUE%u", index+1);
				break;
			}
		}
		//Check if a complete sync word was written.
		status = (index == sync_word_size);
	}
	return status;
}


uint8_t libRFM69_GetChipVersion(void)
{
	uint8_t register_content;
	if (ReadRegister(REG_VERSION, &register_content) == TRUE)
	{		
		return register_content;
	}
	return 0;
}

void libRFM69_DumpRegisterValues(void)
{
	uint8_t register_address;
	uint8_t register_content;
	
	for (register_address = 0; register_address < 0x4F; ++register_address)
	{
		ReadRegister(register_address, &register_content);
		DEBUG("REG ADDR: 0x%02x	REG VALUE: 0x%02x\r\n", register_address, register_content);
	}
	return;
}

bool libRFM69_EnableSyncWordGeneration(bool enabled)
{
	bool status = FALSE;
	uint8_t register_content;
	
	if (ReadRegister(REG_SYNCCONFIG, &register_content) == TRUE)
	{
		SetBit(7, enabled, &register_content);
		status = WriteRegister(REG_SYNCCONFIG, register_content);
	}
	
	return status;
}

bool libRFM69_SetFIFOFillCondition(libRFM69_fifo_fill_condition_type fill_condition)
{
    bool status = FALSE;
    uint8_t register_content;

    if (ReadRegister(REG_SYNCCONFIG, &register_content) == TRUE)
    {
        SetBit(6, (bool)fill_condition, &register_content);
        status = WriteRegister(REG_SYNCCONFIG, register_content);
    }
    return status;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static void PreCallback(void)
{
    libSPI_SetMode(SPI_MODE);
    PORTB &= ~(1 << SS); //Pull SS low to select device
    return;
}

static void PostCallback(void)
{
    PORTB |= (1 << SS); //Pull SS high to release device
    return;
}

static bool WriteRegister(uint8_t address, uint8_t register_data)
{
	bool status = FALSE;
	
	libSPI_WriteByte(address | WRITE_REG, &PreCallback, NULL);
	libSPI_WriteByte(register_data, NULL, &PostCallback);
		
	status = TRUE;
	return status;
}

//TODO: Add check if address is valid and return status
static bool ReadRegister(uint8_t address, uint8_t *register_data)
{
	bool status = FALSE;

	libSPI_WriteByte(address & READ_REG, &PreCallback, NULL);
	libSPI_ReadByte(register_data, NULL, &PostCallback);
		
	status = TRUE;
	return status;
}

///
/// @brief Check if a bit is set in a register. If something fails this
//         function will return true!
///
/// @param register Register to check in
/// @param bit Bit to check
/// @return TRUE if bit is set
/// @return FALSE if bit is not set
///
static bool IsBitSet(uint8_t address, uint8_t bit)
{
    uint8_t register_content;
    bool status = TRUE;
    
    if (bit > 7 || address > REG_TEMP2)
    {
        WARNING("Register or bit index is invalid");
        return TRUE;
    }

    if (ReadRegister(address, &register_content))
    {
        status = (register_content & (1 << bit)) > 0;
    }
    return status;
}    
