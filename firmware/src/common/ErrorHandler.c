/**
 * @file   ErrorHandler.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-10-13 (Last edit)
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

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

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

struct error_message_type
{
    uint32_t id;
    uint32_t timestamp;
    uint8_t code;
    uint8_t information;
};

struct module_t
{
    uint8_t current_index;
    uint8_t current_id;
};

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static struct error_message_type EEMEM error_log[ERROR_LOG_SIZE];
static struct module_t module;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

void FillLogEntry(struct error_message_type *entry,
                  uint8_t code,
                  uint8_t information
                 );

//////////////////////////////////////////////////////////////////////////
//INTERUPT SERVICE ROUTINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void ErrorHandler_Init(void)
{
    uint32_t prev_id = 0;

    _Static_assert(ERROR_LOG_SIZE <= UINT8_MAX, "Invalid error log size!");

    for (uint8_t i = 0; i < ElementsIn(error_log); ++i)
    {
        uint32_t id;
        eeprom_read_block(&id, &error_log[i].id, sizeof(id));

        if (id <= prev_id)
        {
            module.current_index = i;
            module.current_id = ++prev_id;
            DEBUG("Current error log index: %u\r\n", i);
            return;
        }
        prev_id = id;
    }

    /* If we come here the log is full and we wrap around to the start. */
    module.current_index = 0;
    module.current_id = ++prev_id;
}

void ErrorHandler_LogError(uint8_t code, uint8_t information)
{
    struct error_message_type log_entry;
    FillLogEntry(&log_entry, code, information);

    eeprom_write_block(&log_entry, &error_log[module.current_index], sizeof(*error_log));

    /* Increment log index and wrap around if the end is reached. */
    module.current_index = (module.current_index + 1) % ElementsIn(error_log);

    /**
     * No need for wrap around here, if uint32 is used the id will never overflow
     * during normal operation. One log entry per second for 100 years and
     * this still works!
     */
    ++module.current_id;
}

void ErrorHandler_AssertFail(const char *__file, int __lineno,
                             const char *__exp)
{
#ifdef DEBUG_ENABLE
    libDebug_Print_P(PSTR("<ERROR> "));
    libDebug_Print_P(__file);
    libDebug_Print_P(PSTR(":%u ("), __lineno);
    libDebug_Print_P(__exp);
    libDebug_Print_P(PSTR(")\r\n"));
#else
    UNUSED(__file);
    UNUSED(__lineno);
    UNUSED(__exp);
#endif

    ErrorHandler_PointOfNoReturn();
}

void ErrorHandler_PointOfNoReturn(void)
{
    CRITICAL("Entering fail state.");

    wdt_disable();
    while (1)
    {
    }
}

#ifdef DEBUG_ENABLE
void ErrorHandler_DumpLog(void)
{
    DEBUG("****Error log****\r\n");
    for (uint8_t i = 0; i < ElementsIn(error_log); ++i)
    {
        struct error_message_type entry;
        eeprom_read_block(&entry, &error_log[i], sizeof(entry));

        if (entry.id == 0)
        {
            return;
        }

        DEBUG("Index: %u\r\n", i);
        DEBUG("Id: %u\r\n", entry.id);
        DEBUG("Code: %u\r\n", entry.code);
        DEBUG("Info: %u\r\n", entry.information);
        DEBUG("Timestamp: %lu\r\n", entry.timestamp);
        DEBUG("*****************\r\n");
    }
}
#endif

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void FillLogEntry(struct error_message_type *entry,
                  uint8_t code,
                  uint8_t information
                 )
{
    /**
     * Fill log entry even if we fail to get a timestamp. It's better then
     * nothing and it makes no sense to make an error log entry about a failed
     * error log entry.
     */
    RTC_GetTimeStamp(&entry->timestamp);
    entry->id = module.current_id;
    entry->code = code;
    entry->information = information;
}
