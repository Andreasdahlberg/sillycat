/**
 * @file   guiRTC.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-03-11 (Last edit)
 * @brief  Implementation of GUI for displaying the current time.
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
#include "libDebug.h"
#include "libUI.h"
#include "Interface.h"
#include "RTC.h"
#include "guiRTC.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define UTC_OFFSET_MIN 60
#define DST_OFFSET_MIN 60

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

struct module_t
{
    struct
    {
        struct view overview;
        struct view details;
    } view;
};

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static struct module_t module;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

static void DrawClockView(uint16_t context __attribute__ ((unused)));
static void DrawDetailedTimeView(uint16_t context __attribute__ ((unused)));
static void AdjustTimeForView(rtc_time_type *time);

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void guiRTC_Init(void)
{
    Interface_InitView(&module.view.overview, DrawClockView, 0);
    Interface_AddView(&module.view.overview);

    Interface_InitView(&module.view.details, DrawDetailedTimeView, 0);
    Interface_AddChild(&module.view.overview, &module.view.details);

    INFO("Initialized RTC view.");
    return;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static void DrawClockView(uint16_t context __attribute__ ((unused)))
{
    rtc_time_type time;
    RTC_GetCurrentTime(&time);

    AdjustTimeForView(&time);

    libUI_Print("%02u:%02u", 45, UI_SINGLE_ROW, time.hour, time.minute);
    return;
}

static void DrawDetailedTimeView(uint16_t context __attribute__ ((unused)))
{
    rtc_time_type time;
    RTC_GetCurrentTime(&time);

    AdjustTimeForView(&time);

    libUI_Print("20%02u-%02u-%02u", 25, UI_DOUBLE_ROW_FIRST, time.year, time.month, time.date);
    libUI_Print("%02u:%02u:%02u", 34, UI_DOUBLE_ROW_SECOND, time.hour, time.minute, time.second);
    return;
}

static void AdjustTimeForView(rtc_time_type *time_p)
{
    RTC_AddMinutes(time_p, UTC_OFFSET_MIN);

    uint8_t day;
    RTC_GetDay(&day);

    if (RTC_IsDaylightSavingActive(time_p, day))
    {
        RTC_AddMinutes(time_p, DST_OFFSET_MIN);
    }
    return;
}
