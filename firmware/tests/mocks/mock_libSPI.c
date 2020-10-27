/**
 * @file   mock_libSPI.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2020-10-21 (Last edit)
 * @brief  Mock functions.
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

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>

#include "libSPI.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void __wrap_libSPI_Init(uint8_t spi_mode)
{
}

void __wrap_libSPI_WriteByte(uint8_t data, libSPI_callback_type pre_callback, libSPI_callback_type post_callback)
{
    check_expected(data);
}

void __wrap_libSPI_Write(const void *data_p, size_t length, libSPI_callback_type pre_callback, libSPI_callback_type post_callback)
{
}

void __wrap_libSPI_ReadByte(uint8_t *data_p, libSPI_callback_type pre_callback, libSPI_callback_type post_callback)
{
    *data_p = mock_type(uint8_t);
}

void __wrap_libSPI_Read(void *data_p, size_t length, libSPI_callback_type pre_callback, libSPI_callback_type post_callback)
{
}

void __wrap_libSPI_SetAsMaster(void)
{
}

void __wrap_libSPI_SetMode(uint8_t mode)
{
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////
