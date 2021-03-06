/**
 * @file   driverPEC11.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2020-04-11 (Last edit)
 * @brief  Driver for Bourns PEC11 series rotary encoder with push button.
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

#include "common.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include "libDebug.h"
#include "Timer.h"
#include "Board.h"
#include "driverPEC11.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define PUSH_TIME_MS 1000

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

struct module_t
{
    volatile struct
    {
        uint8_t right;
        uint8_t left;
        struct
        {
            uint32_t timer;
            uint8_t brief;
        } push;
    } signals;
    volatile struct
    {
        bool latch;
        bool button;
    } states;
};

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static struct module_t module;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

static bool PopSignal(volatile uint8_t *signal_p);
static inline bool GetLatchState(void);
static inline bool GetDirectionState(void);
static inline bool GetButtonState(void);
static inline bool IsFallingEdge(bool state);
static inline bool IsRisingEdge(bool state);

//////////////////////////////////////////////////////////////////////////
//INTERUPT SERVICE ROUTINES
//////////////////////////////////////////////////////////////////////////

ISR(PCINT0_vect)
{
    bool latch_state;
    bool button_state;

    latch_state = GetLatchState();
    button_state = GetButtonState();

    if (module.states.latch != latch_state)
    {
        /**
         * The offset between the latch signal and the direction signal
         * determines the rotation direction.
         */
        if (latch_state != GetDirectionState())
        {
            module.signals.right = 1;
        }
        else
        {
            module.signals.left = 1;
        }
    }

    /**
     * Start timing a new push on the falling edge. On the rising edge the
     * total time is checked to determine if it was a brief or extended push.
     *
     * No action is taken on extended push since this is handled in
     * 'driverPEC11_PopExtendedPush'.
     */
    if (IsFallingEdge(button_state))
    {
        module.signals.push.timer = Timer_GetMilliseconds();
    }
    else if (IsRisingEdge(button_state))
    {
        /**
         * A timer value of zero means that an extended push already has been
         * taken by a call to 'driverPEC11_PopExtendedPush'.
         */
        if (module.signals.push.timer > 0 &&
                Timer_TimeDifference(module.signals.push.timer) < PUSH_TIME_MS)
        {
            module.signals.push.timer = 0;
            module.signals.push.brief = 1;
        }
    }

    /**
     * Store the current state so it's possible to figure out which pin
     * triggered the interrupt next time the ISR is called.
     */
    module.states.latch = latch_state;
    module.states.button = button_state;
}

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void driverPEC11_Init(void)
{
    module = (struct module_t)
    {
        .signals = {0},
        .states =
        {
            .latch = GetLatchState(),
            .button = GetButtonState()
        }
    };

    INFO("PEC11 driver initialized");
}

bool driverPEC11_PopLeftRotation(void)
{
    return PopSignal(&module.signals.left);
}

bool driverPEC11_PopRightRotation(void)
{
    return PopSignal(&module.signals.right);
}

bool driverPEC11_PopBriefPush(void)
{
    return PopSignal(&module.signals.push.brief);
}

bool driverPEC11_PopExtendedPush(void)
{
    if (module.signals.push.timer > 0 &&
            Timer_TimeDifference(module.signals.push.timer) >= PUSH_TIME_MS)
    {
        module.signals.push.timer = 0;
        return true;
    }
    else
    {
        return false;
    }
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static bool PopSignal(volatile uint8_t *signal_p)
{
    sc_assert(signal_p != NULL);

    bool status;

    if (*signal_p > 0)
    {
        *signal_p -= 1;
        status = true;
    }
    else
    {
        status = false;
    }

    return status;
}

static inline bool GetButtonState(void)
{
    return (PEC11_PINR & (1 << PEC11_BUTTON_PIN)) == 0;
}

static inline bool GetLatchState(void)
{
    return (PEC11_PINR & (1 << PEC11_LATCH_PIN)) != 0;
}

static inline bool GetDirectionState(void)
{
    return (PEC11_PINR & (1 << PEC11_DATA_PIN)) != 0;
}

static inline bool IsFallingEdge(bool state)
{
    return !module.states.button && state;
}

static inline bool IsRisingEdge(bool state)
{
    return module.states.button && !state;
}
