/**
 * @file   libInput.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2015-10-03 (Last edit)
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

#include <avr/interrupt.h> 
#include <util/atomic.h>
#include <stdio.h>

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

static libinput_callback_type left_event_callback;
static libinput_callback_type right_event_callback;
static libinput_callback_type push_event_callback;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

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
    static uint32_t debounce_timer = 0;
    static uint32_t push_timer = 0;
    static uint32_t disable_push_timer = 0;
    static uint8_t prev_a = 1;
    static uint8_t prev_b = 1;    
    uint8_t curr_a;
    uint8_t curr_b;
    
    curr_a = PINB & (1 << DDB0);
    curr_b = PINB & (1 << DDB1);
    
    //Check for a falling edge
    if (prev_a == 1 && curr_a == 0)
    {
        debounce_timer = libTimer_GetMilliseconds();
        disable_push_timer = debounce_timer;
    }
    
    if (debounce_timer != 0)
    {
        if (libTimer_TimeDifference(debounce_timer) > 2)
        {
            if (curr_a == 0)
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
                    if (left_event_callback != NULL)
                    {
                        left_event_callback();
                    }                    
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
    
    if (libTimer_TimeDifference(disable_push_timer) < 350)
    {
        push_timer = 0;
    }
    
    if (prev_b && curr_b == 0 && libTimer_TimeDifference(disable_push_timer) > 300)
    {
        push_timer = libTimer_GetMilliseconds();
    }
  
    if (push_timer != 0 && libTimer_TimeDifference(disable_push_timer) > 300)
    {
        if (libTimer_TimeDifference(push_timer) > 150)
        { 
            if (curr_b == 0)
            {
                push_event_callback();
                DEBUG("Push\r\n");
                push_timer = 0;
            } 
            else
            {
                push_timer = 0;
            }                 
        } 
             
    }    
    
    prev_b = curr_b;    
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
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////
