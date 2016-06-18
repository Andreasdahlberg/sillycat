/**
 * @file   libNHD223.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2016-04-03 (Last edit)
 * @brief  Implementation of NHD223-library.
 *
 * Low level interface for the NHD?2.23 graphic OLED display module.
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

static void ClearDisplay();
static void WriteByte(uint8_t data);
static void EnableControlPin(bool state, uint8_t pin_index);
static void SetDataDirection(data_direction_type direction);

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

///
/// @brief Init display hardware and clear the display.
///
/// @param  None
/// @return None
///
void libNHD223_Init(void)
{
    //Configure data pins as outputs and pull low
    DDRD = 0xFF;
    PORTD = 0x00;

    DDRC |= ((1 << ENABLE) | (1 << RESET) | (1 << READWRITE) |
             (1 << REGSELECT) | (1 << CHIPSELECT));

    libNHD223_ResetDisplay();
    ClearDisplay();
    libNHD223_ResetDisplay();
    SelectDevice(false);
    EnableDataLatch(false);

    libNHD223_SetColumnAddressRange(0, 127);
    libNHD223_SetPageAddressRange(0, 3);

    return;
}

///
/// @brief Write command to display.
///
/// @param  command Command to write.
/// @return None
///
void libNHD223_WriteCommand(uint8_t command)
{
    //Set write mode
    EnableControlPin(false, READWRITE);

    EnableCommandMode(true);

    WriteByte(command);
    return;
}

///
/// @brief Write data to display.
///
/// @param  data Data to write.
/// @return None
///
void libNHD223_WriteData(uint8_t data)
{
    //Set write mode
    EnableControlPin(false, READWRITE);

    EnableCommandMode(false);

    WriteByte(data);
    return;
}

///
/// @brief Reset the display.
///
/// Reset the display by pulling the reset pin high for 5 us.
/// This function is blocking.
///
/// @param  None
/// @return None
///
void libNHD223_ResetDisplay(void)
{
    EnableReset(true);
    _delay_us(5);
    EnableReset(false);
    return;
}

///
/// @brief Set the active page address.
///
/// @param  page_address Page address to set.
/// @return None
///
void libNHD223_SetPageAddress(uint8_t page_address)
{
    sc_assert(page_address < MAX_PAGES);

    libNHD223_WriteCommand(SSD1305_SET_PAGE | page_address);
    return ;
}

///
/// @brief Set the page address range.
///
/// @param  start_page Address to starting page.
/// @param  end_page Address to ending page.
/// @return None
///
void libNHD223_SetPageAddressRange(uint8_t start_page, uint8_t end_page)
{
    sc_assert(start_page <= end_page);

    libNHD223_WriteCommand(SSD1305_SETSTARTPAGE);
    libNHD223_WriteCommand(start_page);
    libNHD223_WriteCommand(end_page);

    return;
}

///
/// @brief Set the column address range.
///
/// @param  start_address Address to starting column.
/// @param  end_address Address to ending column.
/// @return None
///
void libNHD223_SetColumnAddressRange(uint8_t start_address, uint8_t end_address)
{
    sc_assert(start_address <= end_address);

    libNHD223_WriteCommand(SSD1305_SETSTARTCOLUMN);
    libNHD223_WriteCommand(start_address);
    libNHD223_WriteCommand(end_address);

    return;
}

///
/// @brief Set the active column address.
///
/// @param  column_address Column address to set.
/// @return None
///
void libNHD223_SetColumnAddress(uint8_t column_address)
{
    libNHD223_WriteCommand(SSD1305_SETLOWCOLUMN | (column_address & 0x0F));
    libNHD223_WriteCommand(SSD1305_SETHIGHCOLUMN | ((column_address >> 4) & 0x0F));
    return;
}

///
/// @brief Read a byte from display memory.
///
/// @param  *data Pointer to byte where the result will be stored.
/// @return None
///
//TODO: Fix this function, not working properly.
void libNHD223_ReadByte(uint8_t *data)
{
    sc_assert(data != NULL);

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
    *data = PIND;

    return;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void ClearDisplay(void)
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

    return;
}

static void WriteByte(uint8_t data)
{
    SetDataDirection(OUTPUT);
    EnableDataLatch(true);
    SelectDevice(true);

    PORTD = data;
    SelectDevice(false);
    EnableDataLatch(false);

    return;
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
    return;
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
    return;
}