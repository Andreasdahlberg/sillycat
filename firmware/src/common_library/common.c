/**
 * @file   common.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-02-11 (Last edit)
 * @brief  Implementation of common functions
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

#include <avr/wdt.h>
#include <avr/io.h>

#include "common.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define STACK_CANARY 0xc5

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

extern uint8_t _end;
extern uint8_t __stack;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

void StackPaint(void) __attribute__ ((naked)) __attribute__ ((
            section (".init1")));

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

float_parts_type FloatToParts(float f_value)
{
    float_parts_type parts;

    parts.integer = (int32_t)f_value;

    int8_t tmp;
    tmp = (int8_t)((f_value - parts.integer) * 10);
    parts.fractional = tmp < 0 ? -tmp : tmp;

    return parts;
}

///
/// @brief Computes the numerator/denominator and returns the quotient and remainder.
///
/// @param  numerator
/// @param  denominator
/// @return struct div_t Structure with quotient and remainder.
///
struct div_t Divide(int32_t numerator, int32_t denominator)
{
    struct div_t result;

    result.quotient = numerator / denominator;
    result.remainder = numerator % denominator;

    return result;
}

uint8_t BCDToDecimal(uint8_t bcd_data)
{
    uint8_t data;

    data = ((bcd_data & 0xF0) >> 4) * 10;
    data += (bcd_data & 0x0F);

    return data;
}

uint16_t DecimalToBCD(uint8_t decimal_data)
{
    uint16_t data = 0;
    uint8_t shift = 0;

    do
    {
        data |= (decimal_data % 10) << shift;
        shift += 4;
        decimal_data = decimal_data / 10;
    }
    while (decimal_data > 0);

    return data;
}

void SetBit(uint8_t bit_index, bool state, uint8_t *data)
{
    sc_assert(bit_index < 8);

    if (state == true)
    {
        *data |= (1 << bit_index);
    }
    else
    {
        *data &= ~(1 << bit_index);
    }
    return;
}

bool IsBitSet(uint8_t bit_index, const uint8_t *data)
{
    sc_assert(bit_index < 8);
    return (bool)(*data & (1 << bit_index));
}

void SoftReset(void)
{
    wdt_enable(WDTO_15MS);
    while (1)
    {
    }
}

bool IsGlobalInteruptEnabled(void)
{
    return ((SREG & (1 << 7)) > 0);
}

///
/// @brief Count unused stack space.
///
/// This examines the stack space, and determines how many bytes have never
/// been overwritten.
///
/// @author Michael C McTernan, Michael.McTernan.2001@cs.bris.ac.uk
///
/// @param  None
/// @return uint16_t Number of bytes likely to have never been used by the stack.
///
uint16_t StackCount(void)
{
    const uint8_t *p = &_end;
    uint16_t       c = 0;

    while(*p == STACK_CANARY && p <= &__stack)
    {
        p++;
        c++;
    }

    return c;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

///
/// @brief Fill the stack space with a known pattern.
///
/// Fill the stack space with a known pattern.
/// This fills all stack bytes with the 'canary' pattern to allow stack usage
/// to be later estimated.  This runs in the .init1 section, before normal
/// stack initialization and unfortunately before __zero_reg__ has been
/// setup.  The C code is therefore replaced with inline assembly to ensure
/// the zero reg is not used by compiled code.
///
/// @author Michael C McTernan, Michael.McTernan.2001@cs.bris.ac.uk
///
/// @param  None
/// @return None
///
void StackPaint(void)
{
#if 0
    uint8_t *p = &_end;

    while(p <= &__stack)
    {
        *p = STACK_CANARY;
        p++;
    }
#else
    __asm volatile ("    ldi r30,lo8(_end)\n"
                    "    ldi r31,hi8(_end)\n"
                    "    ldi r24,lo8(0xc5)\n" /* STACK_CANARY = 0xc5 */
                    "    ldi r25,hi8(__stack)\n"
                    "    rjmp .cmp\n"
                    ".loop:\n"
                    "    st Z+,r24\n"
                    ".cmp:\n"
                    "    cpi r30,lo8(__stack)\n"
                    "    cpc r31,r25\n"
                    "    brlo .loop\n"
                    "    breq .loop"::);
#endif
}
