/**
 * @file   Board.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2020-04-11 (Last edit)
 * @brief  Board support package for the main unit.
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

#include <avr/io.h>
#include <util/delay.h>
#include "Board.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

#define FILTER_CAP_TC 2

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

static inline void SetPEC11PinsAsInputs(void);
static inline void EnablePinChangeInterrupts(void);
static inline void WaitForFilteringCaps(void);

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//INTERUPT SERVICE ROUTINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void Board_Init(void)
{
    Board_RTC_Init();
    Board_RFM69_Init();
    Board_PEC11_Init();
}

void Board_PEC11_Init(void)
{
    SetPEC11PinsAsInputs();
    WaitForFilteringCaps();
    EnablePinChangeInterrupts();
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static inline void SetPEC11PinsAsInputs(void)
{
    PEC11_DDR &= ~(1 << PEC11_LATCH_PIN | 1 << PEC11_DATA_PIN | 1 << PEC11_BUTTON_PIN);
}

static inline void EnablePinChangeInterrupts(void)
{
    /**
     * Enable pin change interrupts on the latch pin so that the
     * data pin can be decoded when the latch pin changes.
     */
    PCMSK0 |= (1 << PCINT0) | (1 << PCINT6);
    PCICR |= (1 << PCIE0);
}

static inline void WaitForFilteringCaps(void)
{
    _delay_ms(FILTER_CAP_TC);
}
