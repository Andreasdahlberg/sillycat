/**
 * @file   LED.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2015-11-22 (Last edit)
 * @brief  Header of LED
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


#ifndef LED_H_
#define LED_H_

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

typedef enum
{
    LED_STATE_IDLE = 0,
    LED_STATE_SENDING,
    LED_STATE_CHARGING,
    LED_STATE_CHARGING_DONE,
    LED_STATE_SEARCHING
} led_state_type;

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

void LED_Init(void);
void LED_Update(void);
void LED_ChangeState(led_state_type state);

#endif /* LED_H_ */