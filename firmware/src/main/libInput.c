/**
 * @file   libInput.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-09-02 (Last edit)
 * @brief  Module responsible for handling user input.
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

struct module_t
{
    struct adc_channel_t push_channel;
    struct
    {
        libinput_callback_t left;
        libinput_callback_t right;
        libinput_callback_t push;
        libinput_callback_t press;
    } callbacks;
    struct
    {
        uint32_t down_timer;
        bool pressed;
        struct
        {
            uint8_t right;
            uint8_t left;
        } rotations;
    } encoder;
};

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static struct module_t module;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

void InitializePins(void);
void PushCheckAndTrigger(void);
bool IsPushed(void);

//////////////////////////////////////////////////////////////////////////
//INTERUPT SERVICE ROUTINES
//////////////////////////////////////////////////////////////////////////

ISR(PCINT0_vect)
{
    if (GetLatchSignal() != GetDirectionSignal())
    {
        module.encoder.rotations.right = 1;
    }
    else
    {
        module.encoder.rotations.left = 1;
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
    module = (struct module_t) { {0}, {0}, {0}};

    InitializePins();

    //NOTE: Using a ADC-channel for the push-button since no other pin is free.
    ADC_InitChannel(&module.push_channel, PUSH_ADC_CHANNEL);

    INFO("Input module initialized");
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

    if (module.encoder.rotations.right > 0 && module.callbacks.right != NULL)
    {
        module.callbacks.right();
        --module.encoder.rotations.right;
    }
    else if (module.encoder.rotations.left > 0 && module.callbacks.left != NULL)
    {
        module.callbacks.left();
        --module.encoder.rotations.left;
    }
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
void libInput_SetCallbacks(libinput_callback_t right_event,
                           libinput_callback_t left_event,
                           libinput_callback_t push_event,
                           libinput_callback_t press_event)
{
    module.callbacks.right = right_event;
    module.callbacks.left = left_event;
    module.callbacks.push = push_event;
    module.callbacks.press = press_event;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void InitializePins(void)
{
    /**
     * Set latch and data pins as inputs.
     */
    INPUT_DDR &= ~(1 << LATCH_PIN | 1 << DATA_PIN);

    /**
     * Enable pin change interrupts on the latch pin so that the
     * data pin can be decoded when the latch pin changes.
     */
    PCMSK0 |= (1 << PCINT0);
    PCICR |= (1 << PCIE0);
}

void PushCheckAndTrigger(void)
{
    static bool prev_push = false;

    bool curr_push = IsPushed();

    if (!prev_push && curr_push)
    {
        module.encoder.down_timer = Timer_GetMilliseconds();
    }
    else if (prev_push  && curr_push)
    {
        if (!module.encoder.pressed &&
                Timer_TimeDifference(module.encoder.down_timer) > PUSH_TIME_MS)
        {
            if (module.callbacks.press != NULL)
            {
                module.callbacks.press();
            }
            module.encoder.pressed = true;
        }
    }
    else if (prev_push && !curr_push)
    {
        if (!module.encoder.pressed && module.callbacks.push != NULL)
        {
            module.callbacks.push();
        }
        module.encoder.pressed = false;
    }

    prev_push = curr_push;
}

bool IsPushed(void)
{
    uint16_t adc_sample;
    ADC_Convert(&module.push_channel, &adc_sample, 1);

    return (bool)(adc_sample > 512);
}
