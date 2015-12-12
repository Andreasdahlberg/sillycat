/**
 * @file   libPower.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2015-12-12 (Last edit)
 * @brief  Implementation of libPower
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

//NOTE: Include before all other headers
#include "common.h"

#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

#include "libPower.h"
#include "libDebug.h"
#include "libADC.h"
#include "Timer.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define VREF 3300 // mV
#define R1_RESISTANCE 5600.0
#define R2_RESISTANCE 100000.0

#define CONNECTED_PIN PINC1
#define CHARGING_PIN PINC4

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

void EnableLowLevelInterupt(bool enable);

//////////////////////////////////////////////////////////////////////////
//INTERUPT SERVICE ROUTINES
//////////////////////////////////////////////////////////////////////////

ISR(INT1_vect)
{
    //Do nothing here
}

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

///
/// @brief Init power module
///
/// @param  None
/// @return None
///
void libPower_Init(void)
{
    DDRC &= ~(1 << CONNECTED_PIN | 1 << CHARGING_PIN);

    DDRD &= ~(1 << DDD3);
    PORTD |= (1 << DDD3);

    //Disable TWI(I2C) and Timer 2
    PRR = ((1 << PRTWI) | (1 << PRTIM2));

    //Turn off USART if debug is disabled
#ifndef DEBUG_ENABLE
    PRR |= (1 << PRUSART0);
#endif
    return;
}

///
/// @brief Enter sleep mode.
///
/// @param  None
/// @return None
///
void libPower_Sleep(void)
{
    //Do not enter sleep if global interrupts are disabled.
    //Without this the device can not wake up again.
    //IMPORTANT: This function does not catch cases where
    //           interrupts is disabled inside an interrupt vector.
    if (IsGlobalInteruptEnabled() == FALSE)
    {
        sc_assert_fail();
        return;
    }

    EnableLowLevelInterupt(TRUE);

    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    sleep_cpu();
    sleep_disable();

    EnableLowLevelInterupt(FALSE);

#ifdef DEBUG_ENABLE
    DEBUG("\r\n");
#endif
    return;
}

///
/// @brief Check if the charger is connected
///
/// @param  None
/// @return TRUE if connected, otherwise FALSE
///
bool libPower_IsChargerConnected(void)
{
    return ((PINC & (1 << CONNECTED_PIN)) == 0);
}

///
/// @brief Check if the battery is charging
///
/// @param  None
/// @return TRUE if charging, otherwise FALSE
///
bool libPower_IsCharging(void)
{
    return ((PINC & (1 << CHARGING_PIN)) == 0);
}

///
/// @brief Get the current battery voltage
///
/// @param  None
/// @return Battery voltage in mV
///
uint32_t libPower_GetBatteryVoltage(void)
{
    uint32_t sample = 0;//libADC_GetSample();
    uint32_t voltage = (sample * VREF) / 1024;

    /*
     | VBAT
     |
     -
    | | R1 = 5.6 KOhm
    | |
     -
     |
     ------- VSENSE
     |
     -
    | | R2 = 100 KOhm
    | |
     -
     |
     | GND
    */

    //Adjust voltage from voltage divider
    return (uint32_t)((float)voltage / (R2_RESISTANCE / (R1_RESISTANCE + R2_RESISTANCE)));
}

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

//NOTE: This function does not touch the global interrupt enable!
void EnableLowLevelInterupt(bool enable)
{
    if (enable == TRUE)
    {
        //Enable low level sense
        EICRA &= ~(1 << ISC11 | 1 << ISC10);

        //Enable external interrupt 1
        EIMSK |= (1 << INT1);
    }
    else
    {
        //Disable external interrupt 1
        EIMSK &= ~(1 << INT1);
    }
    return;
}