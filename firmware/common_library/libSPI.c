/**
 * @file   libSPI.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2016-01-31 (Last edit)
 * @brief  Implementation of SPI-library.
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
#include <stdio.h>
#include <string.h>

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

static SPI_status spi_status;
static bool slave_active;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

bool SPIBusy();

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

//TODO: Create functions for changing data order, clock freq and to enable/disable SPI

///
/// @brief Init the SPI module
///
/// @param  None
/// @return None
///
void libSPI_Init(uint8_t spi_mode)
{
    //Always set SS as output even if not used, otherwise the device can't
    //act as master.
    DDRB |= ((1 << MOSI) | (1 << SCK) | (1 << SS));
    DDRB &= ~(1 << MISO);

    SPCR = (1 << SPR0);

    libSPI_SetMaster(true);

    if (libSPI_SetMode(spi_mode) == true)
    {
        SPCR |= (1 << SPE);
        spi_status = IDLE;

        INFO("Init done");
    }
    else
    {
        ERROR("Failed to init SPI, invalid mode: %u", spi_mode);
    }

    slave_active = false;
    return;
}

///
/// @brief Perform a blocking write of a single byte.
///
/// @param  data_byte Byte to write
/// @param  pre_write Pointer to function called before writing
/// @param  post_write Pointer to function called after writing
/// @return None
///
void libSPI_WriteByte(uint8_t data_byte, libSPI_callback_type pre_write,
                      libSPI_callback_type post_write)
{
    if (pre_write != NULL)
    {
        pre_write();
    }

    SPDR = data_byte;
    while (!(SPSR & (1 << SPIF)))
    {
    }

    if (post_write != NULL)
    {
        post_write();
    }
    return;
}

///
/// @brief Perform a blocking read of a single byte.
///
/// @param  data_byte Pointer to byte where the read byte will be stored
/// @param  pre_read Pointer to function called before reading
/// @param  post_read Pointer to function called after reading
/// @return None
///
void libSPI_ReadByte(uint8_t *data_byte, libSPI_callback_type pre_read,
                     libSPI_callback_type post__read)
{
    if (pre_read != NULL)
    {
        pre_read();
    }

    SPDR = 0x00; //Dummy data to enable SPI-clock
    while (!(SPSR & (1 << SPIF)))
    {
    }
    *data_byte = SPDR;

    if (post__read != NULL)
    {
        post__read();
    }
    return;
}

///
/// @brief Get the status of the SPI-bus
///
/// @param None
/// @return SPI_status SPI-bus status
///
SPI_status libSPI_GetStatus(void)
{
    return spi_status;
}

///
/// @brief Select if master or slave mode
///
/// @param bool true if master, else slave  //TODO: Fix this description
/// @return None
///
void libSPI_SetMaster(bool master_value)
{
    if (master_value == true)
    {
        SPCR |= (1 << MSTR);
    }
    else
    {
        SPCR &= ~(1 << MSTR);
    }
    return;
}

///
/// @brief Set the SPI transfer mode
///
/// @param mode SPI mode, (0-3)
/// @return bool Status of operation
///
bool libSPI_SetMode(uint8_t mode)
{
    bool status = true;
    switch (mode)
    {
        case 0:
            //Clock low when idle, sample on rising edge
            SPCR &= ~((1 << CPOL) | (1 << CPHA));
            break;
        case 1:
            //Clock low when idle, sample on falling edge
            SPCR &= ~(1 << CPOL);
            SPCR |= (1 << CPHA);
            break;
        case 2:
            //Clock high when idle, sample on rising edge
            SPCR |= (1 << CPOL);
            SPCR &= ~(1 << CPHA);
            break;
        case 3:
            //Clock high when idle, sample on falling edge
            SPCR |= ((1 << CPOL) | (1 << CPHA));
            break;
        default:
            //Invalid mode
            WARNING("Invalid SPI-mode");
            status = false;
            break;
    }
    return status;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

//TODO: Fix this description
///
/// @brief Set slave to active, this prevents other modules to use the SPI-bus. Keep in mind that there is now control if its the same module...
///
///
/// @param state true if SPI is active, otherwise false
/// @return bool Status of operation
///
bool SetActiveSlave(bool state)
{
    bool status = false;
    if (slave_active == false)
    {
        slave_active = true;
        status = true;
    }
    else if (slave_active == true && state == false)
    {
        slave_active = false;
        status = true;
    }
    return status;
}

///
/// @brief Check if the SPI-bus is busy
///
/// @param None
/// @return bool Status of SPI-bus
///
bool SPIBusy(void)
{
    return ((SPSR & (1 << SPIF)) == 0);
}
