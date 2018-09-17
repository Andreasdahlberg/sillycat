/**
 * @file   Encoder.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-09-17 (Last edit)
 * @brief  Module for handling rotary encoders.
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

#ifndef ENCODER_H_
#define ENCODER_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

typedef void (*encoder_callback_t)(void);
struct encoder_callbacks_t
{
    encoder_callback_t right;
    encoder_callback_t left;
    encoder_callback_t brief_push;
    encoder_callback_t extended_push;
};

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

/**
 * Initialize the encoder module.
 */
void Encoder_Init(void);

/**
 * Update the internal state of the encoder module.
 *
 * Calls any set callbacks if the rotary encoder has been active.
 */
void Encoder_Update(void);

/**
 * Set callbacks for the encoder signals.
 *
 * @param callbacks_p Pointer to struct with callbacks for all signals.
 */
void Encoder_SetCallbacks(const struct encoder_callbacks_t *callbacks_p);

/**
 * Get callbacks for the encoder signals.
 *
 * @return Struct with callbacks for all signals.
 */
struct encoder_callbacks_t Encoder_GetCallbacks(void);

#endif
