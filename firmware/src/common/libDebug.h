/**
 * @file   libDebug.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-10-20 (Last edit)
 * @brief  Header of Debug-library.
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

#ifndef LIBDEBUG_H_
#define LIBDEBUG_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include "Event.h"

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

#ifdef DEBUG_ENABLE
#include <avr/pgmspace.h>

#define DEBUG(text, ...)        libDebug_Print_P(PSTR(text), ##__VA_ARGS__)
#define CRITICAL(text, ...)     libDebug_Print_P(PSTR("<CRITICAL> " text "\r\n"), ##__VA_ARGS__)
#define ERROR(text, ...)        libDebug_Print_P(PSTR("<ERROR> " text "\r\n"), ##__VA_ARGS__)
#define WARNING(text, ...)      libDebug_Print_P(PSTR("<WARNING> " text "\r\n"), ##__VA_ARGS__)
#define INFO(text, ...)         libDebug_Print_P(PSTR("<INFO> " text "\r\n"), ##__VA_ARGS__)
#else
#define DEBUG(...)
#define CRITICAL(text, ...)
#define ERROR(text, ...)
#define WARNING(text, ...)
#define INFO(text, ...)
#endif

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

void libDebug_Init(void);
void libDebug_Print_P(const char *text, ...);
void libDebug_Sleep(const event_t *event __attribute__ ((unused)));
void libDebug_WakeUp(const event_t *event __attribute__ ((unused)));

#endif /* LIBDEBUG_H_ */
