/**
 * @file   Event.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2016-10-23 (Last edit)
 * @brief  Header of Event
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

#ifndef EVENT_H_
#define EVENT_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include "Timer.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define Event_New(typeid) ((event_type){Timer_GetMilliseconds(), typeid})

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
} event_id_type;

typedef struct
{
    uint32_t timestamp;
    event_id_type id;
} event_type;

typedef void (*event_callback)(const event_type *event);

typedef struct
{
    event_id_type id;
    event_callback callback;
} event_listener_type;

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

void Event_AddListener(event_callback listener, event_id_type id);
void Event_Trigger(const event_type *event);

#endif /* EVENT_H_ */