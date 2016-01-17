/**
 * @file   LED.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2016-01-17 (Last edit)
 * @brief  Implementation of LED
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

#include "libDebug.h"
#include "libLED.h"
#include "LED.h"
#include "Timer.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//TODO: Increase this, short interval during testing!
#define IDLE_FLASH_INTERVAL_MS 30000
#define FLASH_DURATION_MS 100
#define BLINK_DURATION_MS 300

#define EnabledBlueLED(enabled) libLED_Enable(0, enabled);

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

typedef enum
{
    FLASH_LED_OFF = 0,
    FLASH_LED_ON
} flash_led_state_type;

typedef enum
{
    IDLE_LED_WAIT = 0,
    IDLE_LED_FLASH
} idle_led_state_type;

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static led_state_type led_state;
static uint32_t led_timer;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

void IdleLED(void);
bool FlashLED(uint8_t flashes, uint16_t duration);

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

///
/// @brief Init ...
///
/// @param  None
/// @return None
///
void LED_Init(void)
{
    libLED_Init();
    led_timer = Timer_GetMilliseconds();
    led_state = LED_STATE_IDLE;
    EnabledBlueLED(FALSE);

    INFO("Init done");
    return;
}

///
/// @brief Update ...
///
/// @param  None
/// @return None
///
void LED_Update(void)
{
    sc_assert(led_state <= LED_STATE_CHARGING);

    switch (led_state)
    {
        case LED_STATE_IDLE:
            IdleLED();
            break;

        case LED_STATE_SENDING:
            if (FlashLED(2, FLASH_DURATION_MS) == TRUE)
            {
                LED_ChangeState(LED_STATE_IDLE);
            }
            break;

        case LED_STATE_CHARGING:
            break;

        case LED_STATE_CHARGING_DONE:
            break;

        case LED_STATE_SEARCHING:
            break;

        case LED_STATE_SLEEPING:
            break;

        default:
            break;
    }

    return;
}

///
/// @brief Change the current LED-state
///
/// @param  state The new state
/// @return None
///
void LED_ChangeState(led_state_type state)
{
    sc_assert(state <= LED_STATE_SLEEPING);

    switch (state)
    {
        case LED_STATE_IDLE:
        case LED_STATE_SENDING:
        case LED_STATE_CHARGING_DONE:
        case LED_STATE_SEARCHING:
            EnabledBlueLED(FALSE);
            led_state = state;
            break;

        case LED_STATE_CHARGING:
            EnabledBlueLED(TRUE);
            led_state = LED_STATE_CHARGING;
            break;

        case LED_STATE_SLEEPING:
            EnabledBlueLED(FALSE);
            led_state = LED_STATE_SLEEPING;
            break;

        default:
            break;
    }
    return;
}

///
/// @brief Handle events
///
/// @param  *event Pointer to triggered event
/// @return None
///
void LED_EventHandler(const event_type *event)
{
    switch (event->id)
    {
        case EVENT_SLEEP:
            LED_ChangeState(LED_STATE_SLEEPING);
            INFO("Entering sleep");
            break;

        case EVENT_WAKEUP:
            LED_ChangeState(LED_STATE_IDLE);
            INFO("Exiting sleep");
            break;

        default:
            //Do nothing if an unknown event is received
            break;
    }
    return;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void IdleLED(void)
{
    static idle_led_state_type idle_led_state = IDLE_LED_WAIT;

    switch (idle_led_state)
    {
        case IDLE_LED_WAIT:
            if (Timer_TimeDifference(led_timer) > IDLE_FLASH_INTERVAL_MS)
            {
                idle_led_state = IDLE_LED_FLASH;
                led_timer = Timer_GetMilliseconds();
            }
            break;

        case IDLE_LED_FLASH:
            if (FlashLED(1, BLINK_DURATION_MS) == TRUE)
            {
                idle_led_state = IDLE_LED_WAIT;
                led_timer = Timer_GetMilliseconds();
            }
            break;

        default:
            break;
    }

    return;
}

bool FlashLED(uint8_t flashes, uint16_t duration)
{
    static uint8_t flash_counter;
    static flash_led_state_type flash_state = FLASH_LED_OFF;

    switch (flash_state)
    {
        case FLASH_LED_OFF:
            if (Timer_TimeDifference(led_timer) > duration)
            {
                EnabledBlueLED(TRUE);
                led_timer = Timer_GetMilliseconds();
                flash_state = FLASH_LED_ON;
            }
            break;

        case FLASH_LED_ON:
            if (Timer_TimeDifference(led_timer) > duration)
            {
                EnabledBlueLED(FALSE);
                led_timer = Timer_GetMilliseconds();
                flash_state = FLASH_LED_OFF;


                ++flash_counter;
                if (flash_counter == flashes)
                {
                    flash_counter = 0;
                    return TRUE;
                }
            }
            break;

        default:
            break;
    }
    return FALSE;
}