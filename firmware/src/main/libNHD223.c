/**
 * @file   libNHD223.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-10-16 (Last edit)
 * @brief  NHD2.23 graphic OLED display driver.
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

//NOTE: Include before all other headers
#include "common.h"

#include <util/delay.h>

#include "libNHD223.h"
#include "libSPI.h"
#include "libDebug.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define ENABLE      DDC1
#define RESET       DDC2
#define READWRITE   DDC3
#define REGSELECT   DDC4
#define CHIPSELECT  DDC5

#define MAX_PAGES 8

#define EnableCommandMode(state) EnableControlPin(!state, REGSELECT)
#define EnableDataLatch(state)  EnableControlPin(state, ENABLE)
#define EnableReset(state)  EnableControlPin(!state, RESET)
#define SelectDevice(state) EnableControlPin(!state, CHIPSELECT)

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

typedef enum
{
    INPUT = 0,
    OUTPUT
} data_direction_type;

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

static void InitHardware(void);
static void ClearDisplay(void);
static void WriteByte(uint8_t data);
static void EnableControlPin(bool state, uint8_t pin_index);
static void SetDataDirection(data_direction_type direction);

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void libNHD223_Init(void)
{
    InitHardware();
    libNHD223_ResetDisplay();
    ClearDisplay();
    libNHD223_ResetDisplay();
    SelectDevice(false);
    EnableDataLatch(false);
}

void libNHD223_WriteCommand(uint8_t command)
{
    //Set write mode
    EnableControlPin(false, READWRITE);

    EnableCommandMode(true);

    WriteByte(command);
}

void libNHD223_WriteData(uint8_t data)
{
    //Set write mode
    EnableControlPin(false, READWRITE);

    EnableCommandMode(false);

    WriteByte(data);
}

void libNHD223_ResetDisplay(void)
{
    EnableReset(true);
    _delay_us(5);
    EnableReset(false);
}

void libNHD223_SetHorizontalAddressingMode(void)
{
    libNHD223_WriteCommand(SSD1305_ADDRESSINGMODE);
    libNHD223_WriteCommand(SSD1305_HORIZONTALADDRESSINGMODE);
}

void libNHD223_SetPageAddress(uint8_t address)
{
    sc_assert(address < MAX_PAGES);

    libNHD223_WriteCommand(SSD1305_SET_PAGE | address);
}

void libNHD223_SetPageAddressRange(uint8_t start_address, uint8_t end_address)
{
    sc_assert(start_address <= end_address);

    libNHD223_WriteCommand(SSD1305_SETSTARTPAGE);
    libNHD223_WriteCommand(start_address);
    libNHD223_WriteCommand(end_address);
}

void libNHD223_SetColumnAddress(uint8_t address)
{
    libNHD223_WriteCommand(SSD1305_SETLOWCOLUMN | (address & 0x0F));
    libNHD223_WriteCommand(SSD1305_SETHIGHCOLUMN | ((address >> 4) & 0x0F));
}

void libNHD223_SetColumnAddressRange(uint8_t start_address, uint8_t end_address)
{
    sc_assert(start_address <= end_address);

    libNHD223_WriteCommand(SSD1305_SETSTARTCOLUMN);
    libNHD223_WriteCommand(start_address);
    libNHD223_WriteCommand(end_address);
}

void libNHD223_ReadByte(uint8_t *data_p)
{
    sc_assert(data_p != NULL);

    SetDataDirection(INPUT);
    SelectDevice(true);
    EnableCommandMode(false);
    EnableControlPin(true, READWRITE);

    EnableDataLatch(true);
    EnableDataLatch(false);
    EnableDataLatch(true);
    EnableDataLatch(false);

    EnableControlPin(false, READWRITE);
    SelectDevice(false);
    *data_p = PIND;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static void InitHardware(void)
{
    /* Configure data pins as outputs and pull low. */
    DDRD = 0xFF;
    PORTD = 0x00;
    DDRC |= ((1 << ENABLE) | (1 << RESET) | (1 << READWRITE) |
             (1 << REGSELECT) | (1 << CHIPSELECT));
}

static void ClearDisplay(void)
{
    uint8_t page;
    uint8_t column;

    for (page = 0; page < 4; ++page)
    {
        libNHD223_SetPageAddress(page);

        for(column = 0; column < 128; ++column)
        {
            libNHD223_WriteData(0x00);
        }
    }
}

static void WriteByte(uint8_t data)
{
    SetDataDirection(OUTPUT);
    EnableDataLatch(true);
    SelectDevice(true);

    PORTD = data;
    SelectDevice(false);
    EnableDataLatch(false);
}

static void SetDataDirection(data_direction_type direction)
{
    if (direction == INPUT)
    {
        DDRD = 0x00;
    }
    else
    {
        //Output
        DDRD = 0xFF;
        PORTD = 0x00;
    }
}

static void EnableControlPin(bool state, uint8_t pin_index)
{
    if (state == false)
    {
        PORTC &= ~(1 << pin_index);
    }
    else
    {
        PORTC |= (1 << pin_index);
    }
}
