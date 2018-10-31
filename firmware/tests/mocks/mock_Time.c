/**
 * @file   mock_Time.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-10-31 (Last edit)
 * @brief  Mock functions for the Time module.
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

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include <stdint.h>
#include "Time.h"

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

void __wrap_Time_FormatTimestamp(const struct time_t *time_p, char *timestamp_p, size_t size)
{
}

uint8_t __wrap_Time_CalculateDayOfWeek(const struct time_t *time_p)
{
    mock_type(uint8_t);
}

bool __wrap_Time_IsLeapYear(const struct time_t *time_p)
{
    mock_type(bool);
}

void __wrap_Time_AddYears(struct time_t *time_p, uint8_t year)
{
}

void __wrap_Time_AddDays(struct time_t *time_p, uint8_t days)
{
}

void __wrap_Time_AddSeconds(struct time_t *time_p, uint8_t seconds)
{
}

void __wrap_Time_AddMinutes(struct time_t *time_p, uint8_t minutes)
{
}

uint8_t __wrap_Time_GetDaysInMonth(const struct time_t *time_p)
{
    mock_type(uint8_t);
}

void __wrap_Time_AddHours(struct time_t *time_p, uint8_t hours)
{
}

bool __wrap_Time_IsDaylightSavingActive(const struct time_t *time_p)
{
    mock_type(bool);
}

void __wrap_Time_AddMonths(struct time_t *time_p, uint8_t months)
{
}

uint32_t __wrap_Time_ConvertToTimestamp(const struct time_t *time_p)
{
    mock_type(uint32_t);
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

