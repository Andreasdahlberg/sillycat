/**
 * @file   Event.c
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

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include "common.h"
#include "libDebug.h"
#include "Event.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define MAX_NR_LISTENERS 10

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

struct event_listener_t
{
    event_id_t id;
    event_callback_t callback;
};

struct module_t
{
    uint8_t number_of_listeners;
    struct event_listener_t listeners[MAX_NR_LISTENERS];
};

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static struct module_t module;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void Event_Init(void)
{
    module = (struct module_t) {0};
}

void Event_AddListener(event_callback_t listener_p, event_id_t id)
{
    sc_assert(listener_p != NULL);
    sc_assert(module.number_of_listeners < ElementsIn(module.listeners));

    module.listeners[module.number_of_listeners].callback = listener_p;
    module.listeners[module.number_of_listeners].id = id;
    ++module.number_of_listeners;
}

void Event_Trigger(const event_t *event_p)
{
    sc_assert(event_p != NULL);

    uint8_t index;
    for (index = 0; index < module.number_of_listeners; ++index)
    {
        if (module.listeners[index].id == event_p->id ||
                module.listeners[index].id == EVENT_ALL)
        {
            sc_assert(module.listeners[index].callback != NULL);
            module.listeners[index].callback(event_p);
        }
    }
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////
