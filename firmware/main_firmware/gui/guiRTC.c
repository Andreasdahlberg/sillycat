/**
 * @file   guiRTC.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2016-03-31 (Last edit)
 * @brief  Implementation of guiRTC
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
#include <stdio.h>

#include "libDebug.h"
#include "libUI.h"
#include "Interface.h"
#include "guiRTC.h"
#include "RTC.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static struct view clock_view;
static struct view clock_and_date_view;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

static void DrawClockView(uint16_t context __attribute__ ((unused)));
static void DrawDetailedTimeView(uint16_t context __attribute__ ((unused)));

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

///
/// @brief Init the clock views
///
/// Init clock views and add them to the user interface.
///
/// @param  None
/// @return None
///
void guiRTC_Init(void)
{
    clock_view.draw_function = DrawClockView;
    clock_view.context = 0;
    clock_view.child = NULL;
    clock_view.prev = NULL;
    clock_view.next = NULL;
    clock_view.parent = NULL;

    clock_and_date_view.draw_function = DrawDetailedTimeView;
    clock_and_date_view.context = 0;
    clock_and_date_view.child = NULL;
    clock_and_date_view.prev = NULL;
    clock_and_date_view.next = NULL;
    clock_and_date_view.parent = NULL;

    Interface_AddChild(&clock_view, &clock_and_date_view);
    Interface_AddView(&clock_view);

    INFO("Init done");
    return;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static void DrawClockView(uint16_t context __attribute__ ((unused)))
{
    uint8_t min;
    uint8_t hour;

    RTC_GetMinutes(&min);
    RTC_GetHour(&hour);

    libUI_Print("%02u:%02u", 49, 11, hour, min);
    return;
}

static void DrawDetailedTimeView(uint16_t context __attribute__ ((unused)))
{
    rtc_time_type time;

    RTC_GetCurrentTime(&time);

    libUI_Print("20%02u-%02u-%02u", 31, 2, time.year, time.month, time.date);
    libUI_Print("%02u:%02u:%02u", 40, 16, time.hour, time.minute, time.second);
    return;
}