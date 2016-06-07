/**
 * @file   ErrorHandler.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2016-06-07 (Last edit)
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

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

//NOTE: Include common.h before all other headers
#include "common.h"

#include <avr/wdt.h>
#include <avr/eeprom.h>

#include "libDebug.h"
#include "ErrorHandler.h"
#include "RTC.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define ERROR_LOG_SIZE 80

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

typedef struct
{
    uint32_t id;
    uint32_t timestamp;
    uint8_t code;
    uint8_t information;
} error_message_type;

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static error_message_type EEMEM error_log[ERROR_LOG_SIZE] = {{0}};

static uint8_t current_index = 0;
static uint8_t current_id = 0;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//INTERUPT SERVICE ROUTINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

///
/// @brief Set the current index and id in the error log. This needs to be run
///        once after power on before the error logging can begin.
///
/// @param  None
/// @return None
///
void ErrorHandler_Init(void)
{
    sc_assert(ERROR_LOG_SIZE < 256);

    uint32_t prev_id = 0;
    uint8_t index;

    for (index = 0; index < sizeof(error_log) / sizeof(*error_log); ++index)
    {
        sc_assert(index < ERROR_LOG_SIZE);

        uint32_t id;
        eeprom_read_block(&id, &error_log[index].id, sizeof(id));

        if (id <= prev_id)
        {
            current_index = index;
            current_id = ++prev_id;
            DEBUG("Current error log index: %u\r\n", index);
            return;
        }
        prev_id = id;
    }

    //If we come here the log is full and we wrap around to the start.
    current_index = 0;
    current_id = ++prev_id;
    return;
}

///
/// @brief Log an error code with data and timestamp to the non-volatile
//         error log. If the log is full the oldest entries will be overwritten.
///
/// @param  code Error code to save
/// @param  information Additional information to save
/// @return None
///
void ErrorHandler_LogError(uint8_t code, uint8_t information)
{
    sc_assert(current_index < ERROR_LOG_SIZE);
    sc_assert(current_id  != 0);

    error_message_type log_entry;

    //NOTE: Save log entry even if we fail to get a timestamp. It's better then
    //      nothing and it makes no sense to make an error log entry about a failed
    //      error log entry.
    RTC_GetTimeStamp(&log_entry.timestamp);
    log_entry.id = current_id;
    log_entry.code = code;
    log_entry.information = information;

    eeprom_write_block(&log_entry, &error_log[current_index], sizeof(*error_log));

    //Increment log index and wrap around if the end is reached.
    current_index = (current_index + 1) % sizeof(error_log) / sizeof(*error_log);

    //No need for wrap around here, if uint32 is used the id will never overflow
    //during normal operation. (One log entry per second for 100 years and this still works!)
    ++current_id;
    return;
}

///
/// @brief Print information about the failed an assert and then halt.
///
/// @param  *__func Pointer to string with the name of the function where
///                 the assert failed.
/// @param  *__file Pointer to string with the name of the file where
///                 the assert failed.
/// @param  __lineno Line number of the failed assert.
/// @param  *__exp Pointer to string with the assert expression that
///                returned false.
/// @return None, does not return since an manual reboot is needed.
///
void ErrorHandler_AssertFail(const char *__file, int __lineno,
                             const char *__exp)
{
    char expression[40];
    char file[24];

    //TODO: Use libDebug_PrintP, skip these buffers!

    strncpy_P(file, __file, sizeof(file));
    file[sizeof(file) / sizeof(*file) - 1] = '\0';

    strncpy_P(expression, __exp, sizeof(expression));
    expression[sizeof(expression) / sizeof(*expression) - 1] = '\0';

    DEBUG("<ERROR> Failed assert: %s:%u (%s)\r\n", file, __lineno, expression);
    ErrorHandler_PointOfNoReturn();
}

///
/// @brief Halt device and indicate that an fault has occurred.
///
/// @param  None
/// @return None, does not return since an manual reboot is needed.
///
void ErrorHandler_PointOfNoReturn(void)
{
    CRITICAL("Entering fail state, manual reboot is needed.");

    wdt_disable();
    while (1)
    {
    }
}

#ifdef DEBUG_ENABLE
///
/// @brief Print all logged errors.
///
/// @param  None
/// @return None
///
void ErrorHandler_DumpLog(void)
{
    uint8_t index;

    DEBUG("<LOG>\r\n");
    for (index = 0; index < sizeof(error_log) / sizeof(*error_log); ++index)
    {
        error_message_type entry;
        eeprom_read_block(&entry, &error_log[index], sizeof(entry));

        if (entry.id == 0)
        {
            DEBUG("</LOG>\r\n");
            return;
        }

        DEBUG("%u,%lu,%u,", index, entry.id, entry.code);
        DEBUG("%u,%lu\r\n", entry.information, entry.timestamp);

    }
    DEBUG("</LOG>\r\n");
    return;
}
#endif

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////