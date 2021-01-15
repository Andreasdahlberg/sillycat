/**
 * @file   libDebug.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2021-01-16 (Last edit)
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

//NOTE: Include before all other headers
#include "common.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "libDebug.h"
#include "UART.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define OUT_BUFFER_SIZE 80
#define WAIT_TIMEOUT_MS 10

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
    UART_Init();
    UART_Enable(true);
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
    char buffer[OUT_BUFFER_SIZE];

    va_start(args, text);

    vsnprintf_P(buffer, OUT_BUFFER_SIZE, text, args);

    //Make sure that the output always is null terminated.
    buffer[OUT_BUFFER_SIZE - 1] = '\n';

    UART_Write((uint8_t *)buffer, strnlen(buffer, sizeof(buffer)));
    va_end(args);
    return;
}

///
/// @brief Flush buffers before sleep
///
/// @param  *event Pointer to triggered event
/// @return None
///
void libDebug_Sleep(const event_t *event __attribute__ ((unused)))
{
    sc_assert(event != NULL);

    UART_WaitForTx(WAIT_TIMEOUT_MS);
    UART_Enable(false);
    return;
}

///
/// @brief Enable UART after sleep
///
/// @param  *event Pointer to triggered event
/// @return None
///
void libDebug_WakeUp(const event_t *event __attribute__ ((unused)))
{
    sc_assert(event != NULL);

    UART_Enable(true);
    return;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////
