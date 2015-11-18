/**
 * @file   libDebug.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2015-11-18 (Last edit)
 * @brief  Implementation of Debug-library.
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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "libDebug.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

///
/// @brief Init debug library
///
/// @param  None
/// @return None
///
void libDebug_Init(void)
{
#ifdef DEBUG_ENABLE
    libUART_Init();
#endif
    INFO("Init done");
    return;
}

///
/// @brief Print string from FLASH
///
/// @param  text Text to print
/// @param  variable number of arguments
/// @return None
///
void libDebug_Print_P(const char *text, ...)
{
    va_list args;
    //TODO: Find another safe solution for this, these buffers is too large!
    char buffer[80];
    char format[64];

    strncpy_P(format, text, 64);

    va_start(args, text);
    vsprintf(buffer, format, args);
    libUART_SendArray((uint8_t *)buffer, strlen(buffer));
    va_end(args);
    return;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////