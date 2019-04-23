/**
 * @file   driverInputCapture.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2019-04-23 (Last edit)
 * @brief  ATmega328 Input Capture driver.
 *
 * Driver for the ATmega328 timer input capture mode.
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
#include <avr/interrupt.h>
#include "driverInputCapture.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

struct module_t
{
    driverInputCapture_isr_t InterruptHandler;
};

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

static inline void SetClockSourceToCLK(void);
static inline void EnableInputCaptureInterrupts(void);
static inline void DisableInputCaptureInterrupts(void);
static inline void ClearInputCaptureInterruptFlag(void);
static inline void RemoveClockSource(void);
static inline void ResetTimer(void);
static inline void ResetControlRegister(void);

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static struct module_t module;

//////////////////////////////////////////////////////////////////////////
//INTERUPT SERVICE ROUTINES
//////////////////////////////////////////////////////////////////////////

ISR(TIMER1_CAPT_vect)
{
    if (module.InterruptHandler != NULL)
    {
        module.InterruptHandler(ICR1);
    }
}

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void driverInputCapture_Init(void)
{
    ResetControlRegister();
    DisableInputCaptureInterrupts();
    ClearInputCaptureInterruptFlag();
    ResetTimer();

    module = (__typeof__(module)) {0};
}

void driverInputCapture_RegisterInterruptHandler(driverInputCapture_isr_t interrupt_handler)
{
    module.InterruptHandler = interrupt_handler;
}

void driverInputCapture_Enable(void)
{
    SetClockSourceToCLK();
    ClearInputCaptureInterruptFlag();
    EnableInputCaptureInterrupts();
}

void driverInputCapture_Disable(void)
{
    DisableInputCaptureInterrupts();
    RemoveClockSource();
    ResetTimer();
}

void driverInputCapture_EnableNoiseCanceling(void)
{
    TCCR1B |= (1 << ICNC1);
}

void driverInputCapture_DisableNoiseCanceling(void)
{
    TCCR1B &= ~(1 << ICNC1);
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static inline void SetClockSourceToCLK(void)
{
    TCCR1B |= (1 << CS11);
}

static inline void EnableInputCaptureInterrupts(void)
{
    TIMSK1 |= (1 << ICIE1);
}

static inline void DisableInputCaptureInterrupts(void)
{
    TIMSK1 &= ~(1 << ICIE1);
}

static inline void ClearInputCaptureInterruptFlag(void)
{
    TIFR1 &= ~(1 << ICF1);
}

static inline void RemoveClockSource(void)
{
    TCCR1B &= ~(1 << CS12 | 1 << CS11 | 1 << CS10);
}

static inline void ResetTimer(void)
{
    TCNT1 = 0x00;
}

static inline void ResetControlRegister(void)
{
    TCCR1B = 0x00;
}
