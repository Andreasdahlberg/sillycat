/**
 * @file   CRC.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2017-03-05 (Last edit)
 * @brief  Implementation of functions to calculate CRC.
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

#include "CRC.h"
#include "sc_assert.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define CRC_POLY        0x8005 // X^16 + X^15 + X^2 + 1
#define CRC_INIT_VALUE	0x0000

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

///
/// @brief Calculate CRC-16.
///
/// @param  *data Pointer to data to calculate CRC for.
/// @param  length Number of bytes in data.
/// @return uint16_t CRC-16 value of data.
uint16_t CalculateCRC_16(void *data, size_t length)
{
	sc_assert(data != NULL);

	uint16_t crc = CRC_INIT_VALUE;
	uint8_t *ptr = (uint8_t*)data;

	size_t byte_idx;
	for (byte_idx = 0; byte_idx < length; ++byte_idx)
	{
		crc ^= ((uint16_t)(*ptr)) << 8;
		++ptr;

		uint8_t bit_idx;
		for (bit_idx = 8; bit_idx > 0; --bit_idx)
 		{
			if (crc & (1 << 15))
			{
				crc <<= 1;
				crc ^= CRC_POLY;
			}
			else
			{
				crc <<= 1;
			}
		}
	}

	return crc;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////
