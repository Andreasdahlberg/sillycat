/**
 * @file   Filter.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-02-11 (Last edit)
 * @brief  Implementation of an exponential moving average(EMA) filter.
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

#include "Filter.h"
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

void Filter_Init(struct filter_t *self_p, int16_t initial_value, uint16_t alpha)
{
    sc_assert(self_p != NULL);

    self_p->value = initial_value;
    self_p->alpha = alpha;
}

void Filter_Process(struct filter_t *self_p, int16_t sample)
{
    sc_assert(self_p != NULL);

    /* Use 32-bits to prevent overflow. */
    uint32_t tmp = (int32_t)self_p->alpha * (int32_t)sample +
                   (int32_t)(65536 - self_p->alpha) * (int32_t)self_p->value;

    /* Scale back to 16-bit. */
    self_p->value = (int16_t)((tmp + 32768) / 65536);
}

int16_t Filter_Output(const struct filter_t *self_p)
{
    sc_assert(self_p != NULL);

    return self_p->value;
}

bool Filter_IsInitialized(const struct filter_t *self_p)
{
    sc_assert(self_p != NULL);

    return self_p->alpha != 0;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////
