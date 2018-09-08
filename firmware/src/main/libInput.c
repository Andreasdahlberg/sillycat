/**
 * @file   libInput.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-09-08 (Last edit)
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
#include "Timer.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define INPUT_DDR   DDRB
#define INPUT_PINR  PINB
#define LATCH_PIN   DDB0
#define DATA_PIN    DDB1
#define BUTTON_PIN  DDB6

#define PUSH_TIME_MS 1000

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

struct module_t
{
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

static void InitializePins(void);
static void PushCheckAndTrigger(void);
static inline bool GetLatchSignal(void);
static inline bool GetDirectionSignal(void);
static inline bool GetButtonSignal(void);

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

void libInput_Init(void)
{
    module = (struct module_t) { {0}, {0}};

    InitializePins();

    INFO("Input module initialized");
}

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

static void InitializePins(void)
{
    /**
     * Set latch, data and button pins as inputs and enable internal
     * pull-up for the button pin.
     */
    INPUT_DDR &= ~(1 << LATCH_PIN | 1 << DATA_PIN | 1 << BUTTON_PIN);
    PORTB |= (1 << BUTTON_PIN);

    /**
     * Enable pin change interrupts on the latch pin so that the
     * data pin can be decoded when the latch pin changes.
     */
    PCMSK0 |= (1 << PCINT0);
    PCICR |= (1 << PCIE0);
}

static void PushCheckAndTrigger(void)
{
    static bool prev_push = false;

    bool curr_push = GetButtonSignal();

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

static inline bool GetButtonSignal(void)
{
    return (INPUT_PINR & (1 << BUTTON_PIN)) == 0;
}

static inline bool GetLatchSignal(void)
{
    return (INPUT_PINR & (1 << LATCH_PIN)) != 0;
}

static inline bool GetDirectionSignal(void)
{
    return (INPUT_PINR & (1 << DATA_PIN)) != 0;
}
