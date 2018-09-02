/**
 * @file   libInput.h
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

#ifndef LIBINPUT_H_
#define LIBINPUT_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include <stdbool.h>

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

typedef void (*libinput_callback_t)(void);

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

/**
 * Initialize the input module.
 *
 * Initialize the hardware pins used for user input and reset the internal
 * module state.
 */
void libInput_Init(void);

/**
 * Check for new inputs and trigger events.
 */
void libInput_Update(void);

/**
 * Set callbacks for input events.
 *
 * NULL can be used if no action is needed.
 *
 * @param right_event Function to call when right rotation is detected.
 * @param left_event Function to call when left rotation is detected.
 * @param push_event Function to call when the button is pushed briefly.
 * @param press_event Function to call when the button is pressed.
 */
void libInput_SetCallbacks(libinput_callback_t right_event,
                           libinput_callback_t left_event,
                           libinput_callback_t push_event,
                           libinput_callback_t press_event);
#endif
