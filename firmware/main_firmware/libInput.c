/**
 * @file   <filename>
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2015-09-10 (Last edit)
 * @brief  Implementation of <name>
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

#include <avr/interrupt.h> 
#include <util/atomic.h>

#include "common.h"
#include "libDebug.h"
#include "libTimer.h"
#include "libInput.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static libinput_button_event_type button_events;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void libInput_Init(void)
{
	//Set pins as inputs
	DDRB &= ~(1 << DDB0 | 1 << DDB1);  
    
    button_events.right = 0;
    button_events.left = 0;
    button_events.press = 0;
    
    INFO("Init done");
    return;
}


void libInput_Update(void)
{
    static uint32_t debounce_timer = 0;
    static uint8_t prev = 1;
    uint8_t curr;
    
    curr = PINB & (1 << DDB0);
    
    //Check for a falling edge
    if (prev == 1 && curr == 0)
    {
        debounce_timer = libTimer_GetMilliseconds();
    }
    
    if (debounce_timer != 0)
    {
        if (libTimer_TimeDifference(debounce_timer) > 2)
        {
            if (curr == 0)
            {
                if (PINB & (1 << DDB1))
                {   
                    DEBUG("Right\r\n");
                    button_events.right += 1;
                }
                else
                {
                    button_events.left += 1;
                    DEBUG("Left\r\n");
                }                
                
                debounce_timer = 0;
            }
            else
            {
                debounce_timer = 0;                
            }                
        }
    }    
    
    prev = curr;
    return;
}    


//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////
