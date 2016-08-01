/**
 * @file   libInput.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2016-08-01 (Last edit)
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
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <util/delay.h>
#include <stdio.h>

#include "libADC.h"
#include "libDebug.h"
#include "libInput.h"

#include "Timer.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define INPUT_DDR DDRB
#define INPUT_PINR PINB
#define LATCH_PIN DDB0
#define DATA_PIN DDB1

#define PUSH_ADC_CHANNEL 0x06

#define GetLatchSignal() ((INPUT_PINR & (1 << LATCH_PIN)) != 0)
#define GetDirectionSignal() ((INPUT_PINR & (1 << DATA_PIN)) != 0)

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

typedef struct
{
    uint8_t right;
    uint8_t left;
} encoder_rotations_type;

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static libinput_callback_type left_event_callback;
static libinput_callback_type right_event_callback;
static libinput_callback_type push_event_callback;
static encoder_rotations_type encoder_rotations;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

void PushCheckAndTrigger(void);

//////////////////////////////////////////////////////////////////////////
//INTERUPT SERVICE ROUTINES
//////////////////////////////////////////////////////////////////////////

ISR(PCINT0_vect)
{
    if (GetLatchSignal() != GetDirectionSignal())
    {

        encoder_rotations.right = 1;
    }
    else
    {
        encoder_rotations.left = 1;
    }
}

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
    INPUT_DDR &= ~(1 << LATCH_PIN | 1 << DATA_PIN);

    //NOTE: Using a ADC-channel for the push-button since no other pin is free.
    libADC_EnableInput(PUSH_ADC_CHANNEL, true);

    //Reset all callbacks
    right_event_callback = NULL;
    left_event_callback = NULL;
    push_event_callback = NULL;

    //Reset pending rotations
    encoder_rotations = (encoder_rotations_type) {0};

    //Enable pin change interupt on the latch pin
    PCMSK0 |= (1 << PCINT0);
    PCICR |= (1 << PCIE0);

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
    PushCheckAndTrigger();

    if (encoder_rotations.right > 0 && right_event_callback != NULL)
    {
        right_event_callback();
        --encoder_rotations.right;
    }
    else if (encoder_rotations.left > 0 && left_event_callback != NULL)
    {
        left_event_callback();
        --encoder_rotations.left;
    }

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

void PushCheckAndTrigger(void)
{
    static bool prev_push = false;
    uint16_t adc_sample;
    bool curr_push;

    if (libADC_GetSample(PUSH_ADC_CHANNEL, &adc_sample) == false)
    {
        return;
    }

    curr_push = adc_sample > 512;

    if (prev_push == false && curr_push == true)
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