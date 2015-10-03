/**
 * @file   libDS3234.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2015-10-03 (Last edit)
 * @brief  Implementation of DS3234-library.
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
#include <avr/io.h>
#include <string.h>

#include "common.h"
#include "libDebug.h"
#include "libSPI.h"
#include "libDS3234.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define SS	DDC0
#define SPIMODE 1

#define SRAM_SIZE 256

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

static bool ReadRegister(uint8_t address, uint8_t *register_data);
static bool WriteRegister(uint8_t address, uint8_t register_data);
static bool RegisterAddressValid(uint8_t address);
static bool GetDecimalRegisterValue(uint8_t address, uint8_t *value);
static bool SetDecimalRegisterValue(uint8_t address, uint8_t value);
static void DumpRegisterValues(void);

static void PreCallback(void);
static void PostCallback(void);

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

///
/// @brief Init the DS3234 RTC
///
/// @param  None
/// @return None
///
void libDS3234_Init(void)
{
    libDS3234_HWInit();
    
    // Clear control register, this makes sure that the oscillator is running
    // when power is removed.
    WriteRegister(REG_CONTROL, 0x00);
    DumpRegisterValues();
        
    INFO("Init done");
    return;
}

///
/// @brief Set the SS pin as output and pull high. This function should
///        be called as early as possible in a systems with several SPI-devices.
///
/// @param  None
/// @return None
///
void libDS3234_HWInit(void)
{
    //Set SS as output
    DDRC |= (1 << SS);
    //Pull SS high to release device
    PORTC |= (1 << SS);    
    return;
}

///
/// @brief Update the internal state of libDS3234. NOT IMPLEMENTED
///
/// @param  None
/// @return None
///
void libDS3234_Update(void)
{
    return;
}

///
/// @brief Get the chip-temperature from the DS3234 RTC
///
/// @param  *temperature Pointer to variable where the temperature will be stored
/// @return None
///
void libDS3234_GetTemperature(uint16_t *temperature)
{
    uint8_t data;
    
    ReadRegister(REG_TEMP_MSB, &data);
    *temperature = data * 100;
    
    ReadRegister(REG_TEMP_LSB, &data);
    *temperature += (data >> 6);
}

///
/// @brief Get the current month
///
/// @param  *month Pointer to variable where the month will be stored
/// @return FALSE  If read of month failed
/// @return SUCCESS If read of month succeeded
///
bool libDS3234_GetMonth(uint8_t *month)
{
    bool status = FALSE;
    uint8_t register_data;

    if(ReadRegister(REG_MONTH_CENTURY, &register_data) == TRUE)
    {
        *month = BCDToDecimal(register_data & MONTH_MASK);
        status = TRUE;
    }
    return status;
}

///
/// @brief Get the current date
///
/// @param  *date Pointer to variable where the date will be stored(1-31)
/// @return FALSE  If read failed
/// @return SUCCESS If read succeeded
///
bool libDS3234_GetDate(uint8_t *date)
{
    return GetDecimalRegisterValue(REG_DATE, date);
}

///
/// @brief Get the current day in week
///
/// @param  *day Pointer to variable where the day will be stored(1-7)
/// @return FALSE  If read failed
/// @return SUCCESS If read succeeded
///
bool libDS3234_GetDay(uint8_t *day)
{
    return GetDecimalRegisterValue(REG_DAY, day);
}

///
/// @brief Get the current hour
///
/// @param  *hour Pointer to variable where the hour will be stored
/// @return FALSE  If read failed
/// @return SUCCESS If read succeeded
///
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

///
/// @brief Set the current hour
///
/// @param  hour New hour to set
/// @return FALSE  If set failed
/// @return SUCCESS If set succeeded
///
bool libDS3234_SetHour(uint8_t hour)
{
    return SetDecimalRegisterValue(REG_HOUR, hour);
}

///
/// @brief Get the current minute
///
/// @param  *date Pointer to variable where the minute will be stored
/// @return FALSE  If read failed
/// @return SUCCESS If read succeeded
///
bool libDS3234_GetMinutes(uint8_t *minutes)
{
    return GetDecimalRegisterValue(REG_MINUTES, minutes);
}

bool libDS3234_SetMinutes(uint8_t minutes)
{
    return SetDecimalRegisterValue(REG_MINUTES, minutes);
}

///
/// @brief Get the current second
///
/// @param  *date Pointer to variable where the second will be stored
/// @return FALSE  If read failed
/// @return SUCCESS If read succeeded
///
bool libDS3234_GetSeconds(uint8_t *seconds)
{
    return GetDecimalRegisterValue(REG_SECONDS, seconds);
}

bool libDS3234_SetSeconds(uint8_t seconds)
{
    return SetDecimalRegisterValue(REG_SECONDS, seconds);
}

///
/// @brief Get the current hour mode, 12 or 24
///
/// @param  *hour_mode Pointer to variable where the hour mode will be stored
/// @return FALSE  If read failed
/// @return SUCCESS If read succeeded
///
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

///
/// @brief  Write data to SRAM. The SRAM is powered by the RTC battery so data is
///         persistent between power cycles.
///
/// @param  address Address in flash to write
/// @param  *data Pointer to data to write
/// @param  length Number of bytes to write
/// @return FALSE  If write failed
/// @return SUCCESS If write succeeded
///
bool libDS3234_WriteToSRAM(uint8_t address, uint8_t *data, uint8_t length)
{
    bool status = FALSE;
    uint8_t index;
    
    if ((uint16_t)address + (uint16_t)length <= SRAM_SIZE)
    {
        WriteRegister(REG_SRAM_ADDRESS, address);
        for (index = 0; index < length; ++index)
        {
            //SRAM address is auto incremented after each write            
            WriteRegister(REG_SRAM_DATA, data[index]);
        }
        status = TRUE;
    }    
    return status;
}

///
/// @brief  Read data from SRAM. The SRAM is powered by the RTC battery so data is
///         persistent between power cycles.
///
/// @param  address Address in flash to read
/// @param  *data Pointer to buffer where data will be stored
/// @param  length Number of bytes to read
/// @return FALSE  If read failed
/// @return SUCCESS If read succeeded
///
bool libDS3234_ReadFromSRAM(uint8_t address, uint8_t *data, uint8_t length)
{
    bool status = FALSE;
    uint8_t index;
    
    if ((uint16_t)address + (uint16_t)length <= SRAM_SIZE)
    {
        WriteRegister(REG_SRAM_ADDRESS, address);
        for (index = 0; index < length; ++index)
        {
            //SRAM address is auto incremented after each read            
            ReadRegister(REG_SRAM_DATA, &data[index]);     
        }
        status = TRUE;
    }
    return status;    
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

///
/// @brief	Get the value of a register as a decimal value. The register must
///			contain packed BCD-data.
///
///
/// @param	address Address to register
/// @param	value Pointer to variable where the value will be stored.
/// @return bool Status of operation
///
static bool GetDecimalRegisterValue(uint8_t address, uint8_t *value)
{
    bool status = FALSE;
    uint8_t register_data;
        
    if(ReadRegister(address, &register_data) == TRUE)
    {
        *value = BCDToDecimal(register_data);
        status = TRUE;
    }
    return status;
}

static bool SetDecimalRegisterValue(uint8_t address, uint8_t value)
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
    
    if(RegisterAddressValid(address))
    {
        libSPI_WriteByte(address|WRITE_ADDRESS, &PreCallback, NULL);
        libSPI_WriteByte(register_data, NULL, &PostCallback);
        status = TRUE;
    }
    return status;
}

static bool ReadRegister(uint8_t address, uint8_t *register_data)
{
    bool status = FALSE;
    
    if(RegisterAddressValid(address))
    {
        libSPI_WriteByte(address|READ_ADDRESS, &PreCallback, NULL);
        libSPI_ReadByte(register_data, NULL, &PostCallback);
        status = TRUE;
    }
    return status;
}

static bool RegisterAddressValid(uint8_t address)
{
    return (address <= REG_SRAM_DATA);
}

static void PreCallback(void)
{
    libSPI_SetMode(SPIMODE);
    PORTC &= ~(1 << SS); //Pull SS low to select device
    return;
}

static void PostCallback(void)
{
    PORTC |= (1 << SS); //Pull SS high to release device
    return;
}

static void DumpRegisterValues(void)
{
    uint8_t reg_addr;
    uint8_t reg_value;
    
    DEBUG("**DS3234 registers**\r\n");
    for (reg_addr = 0x00; reg_addr <= REG_DISABLE_TEMP; ++ reg_addr)
    {
        if (ReadRegister(reg_addr, &reg_value) == TRUE)
        {
            DEBUG("Addr: 0x%02X, Value: 0x%02X\r\n", reg_addr, reg_value);
        } 
        else
        {
            DEBUG("Addr: 0x--, Value: 0x--\r\n");  
        }                       
    }
    return;
}


