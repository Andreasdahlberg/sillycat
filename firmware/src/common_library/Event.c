/**
 * @file   Event.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2016-06-20 (Last edit)
 * @brief  Implementation of Events
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

//NOTE: Include before all other headers
#include "common.h"

#include "libDebug.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define MAX_NR_LISTENERS 10

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static uint8_t nr_listeners = 0;
static event_listener_type listener_pool[MAX_NR_LISTENERS] = {{0}};

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

///
/// @brief Connect an listener to an event type.
///
/// @param  listener Function pointer to an listener function
/// @param  id Event id to register to
/// @return None
///
void Event_AddListener(event_callback listener, event_id_type id)
{
    sc_assert(listener != NULL);
    sc_assert(nr_listeners < MAX_NR_LISTENERS);

    listener_pool[nr_listeners].callback = listener;
    listener_pool[nr_listeners].id = id;
    ++nr_listeners;

    return;
}

///
/// @brief Notify all listeners connected to the triggered event.
///
/// @param  event Pointer to triggered event
/// @return None
///
void Event_Trigger(const event_type *event)
{
    sc_assert(nr_listeners < MAX_NR_LISTENERS);

    uint8_t index;
    for (index = 0; index < nr_listeners; ++index)
    {
        if (listener_pool[index].id == event->id ||
                listener_pool[index].id == EVENT_ALL)
        {
            sc_assert(listener_pool[index].callback != NULL);
            listener_pool[index].callback(event);
        }
    }
    return;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////