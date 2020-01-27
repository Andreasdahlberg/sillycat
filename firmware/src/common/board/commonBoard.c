/**
 * @file   commonBoard.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2020-01-27 (Last edit)
 * @brief  Board support package for the main unit.
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
#include <avr/io.h>
#include <avr/wdt.h>
#include "libSPI.h"
#include "Board.h"
#include "commonBoard.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

static inline void SetRFM69ResetAsOutput(void);
static inline void SetRFM69ResetAsInput(void);
static inline void SetRFM69ResetHigh(void);
static inline void SetRFM69ResetLow(void);

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//INTERUPT SERVICE ROUTINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void Board_Init(void)
{
    Board_RTC_Init();
    Board_RFM69_Init();
}

void Board_RTC_Init(void)
{
    /* Set SS as output and pull high to release device. */
    RTC_SPI_DDR |= (1 << RTC_SPI_SS);
    RTC_SPI_PORT |= (1 << RTC_SPI_SS);
}

void Board_RTC_SPIPreCallback(void)
{
    libSPI_SetMode(RTC_SPI_MODE);
    RTC_SPI_PORT &= ~(1 << RTC_SPI_SS);
}

void Board_RTC_SPIPostCallback(void)
{
    RTC_SPI_PORT |= (1 << RTC_SPI_SS);
}

void Board_RFM69_Init(void)
{
    /* Set SS as output and pull high to release device. */
    RFM69_SPI_DDR |= (1 << RFM69_SPI_SS);
    RFM69_SPI_PORT |= (1 << RFM69_SPI_SS);

    SetRFM69ResetAsInput();
    SetRFM69ResetLow();
}

void Board_RFM69_SPIPreCallback(void)
{
    libSPI_SetMode(RFM69_SPI_MODE);
    RFM69_SPI_PORT &= ~(1 << RFM69_SPI_SS);
}

void Board_RFM69_SPIPostCallback(void)
{
    RFM69_SPI_PORT |= (1 << RFM69_SPI_SS);
}

void Board_RFM69_PullReset(void)
{
    SetRFM69ResetHigh();
    SetRFM69ResetAsOutput();
}

void Board_RFM69_ReleaseReset(void)
{
    SetRFM69ResetAsInput();
    SetRFM69ResetLow();
}

void Board_SoftReset(void)
{
    wdt_enable(WDTO_15MS);
    while (1)
    {
        /**
         * Wait for the watchdog to reset the device.
         */
    }
}

bool Board_IsGlobalInterruptEnabled(void)
{
    return ((SREG & (1 << 7)) > 0);
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static inline void SetRFM69ResetAsOutput(void)
{
    RFM69_RESET_DDR |= (1 << RFM69_RESET_PIN);
}

static inline void SetRFM69ResetAsInput(void)
{
    RFM69_RESET_DDR &= ~(1 << RFM69_RESET_PIN);
}

static inline void SetRFM69ResetHigh(void)
{
    RFM69_RESET_PORT |= (1 << RFM69_RESET_PIN);
}

static inline void SetRFM69ResetLow(void)
{
    RFM69_RESET_PORT &= ~(1 << RFM69_RESET_PIN);
}
