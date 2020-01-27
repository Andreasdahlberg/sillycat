/**
 * @file   Bit.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2020-01-27 (Last edit)
 * @brief  Bit manipulation utility
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

#include <stddef.h>

#include "Bit.h"
#include "sc_assert.h"

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

void Bit_Set(uint8_t bit_index, bool state, uint8_t *data)
{
    sc_assert(bit_index < 8);
    sc_assert(data != NULL);

    if (state == true)
    {
        *data |= (1 << bit_index);
    }
    else
    {
        *data &= ~(1 << bit_index);
    }
}

bool Bit_Get(uint8_t bit_index, const uint8_t *data)
{
    sc_assert(bit_index < 8);
    sc_assert(data != NULL);

    return (bool)(*data & (1 << bit_index));
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////
