/**
 * @file   Event.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-10-20 (Last edit)
 * @brief  Implementation of Event module
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

#ifndef EVENT_H_
#define EVENT_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include "Timer.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define Event_New(typeid) ((event_t){Timer_GetMilliseconds(), typeid})

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

typedef enum
{
    EVENT_ALL = 0,
    EVENT_SLEEP,
    EVENT_WAKEUP,
    EVENT_BATTERY_CHARGING_STARTED,
    EVENT_BATTERY_CHARGING_STOPPED,
    EVENT_BATTERY_CHARGER_DISCONNECTED,
    EVENT_BATTERY_CRITICAL,
    EVENT_BATTERY_LOW,
    EVENT_RHT_AVAILABLE
} event_id_t;

typedef struct
{
    uint32_t timestamp;
    event_id_t id;
} event_t;

typedef void (*event_callback_t)(const event_t *event_p);

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

/**
 * Initialize the Event module.
 */
void Event_Init(void);

/**
 * Connect a listener to an event type.
 *
 * @param listener_p Function pointer to a listener function.
 * @param id         Event id to register to.
 */
void Event_AddListener(event_callback_t listener_p, event_id_t id);

/**
 * Trigger an event.
 *
 * This will notify all listeners connected to the triggered event.
 *
 * @param event_p Pointer to event to trigger.
 */
void Event_Trigger(const event_t *event_p);

#endif
