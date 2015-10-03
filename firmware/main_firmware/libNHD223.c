/**
 * @file   libNHD223.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2015-10-03 (Last edit)
 * @brief  Implementation of NHD223-library.
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

//TODO: Define this in one place
#define F_CPU 8000000UL // 8 MHz

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include <util/delay.h>
#include "common.h"
#include "libNHD223.h"
#include "libSPI.h"
#include "libDebug.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define ENABLE		DDC1
#define RESET		DDC2
#define READWRITE	DDC3
#define REGSELECT	DDC4
#define CHIPSELECT	DDC5

#define MAX_PAGES 8

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

typedef enum
{
    INPUT = 0,
    OUTPUT
} data_direction_type;

typedef struct
{
    uint8_t height;
    uint8_t width;
    uint8_t pages;
    
} display_properties_type;


//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

void ClearDisplay();
static bool WriteByte(uint8_t data);
static void selectDevice(bool state);
static void enableDataLatch(bool state);
static void enableCommandMode(bool state);
static void enableControlPin(bool state, uint8_t pin_index);
static void enableReset(bool state);
void setDataDirection(data_direction_type direction);

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void libNHD223_Init()
{
    //Configure data pins as outputs and pull low
    DDRD = 0xFF;
    PORTD = 0x00;
    
    DDRC |= ((1 << ENABLE) | (1 << RESET) | (1 << READWRITE) |
             (1 << REGSELECT) | (1 << CHIPSELECT));
             
    libNHD223_ResetDisplay();
    ClearDisplay();
    libNHD223_ResetDisplay();
    selectDevice(FALSE);
    enableDataLatch(FALSE);
    
    libNHD223_SetColumnAddressRange(0, 127);
    libNHD223_SetPageAddressRange(0, 3);
}

void libNHD223_Update()
{
}


bool libNHD223_WriteCommand(uint8_t command)
{
    //Set write mode
    enableControlPin(FALSE, READWRITE);
    
    enableCommandMode(TRUE);
    
    WriteByte(command);
    return TRUE;	
}


bool libNHD223_WriteData(uint8_t data)
{
    //Set write mode
    enableControlPin(FALSE, READWRITE);
    
    enableCommandMode(FALSE);
    
    WriteByte(data);
    return TRUE;
}


void libNHD223_ResetDisplay()
{
    enableReset(TRUE);
    _delay_us(5);
    enableReset(FALSE);
}


bool libNHD223_SetPageAddress(uint8_t page_address)
{
    bool status = FALSE;
    
    if (page_address < MAX_PAGES)
    {
        status = libNHD223_WriteCommand(SSD1305_SET_PAGE | page_address);
    }
    
    return status;
}

bool libNHD223_SetPageAddressRange(uint8_t start_page, uint8_t end_page)
{
    bool status = FALSE;
    
    //if (start_page <= end_page && end_page < MAX_PAGES)
    //{
        libNHD223_WriteCommand(SSD1305_SETSTARTPAGE);
        libNHD223_WriteCommand(start_page);
        libNHD223_WriteCommand(end_page);	
        status = TRUE;		
    //}
    return status;
}

void libNHD223_SetColumnAddressRange(uint8_t start_address, uint8_t end_address)
{
        libNHD223_WriteCommand(SSD1305_SETSTARTCOLUMN);
        libNHD223_WriteCommand(start_address);	
        libNHD223_WriteCommand(end_address);					
}

void libNHD223_SetColumnAddress(uint8_t column_address)
{
        libNHD223_WriteCommand(SSD1305_SETLOWCOLUMN | (column_address & 0x0F));
        libNHD223_WriteCommand(SSD1305_SETHIGHCOLUMN | ((column_address >> 4) & 0x0F));
}

bool libNHD223_ReadByte(uint8_t *data)
{
    
    setDataDirection(INPUT);
    selectDevice(TRUE);
    enableCommandMode(FALSE);
    enableControlPin(TRUE, READWRITE);
    
    
    enableDataLatch(TRUE);
    enableDataLatch(FALSE);
    enableDataLatch(TRUE);
    enableDataLatch(FALSE);
    
    enableControlPin(FALSE, READWRITE);
    selectDevice(FALSE);
    *data = PIND;
    
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void ClearDisplay()
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

bool WriteByte(uint8_t data)
{

    setDataDirection(OUTPUT);
    enableDataLatch(TRUE);
    selectDevice(TRUE);

    PORTD = data;
    selectDevice(FALSE);
    enableDataLatch(FALSE);
    
    
    return TRUE;
}

void setDataDirection(data_direction_type direction)
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

static void enableCommandMode(bool state)
{
    //0: Command, 1: Data
    enableControlPin(!state, REGSELECT);
}


static void enableDataLatch(bool state)
{
    enableControlPin(state, ENABLE);
}

static void enableReset(bool state)
{
    //Reset is active low
    enableControlPin(!state, RESET);
}


static void selectDevice(bool state)
{
    //CS is active low
    enableControlPin(!state, CHIPSELECT);
}


static void enableControlPin(bool state, uint8_t pin_index)
{
    if (state == FALSE)
    {
        PORTC &= ~(1 << pin_index);
    }
    else
    {
        PORTC |= (1 << pin_index);
    }
}

