/**
 * @file   libPower.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2017-07-28 (Last edit)
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
#include "ADC.h"
#include "Timer.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define VREF 3300 // mV
#define R1_RESISTANCE 5600.0
#define R2_RESISTANCE 100000.0

// Defines for approximation of maximum output current.
#define K_CONST ((uint32_t)91660)
#define M_CONST ((int32_t)-70810000)
#define VIN_MIN 1800
#define VIN_MAX 3500

#define NR_ADC_SAMPLES 16
#if NR_ADC_SAMPLES % 2 != 0
#error "The number of ADC samples must be a power of 2!"
#endif

#define CONNECTED_PIN PINC1
#define CHARGING_PIN PINC4

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

struct module_t
{
    struct adc_channel_t adc_battery_channel;
};

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static struct module_t module;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

static void EnableLowLevelInterupt(bool enable);

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

    PORTC  |= (1 << CONNECTED_PIN);
    PORTC  |= (1 << CHARGING_PIN);

    //Disable TWI(I2C) and Timer 2
    PRR = ((1 << PRTWI) | (1 << PRTIM2));

    //Turn off USART if debug is disabled
#ifndef DEBUG_ENABLE
    PRR |= (1 << PRUSART0);
#endif

    ADC_InitChannel(&module.adc_battery_channel, 7);
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
    if (IsGlobalInteruptEnabled() == false)
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

///
/// @brief Check if the charger is connected
///
/// @param  None
/// @return true if connected, otherwise false
///
bool libPower_IsChargerConnected(void)
{
    return ((PINC & (1 << CONNECTED_PIN)) == 0);
}

///
/// @brief Check if the battery is charging
///
/// @param  None
/// @return true if charging, otherwise false
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
    uint16_t samples[NR_ADC_SAMPLES];
    ADC_Convert(&module.adc_battery_channel, samples, ElementsIn(samples));

    uint32_t sample_sum = 0;
    for (size_t i = 0; i < ElementsIn(samples); ++i)
    {
        sample_sum += samples[i];
    }

    uint32_t voltage;
    voltage = (sample_sum * VREF + (NR_ADC_SAMPLES * 1024 / 2)) /
              (NR_ADC_SAMPLES * 1024);

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
    return (uint32_t)((float)voltage / (R2_RESISTANCE / (R1_RESISTANCE +
                                        R2_RESISTANCE)));
}

///
/// @brief Get an approximation of the maximum output power from
///        the buck-boost converter at the specified input voltage.
///
///        Note: The approximation works best in the normal operating
///              range(1.8V to 2.8V).
///
/// @param  vin Input voltage in mV
/// @return uint32_t Max output current in mA
///
uint32_t libPower_ApproximateMaxIout(uint16_t vin)
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
        iout_max = (K_CONST * vin + M_CONST) / 1000000;
    }

    return iout_max;
}

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

//TODO: Change to edge interrupt?
//NOTE: This function does not touch the global interrupt enable!
static void EnableLowLevelInterupt(bool enable)
{
    if (enable == true)
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
