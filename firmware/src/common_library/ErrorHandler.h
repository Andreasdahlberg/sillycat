/**
 * @file   ErrorHandler.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-03-08 (Last edit)
 * @brief  Implementation of ErrorHandler
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

#ifndef ERRORHANDLER_H_
#define ERRORHANDLER_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include "sc_assert.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#ifdef DEBUG_ENABLE
#define ErrorHandler_PrintLog() ErrorHandler_DumpLog()
#else
#define ErrorHandler_PrintLog()
#endif

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

enum
{
    POWERON = 1,
    ASSFAIL,
    LOW_STACK,
    RTC_FAIL
};

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

void ErrorHandler_Init(void);
void ErrorHandler_LogError(uint8_t code, uint8_t information);
void ErrorHandler_PointOfNoReturn(void) __attribute__((noreturn));
void ErrorHandler_AssertFail(const char *__file, int __lineno,
                             const char *__sexp) __attribute__((noreturn));
#ifdef DEBUG_ENABLE
void ErrorHandler_DumpLog(void);
#endif

#endif
