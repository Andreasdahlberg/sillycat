/**
 * @file   driverCharger.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-04-20 (Last edit)
 * @brief  LTC4060 charger driver
 *
 * Driver for the LTC4060 NiMH/NICd fast battery charger.
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
#include <avr/interrupt.h>

#include "driverCharger.h"
#include "libDebug.h"
#include "ADC.h"
#include "Filter.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define VREF            3300    // mV
#define R1_RESISTANCE   5600    // Ohm
#define R2_RESISTANCE   100000  // Ohm
#define SAMPLE_PERIOD   20      // ms

#define CONNECTED_PIN       PINC1
#define CONNECTED_PIN_INT   PCINT9
#define CHARGING_PIN        PINC4
#define CHARGING_PIN_INT    PCINT12

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

struct module_t
{
    struct
    {
        struct adc_channel_t voltage;
        struct adc_channel_t temperature;
    } adc;
    struct
    {
        struct filter_t voltage;
    } filter;
    uint32_t sample_timer;
};

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static struct module_t module;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

void InitChargerPins(void);
uint16_t GetBatteryVoltage(void);

//////////////////////////////////////////////////////////////////////////
//INTERUPT SERVICE ROUTINES
//////////////////////////////////////////////////////////////////////////

ISR(PCINT1_vect)
{
    /**
     * Nothing is done in the ISR for the moment. The interrupt is only used
     * to wake the device from sleep.
     */
}

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void driverCharger_Init(void)
{
    ADC_InitChannel(&module.adc.voltage, 7);
    ADC_InitChannel(&module.adc.temperature, 6);

    InitChargerPins();

    Filter_Init(&module.filter.voltage, GetBatteryVoltage(), FILTER_ALPHA(0.1));
    module.sample_timer = 0;

    INFO("LTC4060 driver initialized");
}

void driverCharger_Update(void)
{
    if (Timer_TimeDifference(module.sample_timer) > SAMPLE_PERIOD)
    {
        uint16_t voltage = GetBatteryVoltage();

        Filter_Process(&module.filter.voltage, (int16_t)voltage);

        module.sample_timer = Timer_GetMilliseconds();
    }
}

bool driverCharger_IsConnected(void)
{
    return ((PINC & (1 << CONNECTED_PIN)) == 0);
}

bool driverCharger_IsCharging(void)
{
    return ((PINC & (1 << CHARGING_PIN)) == 0);
}

uint16_t driverCharger_GetBatteryVoltage(void)
{
    return (uint16_t)Filter_Output(&module.filter.voltage);
}

int16_t driverCharger_GetBatteryTemperature(void)
{
    return 0;
}

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void InitChargerPins(void)
{
    /* Set pins as inputs. */
    DDRC &= ~(1 << CONNECTED_PIN | 1 << CHARGING_PIN);

    /* Enable pull-ups. */
    PORTC |= 1 << CONNECTED_PIN | 1 << CHARGING_PIN;

    /* Enable pin change interrupts. */
    PCMSK1 |= 1 << CONNECTED_PIN_INT | 1 << CHARGING_PIN_INT;
    PCICR |= (1 << PCIE1);
}

uint16_t GetBatteryVoltage(void)
{
    uint16_t adc_value;
    ADC_Convert(&module.adc.voltage, &adc_value, 1);

    /**
     *
     *  | VBAT
     *  |
     *  -
     * | | R1 = 5.6 KOhm
     * | |
     *  -
     *  |
     *  ------- VSENSE
     *  |
     *  -
     * | | R2 = 100 KOhm
     * | |
     *  -
     *  |
     *  | GND
     */

    return ((uint32_t)adc_value * VREF) /
           ((R2_RESISTANCE * 1024) / (R1_RESISTANCE + R2_RESISTANCE));
}
