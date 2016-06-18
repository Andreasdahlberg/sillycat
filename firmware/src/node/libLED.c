/**
 * @file   libLED.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2016-01-31 (Last edit)
 * @brief  Implementation of libLED
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
#include "libDebug.h"
#include "libLED.h"
#include "Timer.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//TODO: Change to correct pin when using real hardware!
#define LED_PIN DDD5
#define NR_LEDS 1

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

///
/// @brief Init the LED library
///
/// @param  None
/// @return None
///
void libLED_Init(void)
{
    //Set led pin as output and pull it low
    DDRD |= (1 << LED_PIN);
    PORTD &= ~(1 << LED_PIN);

    INFO("Init done");
    return;
}

///
/// @brief Enable/disable a selected LED
///
/// @param  index LED to enable/disable, (is unused in this implementation)
/// @param  enable true to enable, false to disable
/// @return None
///
void libLED_Enable(uint8_t index __attribute__ ((unused)), bool enable)
{
    if (enable == true)
    {
        PORTD |= (1 << LED_PIN);
    }
    else
    {
        PORTD &= ~(1 << LED_PIN);
    }
    return;
}

///
/// @brief Toggle the state a selected LED
///
/// @param  index LED to toggle, (is unused in this implementation)
/// @return None
///
void libLED_Toggle(uint8_t index __attribute__ ((unused)))
{
    PORTD ^= (1 << LED_PIN);
    return;
}