/**
 * @file   libSPI.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-11-30 (Last edit)
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

#ifndef LIBSPI_H_
#define LIBSPI_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include "stdint.h"
#include "stdbool.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

typedef void (*libSPI_callback_type)(void);

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

/**
 * Initialize the SPI driver.
 *
 * @param spi_mode SPI-mode(0-3).
 */
void libSPI_Init(uint8_t spi_mode);


/**
 * Perform a blocking write of a single byte.
 *
 * @param data          Byte to write.
 * @param pre_callback  Function call before writing.
 * @param post_callback Function call after writing.
 */
void libSPI_WriteByte(uint8_t data,
                      libSPI_callback_type pre_callback,
                      libSPI_callback_type post_callback);

/**
 * Perform a blocking write.
 *
 * @param data_p        Pointer to data to write.
 * @param length        Number of bytes to write.
 * @param pre_callback  Function call before writing.
 * @param post_callback Function call after writing.
 */
void libSPI_Write(const void *data_p,
                  size_t length,
                  libSPI_callback_type pre_callback,
                  libSPI_callback_type post_callback);

/**
 * Perform a blocking read of a single byte.
 *
 * @param data_p        Pointer to location where the byte will be stored.
 * @param pre_callback  Function call before writing.
 * @param post_callback Function call after writing.
 */
void libSPI_ReadByte(uint8_t *data_p,
                     libSPI_callback_type pre_callback,
                     libSPI_callback_type post_callback);

/**
 * Perform a blocking read.
 *
 * @param data_p        Pointer to location where the data will be stored.
 * @param length        Number of bytes to read.
 * @param pre_callback  Function call before writing.
 * @param post_callback Function call after writing.
 */
void libSPI_Read(void *data_p,
                 size_t length,
                 libSPI_callback_type pre_callback,
                 libSPI_callback_type post_callback);

/**
 * Set the driver to act as master on the SPI-bus.
 */
void libSPI_SetAsMaster(void);

/**
 * Set the SPI transfer mode.
 *
 * Mode Leading Edge        Trailing Edge
 *  0   Sample (Rising)     Setup (Falling)
 *  1   Setup (Rising)      Sample (Falling)
 *  2   Sample (Falling)    Setup (Rising)
 *  3   Setup (Falling)     Sample (Rising)
 *
 * @param mode SPI-mode.
 */
void libSPI_SetMode(uint8_t mode);

#endif
