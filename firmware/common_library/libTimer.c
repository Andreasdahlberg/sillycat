/**
 * @file   libTimer.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2015-11-05 (Last edit)
 * @brief  Implementation of low level timer functions
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

#include <avr/interrupt.h>
#include <util/atomic.h>

#include "libTimer.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

volatile uint32_t system_timer;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//INTERUPT SERVICE ROUTINES
//////////////////////////////////////////////////////////////////////////

ISR(TIMER1_COMPA_vect)
{
    ++system_timer;
}

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

///
/// @brief Init the timer hardware
///
/// @param  None
/// @return None
///
void libTimer_Init()
{
    system_timer = 0;

    TCCR1B |= (1 << WGM12);
    TIMSK1 |= (1 << OCIE1A);

    sei();

    OCR1A = 1000;
    TCCR1B |= (1 << CS11); // Fosc/8
}

///
/// @brief Resets the system timer
///
/// @param  None
/// @return None
///
void libTimer_Reset()
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        system_timer = 0;
    }
}

///
/// @brief Get the current time(ms)
///
/// @param  None
/// @return uint32_t The current system time in milliseconds
///
uint32_t libTimer_GetMilliseconds()
{
    uint32_t current_timer;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        current_timer = system_timer;
    }

    return current_timer;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////