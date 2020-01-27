/**
 * @file   libPower.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2020-01-27 (Last edit)
 * @brief  Implementation of low level power functions.
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

#include "common.h"
#include "Board.h"

#include <avr/sleep.h>
#include <avr/interrupt.h>

#include "libPower.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

/* Defines for approximation of maximum output current. */
#define K_CONST ((uint32_t)91660)
#define M_CONST ((int32_t)-70810000)
#define VIN_MIN 1800
#define VIN_MAX 3500

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

static void EnableLowLevelInterupt(bool enable);

//////////////////////////////////////////////////////////////////////////
//INTERUPT SERVICE ROUTINES
//////////////////////////////////////////////////////////////////////////

ISR(INT1_vect)
{
    /* Do nothing here. */
}

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void libPower_Init(void)
{
    /* Disable TWI(I2C) and Timer 2 since they are not used. */
    PRR = ((1 << PRTWI) | (1 << PRTIM2));

    /* Turn off USART if debug is disabled. */
#ifndef DEBUG_ENABLE
    PRR |= (1 << PRUSART0);
#endif

    return;
}

void libPower_Sleep(void)
{
    /**
     * Do not enter sleep if global interrupts are disabled. Without this the
     * device can not wake up again.
     *
     * IMPORTANT: This function does not catch cases where interrupts are
     * disabled inside an interrupt vector.
     */
    if (Board_IsGlobalInterruptEnabled() == false)
    {
        sc_assert_fail();
        return;
    }

    EnableLowLevelInterupt(true);

    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    sleep_cpu();
    sleep_disable();

    EnableLowLevelInterupt(false);
    return;
}

uint32_t libPower_GetMaxOutputCurrent(uint16_t vin)
{
    uint32_t iout_max;

    if (vin < VIN_MIN)
    {
        iout_max = 0;
    }
    else if (vin > VIN_MAX)
    {
        iout_max = 240;
    }
    else
    {
        iout_max = ((K_CONST * (uint32_t)vin) + M_CONST) / 1000000;
    }

    return iout_max;
}

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

/* This function does not touch the global interrupt enable! */
static void EnableLowLevelInterupt(bool enable)
{
    if (enable == true)
    {
        /* Enable low level sense. */
        EICRA &= ~(1 << ISC11 | 1 << ISC10);

        /* Enable external interrupt 1. */
        EIMSK |= (1 << INT1);
    }
    else
    {
        /* Disable external interrupt 1. */
        EIMSK &= ~(1 << INT1);
    }
    return;
}
