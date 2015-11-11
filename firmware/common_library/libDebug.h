/**
 * @file   libDebug.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2015-11-11 (Last edit)
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

#include "common.h"
#include "libUART.h"

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

#ifdef DEBUG_ENABLE
#define DEBUG(text, ...)		libDebug_Print_P(PSTR(text), ##__VA_ARGS__)
#define CRITICAL(text, ...)     libDebug_Print_P(PSTR("<CRITICAL> %s() " text "\r\n"), __func__, ##__VA_ARGS__)
#define ERROR(text, ...)        libDebug_Print_P(PSTR("<ERROR> %s() " text "\r\n"), __func__, ##__VA_ARGS__)
#define WARNING(text, ...)      libDebug_Print_P(PSTR("<WARNING> %s() " text "\r\n"), __func__, ##__VA_ARGS__)
#define INFO(text, ...)         libDebug_Print_P(PSTR("<INFO> %s() " text "\r\n"), __func__, ##__VA_ARGS__)
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

void libDebug_Init();
void libDebug_Print_P(const char *text, ...);

#endif /* LIBDEBUG_H_ */