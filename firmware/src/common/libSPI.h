/**
 * @file   libSPI.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2016-04-24 (Last edit)
 * @brief  Header for SPI-library.
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

#ifndef LIBSPI_H_
#define LIBSPI_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include <avr/io.h>
#include "common.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

typedef enum SPI_status
{
    IDLE = 0,
    READ,
    READ_DONE,
    WRITE,
    WRITE_DONE,
} SPI_status;

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

typedef void (*libSPI_callback_type)(void);

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

void libSPI_Init(uint8_t spi_mode);
void libSPI_SetMaster(bool master_value);
SPI_status libSPI_GetStatus(void);
bool libSPI_SetMode(uint8_t mode);
bool SetActiveSlave(bool state);
void libSPI_WriteByte(uint8_t data_byte, libSPI_callback_type pre_write,
                      libSPI_callback_type post_write);
void libSPI_Write(void *data, size_t length, libSPI_callback_type pre_write,
                  libSPI_callback_type post_write);
void libSPI_ReadByte(uint8_t *data_byte, libSPI_callback_type pre_write,
                     libSPI_callback_type post_write);
void libSPI_Read(void *buffer, size_t length, libSPI_callback_type pre_read,
                 libSPI_callback_type post_read);

#endif /* LIBSPI_H_ */