/**
 * @file   driverNVM.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2021-01-14 (Last edit)
 * @brief  Virtual NVM driver.
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

#ifndef DRIVERNVM_H_
#define DRIVERNVM_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

/**
 * Write data to NVM.
 *
 * @param address Address in memory to write. Virtual address starting at zero.
 * @param data_p  Pointer to data to write.
 * @param length  Number of bytes to write.
 *
 * @return True if address and length is valid, otherwise false.
 */
bool driverNVM_Write(size_t address, const void *data_p, size_t length);

/**
 * Read data from NVM.
 *
 * @param address Address in memory to read. Virtual address starting at zero.
 * @param data_p  Pointer to buffer where data will be stored.
 * @param length  Number of bytes to read.
 *
 * @return True if address and length is valid, otherwise false.
 */
bool driverNVM_Read(size_t address, void *data_p, size_t length);

#endif
