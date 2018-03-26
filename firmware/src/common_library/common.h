/**
 * @file   common.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-03-26 (Last edit)
 * @brief  Header of common functions
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

#ifndef COMMON_H_
#define COMMON_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "sc_assert.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define SUPPLY_VOLTAGE_mV 3300

//Valid if sample_window >> sample_freq
#define CALCULATE_ALPHA(sample_freq, sample_window) ((uint32_t)131070 / ((uint32_t)sample_window / (uint32_t)sample_freq + 1))

#define SetBitD(data, bit) data |= (1 << bit)
#define ClearBit(data, bit) data &= ~(1 << bit)

#define UNUSED(var) (void)var

#define ElementsIn(array) \
    ({ \
        _Static_assert \
        ( \
            ! __builtin_types_compatible_p(__typeof__(array), __typeof__(&array[0])), \
            "ElementsIn: "  # array " is not an array" \
        ); \
        sizeof(array) / sizeof((array)[0]); \
    })

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

typedef enum function_status
{
    ERROR = 0,
    SUCCESS,
    BUSY,
    TIMEOUT
} function_status;

struct div_t
{
    int32_t quotient;
    int32_t remainder;
};

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

struct div_t Divide(int32_t numerator, int32_t denominator);
void SetBit(uint8_t bit_index, bool state, uint8_t *data);
bool IsBitSet(uint8_t bit_index, const uint8_t *data);
uint8_t BCDToDecimal(uint8_t bcd_data);
uint16_t DecimalToBCD(uint8_t decimal_data);
void SoftReset(void) __attribute__((noreturn));
bool IsGlobalInteruptEnabled(void);
uint16_t StackCount(void);

#endif
