/**
 * @file   driverTimer.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2019-06-27 (Last edit)
 * @brief  Implementation of ATmega328 timer driver.
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

#include "common.h"
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "driverTimer.h"

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

ISR(TIMER0_COMPA_vect)
{
    ++system_timer;
}

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void driverTimer_Init()
{
    system_timer = 0;

    //Set CTC-mode
    TCCR0A |= (1 << WGM01);

#if F_CPU == 8000000
    //Set prescaler to 64
    TCCR0B |= (1 << CS00 | 1 << CS01);

    //Set compare value
    OCR0A = 125;
#else
#error "Unsupported frequency"
#endif

    driverTimer_Start();

    sei();
}

void driverTimer_Start()
{
    /* Enabled interrupt on compare match A. */
    TIMSK0 |= (1 << OCIE0A);
}

void driverTimer_Stop()
{
    /* Disabled interrupt on compare match A. */
    TIMSK0 &= ~(1 << OCIE0A);
}

void driverTimer_Reset()
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        system_timer = 0;
    }
}

uint32_t driverTimer_GetMilliseconds()
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
