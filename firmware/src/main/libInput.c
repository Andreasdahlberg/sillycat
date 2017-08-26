/**
 * @file   libInput.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2017-08-26 (Last edit)
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

#include "libDebug.h"
#include "libInput.h"

#include "ADC.h"
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

#define PUSH_TIME_MS 1000

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

typedef struct
{
    uint8_t right;
    uint8_t left;
} encoder_rotations_type;

struct encoder_push_t
{
    uint32_t down;
    bool pressed;
};

struct module_t
{
    struct adc_channel_t push_channel;
};

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static libinput_callback_type left_event_callback;
static libinput_callback_type right_event_callback;
static libinput_callback_type push_event_callback;
static libinput_callback_type press_event_callback;
static encoder_rotations_type encoder_rotations;
static struct module_t module;

static struct encoder_push_t push;

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
    ADC_InitChannel(&module.push_channel, PUSH_ADC_CHANNEL);

    //Reset all callbacks
    right_event_callback = NULL;
    left_event_callback = NULL;
    push_event_callback = NULL;
    press_event_callback = NULL;

    //Reset pending rotations
    encoder_rotations = (encoder_rotations_type) {0};

    push = (struct encoder_push_t) {0};

    //Enable pin change interrupt on the latch pin
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
/// @param  press_event Function to call when pressing is detected, NULL if no
///                     action is wanted.
/// @return None
///
void libInput_SetCallbacks(libinput_callback_type right_event,
                           libinput_callback_type left_event,
                           libinput_callback_type push_event,
                           libinput_callback_type press_event)
{
    right_event_callback = right_event;
    left_event_callback = left_event;
    push_event_callback = push_event;
    press_event_callback = press_event;

    return;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void PushCheckAndTrigger(void)
{
    static bool prev_push = false;

    uint16_t adc_sample;
    ADC_Convert(&module.push_channel, &adc_sample, 1);

    bool curr_push;
    curr_push = (bool)(adc_sample > 512);

    if (!prev_push && curr_push)
    {
        push.down = Timer_GetMilliseconds();
    }
    else if (prev_push  && curr_push)
    {
        if (!push.pressed && Timer_TimeDifference(push.down) > PUSH_TIME_MS)
        {
            if (press_event_callback != NULL)
            {
                press_event_callback();
            }
            push.pressed = true;
        }
    }
    else if (prev_push && !curr_push)
    {
        if (!push.pressed && push_event_callback != NULL)
        {
            push_event_callback();
        }
        push.pressed = false;
    }

    prev_push = curr_push;
    return;
}
