/**
 * @file   libInput.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2015-11-11 (Last edit)
 * @brief  Implementation of input module.
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
#include <util/delay.h>
#include <stdio.h>

#include "libADC.h"
#include "libDebug.h"
#include "libInput.h"

#include "Timer.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define PUSH_ADC_CHANNEL 0x06

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static libinput_callback_type left_event_callback;
static libinput_callback_type right_event_callback;
static libinput_callback_type push_event_callback;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

void DirectionCheckAndTrigger(void);
void PushCheckAndTrigger(void);

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

///
/// @brief Init pins and reset all event callbacks.
///
/// @param  None
/// @return None
///
void libInput_Init(void)
{
    //Set pins as inputs
    DDRB &= ~(1 << DDB0 | 1 << DDB1);

    //NOTE: Using a ADC-channel for the push-button since no other pin is free.
    libADC_EnableInput(PUSH_ADC_CHANNEL, TRUE);

    //Reset all callbacks
    right_event_callback = NULL;
    left_event_callback = NULL;
    push_event_callback = NULL;

    INFO("Init done");
    return;
}

///
/// @brief Check for new inputs and trigger events.
///
/// @param  None
/// @return None
///
void libInput_Update(void)
{
    static uint8_t prev_a = 1;
    uint8_t curr_a;

    PushCheckAndTrigger();

    curr_a = PINB & (1 << DDB0);

    //Check for a falling edge on channel A
    if (prev_a == 1 && curr_a == 0)
    {
        //debounce_timer
        _delay_us(20);
        if (!(PINB & (1 << DDB0)))
        {
            DirectionCheckAndTrigger();
        }
    }

    prev_a = curr_a;
    return;
}

///
/// @brief Set callbacks for input events.
///
/// @param  right_event Function to call when rotation to the right is detected,
///                     NULL if no action is wanted.
/// @param  left_event Function to call when rotation to the left is detected,
///                    NULL if no action is wanted.
/// @param  push_event Function to call when push is detected, NULL if no action
///                    is wanted.
/// @return None
///
void libInput_SetCallbacks(libinput_callback_type right_event,
                           libinput_callback_type left_event,
                           libinput_callback_type push_event)
{
    right_event_callback = right_event;
    left_event_callback = left_event;
    push_event_callback = push_event;
    return;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void DirectionCheckAndTrigger(void)
{
    if (PINB & (1 << DDB1))
    {
        DEBUG("Right\r\n");
        if (right_event_callback != NULL)
        {
            right_event_callback();
        }
    }
    else
    {
        DEBUG("Left\r\n");
        if (left_event_callback != NULL)
        {
            left_event_callback();
        }
    }
    return;
}

void PushCheckAndTrigger(void)
{
    static bool prev_push = FALSE;
    uint16_t adc_sample;
    bool curr_push;

    libADC_GetSample(PUSH_ADC_CHANNEL, &adc_sample);
    curr_push = adc_sample > 512;

    if (prev_push == FALSE && curr_push == TRUE)
    {
        DEBUG("Push\r\n");
        if (push_event_callback != NULL)
        {
            push_event_callback();
        }
    }

    prev_push = curr_push;
    return;
}