/**
 * @file   libSPI.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2019-04-27 (Last edit)
 * @brief  Driver for the ATmega328 SPI-peripheral.
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
#include "libDebug.h"
#include "libSPI.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define SS      DDB2
#define MOSI    DDB3
#define MISO    DDB4
#define SCK     DDB5

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

static inline void InitializePins(void);
static inline void SetClockRateDividerTo16(void);
static inline void EnableSPI(void);
static inline void TryExecuteCallback(libSPI_callback_type callback);
static inline void ReadByte(uint8_t *data_p);
static inline void WriteByte(uint8_t data);

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void libSPI_Init(uint8_t spi_mode)
{
    InitializePins();
    SetClockRateDividerTo16();
    libSPI_SetAsMaster();
    libSPI_SetMode(spi_mode);
    EnableSPI();

    INFO("SPI driver initialized");
}

void libSPI_WriteByte(uint8_t data,
                      libSPI_callback_type pre_callback,
                      libSPI_callback_type post_callback)
{
    TryExecuteCallback(pre_callback);
    WriteByte(data);
    TryExecuteCallback(post_callback);
}

void libSPI_Write(const void *data_p,
                  size_t length,
                  libSPI_callback_type pre_callback,
                  libSPI_callback_type post_callback)
{
    sc_assert(data_p != NULL);

    TryExecuteCallback(pre_callback);

    const uint8_t *data_ptr = (uint8_t *)data_p;
    for (uint8_t i = 0; i < length; ++i)
    {
        WriteByte(*data_ptr);
        ++data_ptr;
    }

    TryExecuteCallback(post_callback);
}

void libSPI_ReadByte(uint8_t *data_p,
                     libSPI_callback_type pre_callback,
                     libSPI_callback_type post_callback)
{
    TryExecuteCallback(pre_callback);
    ReadByte(data_p);
    TryExecuteCallback(post_callback);
}

void libSPI_Read(void *data_p,
                 size_t length,
                 libSPI_callback_type pre_callback,
                 libSPI_callback_type post_callback)
{
    sc_assert(data_p != NULL);

    TryExecuteCallback(pre_callback);

    uint8_t *data_ptr = (uint8_t *)data_p;
    for (uint8_t i = 0; i < length; ++i)
    {
        ReadByte(data_ptr);
        ++data_ptr;
    }

    TryExecuteCallback(post_callback);
}

void libSPI_SetAsMaster(void)
{
    SPCR |= (1 << MSTR);
}

void libSPI_SetMode(uint8_t mode)
{
    switch (mode)
    {
        case 0:
            /* Clock low when idle, sample on rising edge. */
            SPCR &= ~((1 << CPOL) | (1 << CPHA));
            break;

        case 1:
            /* Clock low when idle, sample on falling edge. */
            SPCR &= ~(1 << CPOL);
            SPCR |= (1 << CPHA);
            break;

        case 2:
            /* Clock high when idle, sample on rising edge. */
            SPCR |= (1 << CPOL);
            SPCR &= ~(1 << CPHA);
            break;

        case 3:
            /* Clock high when idle, sample on falling edge. */
            SPCR |= ((1 << CPOL) | (1 << CPHA));
            break;

        default:
            sc_assert_fail();
            break;
    }
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static inline void InitializePins(void)
{
    /**
     * Always set SS as output even if not used, otherwise the device can't
     * act as master.
     */
    DDRB |= ((1 << MOSI) | (1 << SCK) | (1 << SS));
    DDRB &= ~(1 << MISO);
}

static inline void SetClockRateDividerTo16(void)
{
    SPCR = (1 << SPR0);
}

static inline void EnableSPI(void)
{
    SPCR |= (1 << SPE);
}

static inline void TryExecuteCallback(libSPI_callback_type callback)
{
    if (callback != NULL)
    {
        callback();
    }
}

static inline void ReadByte(uint8_t *data_p)
{
    SPDR = 0x00;
    while (!(SPSR & (1 << SPIF)))
    {
        /**
         * Wait for read to complete.
         */
    }
    *data_p = SPDR;
}

static inline void WriteByte(uint8_t data)
{
    SPDR = data;
    while (!(SPSR & (1 << SPIF)))
    {
        /**
         * Wait for write to complete.
         */
    }
}
