/**
 * @file   Encoder.c
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

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include "common.h"

#include "driverPEC11.h"
#include "libDebug.h"
#include "Encoder.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

struct module_t
{
    struct encoder_callbacks_t callbacks;
};

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static struct module_t module;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//INTERUPT SERVICE ROUTINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void Encoder_Init(void)
{
    module = (struct module_t) { {0}};
    driverPEC11_Init();

    INFO("Encoder module initialized");
}

void Encoder_Update(void)
{
    if (module.callbacks.right != NULL && driverPEC11_PopRightRotation())
    {
        module.callbacks.right();
    }
    else if (module.callbacks.left != NULL && driverPEC11_PopLeftRotation())
    {
        module.callbacks.left();
    }
    else if (module.callbacks.brief_push != NULL && driverPEC11_PopBriefPush())
    {
        module.callbacks.brief_push();
    }
    else if (module.callbacks.extended_push != NULL && driverPEC11_PopExtendedPush())
    {
        module.callbacks.extended_push();
    }
}

void Encoder_SetCallbacks(const struct encoder_callbacks_t *callbacks_p)
{
    module.callbacks = *callbacks_p;
}

struct encoder_callbacks_t Encoder_GetCallbacks(void)
{
    return module.callbacks;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////
