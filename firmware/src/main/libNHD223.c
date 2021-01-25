/**
 * @file   libNHD223.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2021-01-25 (Last edit)
 * @brief  NHD2.23(SSD1305) graphic OLED display driver.
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

#include "common.h"
#include <util/delay.h>
#include "libSPI.h"
#include "libDebug.h"
#include "Board.h"
#include "libNHD223.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define MAX_PAGES 8

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

static inline void SelectDevice(void);
static inline void ReleaseDevice(void);
static inline void PullReset(void);
static inline void ReleaseReset(void);
static inline void EnableDataLatch(void);
static inline void DisableDataLatch(void);
static inline void SetCommandMode(void);
static inline void SetDataMode(void);
static inline void SetWriteMode(void);
static inline void SetReadMode(void);
static void WriteByte(uint8_t data);
static void ClearDisplay(void);

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void libNHD223_Init(void)
{
    libNHD223_ResetDisplay();
    ClearDisplay();
    libNHD223_ResetDisplay();
    ReleaseDevice();
    DisableDataLatch();
}

void libNHD223_WriteCommand(uint8_t command)
{
    SetWriteMode();
    SetCommandMode();
    WriteByte(command);
}

void libNHD223_WriteData(uint8_t data)
{
    SetWriteMode();
    SetDataMode();
    WriteByte(data);
}

void libNHD223_ResetDisplay(void)
{
    /* According to the SSD1305 datasheet, the minimum reset pulse width is 3 µs. */
    const uint8_t reset_delay_us = 5;
    PullReset();
    _delay_us(reset_delay_us);
    ReleaseReset();
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
    const uint8_t lower_nibble_mask = 0x0F;
    libNHD223_WriteCommand(SSD1305_SETLOWCOLUMN | (address & lower_nibble_mask));
    libNHD223_WriteCommand(SSD1305_SETHIGHCOLUMN | ((address >> 4) & lower_nibble_mask));
}

void libNHD223_SetColumnAddressRange(uint8_t start_address, uint8_t end_address)
{
    sc_assert(start_address <= end_address);

    libNHD223_WriteCommand(SSD1305_SETSTARTCOLUMN);
    libNHD223_WriteCommand(start_address);
    libNHD223_WriteCommand(end_address);
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static inline void SelectDevice(void)
{
    Board_NHD223_SetChipSelectLow();
}

static inline void ReleaseDevice(void)
{
    Board_NHD223_SetChipSelectHigh();
}

static inline void PullReset(void)
{
    Board_NHD223_SetResetLow();
}

static inline void ReleaseReset(void)
{
    Board_NHD223_SetResetHigh();
}

static inline void EnableDataLatch(void)
{
    Board_NHD223_SetEnableHigh();
}

static inline void DisableDataLatch(void)
{
    Board_NHD223_SetEnableLow();
}

static inline void SetCommandMode(void)
{
    Board_NHD223_SetRegSelectLow();
}

static inline void SetDataMode(void)
{
    Board_NHD223_SetRegSelectHigh();
}

static inline void SetWriteMode(void)
{
    Board_NHD223_SetReadWriteLow();
    Board_NHD223_SetDataPinsAsOutputs();
}

static inline void SetReadMode(void)
{
    Board_NHD223_SetReadWriteHigh();
}

static void WriteByte(uint8_t data)
{
    EnableDataLatch();
    SelectDevice();
    Board_NHD223_SetDataPins(data);
    ReleaseDevice();
    DisableDataLatch();
}

static void ClearDisplay(void)
{
    for (uint8_t page = 0; page < 4; ++page)
    {
        libNHD223_SetPageAddress(page);

        for(uint8_t column = 0; column < 128; ++column)
        {
            libNHD223_WriteData(0x00);
        }
    }
}
