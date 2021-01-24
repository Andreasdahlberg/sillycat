/**
 * @file   Filter.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2021-01-24 (Last edit)
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

#ifndef FILTER_H_
#define FILTER_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define FILTER_ALPHA(a) ((uint16_t)((a) * UINT16_MAX))

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

struct filter_t
{
    int16_t value;
    uint16_t alpha;
};

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

/**
 * Initialize the exponential moving average(EMA) filter.
 *
 * @param self_p        Pointer to filter struct.
 * @param initial_value Initial value of the filter, e.g. the first sample.
 * @param alpha         Smoothing factor, a higher value discounts older
 *                      observations faster. Use `FILTER_ALPHA()` to convert
 *                      a floating point value to an integer at compile time.
 */
void Filter_Init(struct filter_t *self_p, int16_t initial_value, uint16_t alpha);

/**
 * Process the supplied sample.
 *
 * @param self_p Pointer to filter struct.
 * @param sample Sample to process.
 */
void Filter_Process(struct filter_t *self_p, int16_t sample);

/**
 * Get the filter output value.
 *
 * @param self_p Pointer to filter struct.
 *
 * @return The filter output value.
 */
int16_t Filter_Output(const struct filter_t *self_p);

/**
 * Check if the supplied filter is initialized.
 *
 * @param self_p Pointer to filter struct.
 *
 * @return True if initialized, otherwise false.
 */
bool Filter_IsInitialized(const struct filter_t *self_p);

#endif
