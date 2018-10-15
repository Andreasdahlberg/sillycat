/**
 * @file   ErrorHandler.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-10-15 (Last edit)
 * @brief  Implementation of ErrorHandler
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
    RTC_FAIL,
    CORRUPT_CONFIG,
    HIGH_MCU_TEMPERATURE
};

typedef void (*error_handler_assert_fail_t)(
    const char *file,
    int line_number,
    const char *expression
);

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

/**
 * Initialize the error handler.
 *
 * Set the current index and id in the error log. This needs to be run once
 * after power on before the error logging can begin.
 */
void ErrorHandler_Init(void);

/**
 * Set a handler function for assert failures.
 *
 * @param error_handler_p Handler function pointer. NULL can be used to
 *                        disable the assert failure handler.
 */
void ErrorHandler_SetAssertFailHandler(
    error_handler_assert_fail_t error_handler_p);

/**
 * Log an error code with data and timestamp to the error log.
 *
 * The error log is stored in EEPROM so it's preserved between restarts.
 * If the log is full the oldest entries will be overwritten.
 *
 * @param code        Error code to save.
 * @param information Additional information to save.
 */
void ErrorHandler_LogError(uint8_t code, uint8_t information);

/**
 * Halt the device and indicate that an fault has occurred.
 *
 * This function does never return, a manual reboot is needed to exit this
 * state.
 */
void ErrorHandler_PointOfNoReturn(void) __attribute__((noreturn));

/**
 * Print information about the failed an assert and then halt.
 *
 * This function does never return, a manual reboot is needed to exit this
 * state.
 *
 * @param __file   String with the name of the file where the assert failed.
 * @param __lineno Line number of the failed assert.
 * @param __exp    String with the assert expression that returned false.
 */
void ErrorHandler_AssertFail(const char *__file,
                             int __lineno,
                             const char *__exp) __attribute__((noreturn));
#ifdef DEBUG_ENABLE
/**
 * Print all logged errors.
 */
void ErrorHandler_DumpLog(void);
#endif

#endif
