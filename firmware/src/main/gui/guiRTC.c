/**
 * @file   guiRTC.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-03-15 (Last edit)
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
#include "libInput.h"
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
    struct
    {
        bool active;
        size_t index;
        rtc_time_type time;
    } set_time;
};

struct limits_t
{
    uint8_t max;
    uint8_t min;
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
static void DrawUnderLine(void);
static void DrawSetTimeView(uint16_t context __attribute__ ((unused)));
static void AdjustTimeForView(rtc_time_type *time);
static void SetTimeAction(uint16_t context __attribute__ ((unused)));
static struct limits_t GetCurrentFieldLimits(size_t field_index, rtc_time_type *time_p);
static void AdjustTimeToLimits(rtc_time_type *time_p);
static void IncreaseField(void);
static void DecreaseField(void);

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void guiRTC_Init(void)
{
    module.set_time.active = false;
    module.set_time.index = 0;

    Interface_InitView(&module.view.overview, DrawClockView, 0);
    Interface_AddView(&module.view.overview);

    Interface_InitView(&module.view.details, DrawDetailedTimeView, 0);
    Interface_AddAction(&module.view.details, SetTimeAction);
    Interface_AddChild(&module.view.overview, &module.view.details);

    INFO("Initialized RTC view.");
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
}

static void DrawDetailedTimeView(uint16_t context __attribute__ ((unused)))
{
    rtc_time_type time;
    RTC_GetCurrentTime(&time);

    AdjustTimeForView(&time);

    libUI_Print("20%02u-%02u-%02u", 25, UI_DOUBLE_ROW_FIRST, time.year, time.month, time.date);
    libUI_Print("%02u:%02u:%02u", 34, UI_DOUBLE_ROW_SECOND, time.hour, time.minute, time.second);
}

static void DrawUnderLine(void)
{
    struct underline_t
    {
        uint8_t x;
        uint8_t y;
        uint8_t length;
    };

    const struct underline_t lines[6] =
    {
        {
            .x = 25,
            .y = UI_DOUBLE_ROW_FIRST + 1,
            .length = 32
        },
        {
            .x = 65,
            .y = UI_DOUBLE_ROW_FIRST + 1,
            .length = 16
        },
        {
            .x = 89,
            .y = UI_DOUBLE_ROW_FIRST + 1,
            .length = 16
        },
        {
            .x = 34,
            .y = UI_DOUBLE_ROW_SECOND + 1,
            .length = 16
        },
        {
            .x = 58,
            .y = UI_DOUBLE_ROW_SECOND + 1,
            .length = 16
        },
        {
            .x = 82,
            .y = UI_DOUBLE_ROW_SECOND + 1,
            .length = 16
        },
    };

    const struct underline_t *line_p = &lines[module.set_time.index];

    libUI_DrawLine(line_p->x, line_p->y, line_p->x + line_p->length, line_p->y);
}

static void DrawSetTimeView(uint16_t context __attribute__ ((unused)))
{
    rtc_time_type time;
    time = module.set_time.time;

    AdjustTimeForView(&time);

    libUI_Print("20%02u-%02u-%02u", 25, UI_DOUBLE_ROW_FIRST, time.year, time.month, time.date);
    libUI_Print("%02u:%02u:%02u", 34, UI_DOUBLE_ROW_SECOND, time.hour, time.minute, time.second);

    DrawUnderLine();
}

static void AdjustTimeForView(rtc_time_type *time_p)
{
    /**
     * Always add offset since time is stored as UTC,
     */
    RTC_AddMinutes(time_p, UTC_OFFSET_MIN);

    uint8_t day = RTC_CalculateDayOfWeek(time_p);

    if (RTC_IsDaylightSavingActive(time_p, day))
    {
        RTC_AddMinutes(time_p, DST_OFFSET_MIN);
    }
}

static void NextField(void)
{
    if (module.set_time.index < 5)
    {
        ++module.set_time.index;
    }
    else
    {
        module.set_time.index = 0;
    }

    /**
     * Call refresh so that the underline marking the current field is updated
     * immediately.
     */
    Interface_Refresh();
}

static struct limits_t GetCurrentFieldLimits(
    size_t field_index,
    rtc_time_type *time_p)
{
    struct limits_t limits;

    switch (field_index)
    {
        case 0:
            limits.max = 99;
            limits.min = 0;
            break;

        case 1:
            limits.max = 12;
            limits.min = 1;
            break;

        case 2:
            limits.max = RTC_GetDaysInMonth(time_p);
            limits.min = 1;
            break;

        case 3:
            limits.max = 23;
            limits.min = 0;
            break;

        case 4:
        case 5:
            limits.max = 59;
            limits.min = 0;
            break;

        default:
            sc_assert_fail();
            break;
    };

    return limits;
}

static void AdjustTimeToLimits(rtc_time_type *time_p)
{
    for (size_t i = 0; i < sizeof(*time_p); ++i)
    {
        struct limits_t limits = GetCurrentFieldLimits(i, time_p);

        uint8_t *field_p = ((uint8_t *)&module.set_time.time) + i;

        if (*field_p > limits.max)
        {
            *field_p = limits.max;
        }
        else if (*field_p < limits.min)
        {
            *field_p = limits.min;
        }
    }
}

static void IncreaseField(void)
{
    uint8_t *field_p = ((uint8_t *)&module.set_time.time) + module.set_time.index;

    struct limits_t limits;
    limits = GetCurrentFieldLimits(module.set_time.index, &module.set_time.time);

    if (*field_p < limits.max)
    {
        *field_p += 1;
    }
    else
    {
        *field_p = limits.min;
    }
    AdjustTimeToLimits(&module.set_time.time);

    /**
     * Call refresh so that the updated value is updated immediately.
     */
    Interface_Refresh();
}

static void DecreaseField(void)
{
    uint8_t *field_p = ((uint8_t *)&module.set_time.time) + module.set_time.index;

    struct limits_t limits;
    limits = GetCurrentFieldLimits(module.set_time.index, &module.set_time.time);

    if (*field_p > limits.min)
    {
        *field_p -= 1;
    }
    else
    {
        *field_p = limits.max;
    }
    AdjustTimeToLimits(&module.set_time.time);

    /**
     * Call refresh so that the updated value is updated immediately.
     */
    Interface_Refresh();
}

static void SetTimeAction(uint16_t context __attribute__ ((unused)))
{
    if (module.set_time.active)
    {
        /**
         * The callbacks need to be restored to the default functions
         * so that the user can leave the 'set time' view.
         */
        libInput_SetCallbacks(Interface_NextView,
                              Interface_PreviousView,
                              Interface_ActivateView,
                              Interface_Action);

        RTC_SetCurrentTime(&module.set_time.time);

        module.view.details.draw_function = DrawDetailedTimeView;
        module.set_time.active = false;

        INFO("New time set");
    }
    else
    {
        /**
         * Hijack the input callbacks so that they are used to change the
         * time instead of navigation.
         */
        libInput_SetCallbacks(IncreaseField,
                              DecreaseField,
                              NextField,
                              Interface_Action);

        RTC_GetCurrentTime(&module.set_time.time);

        /* TODO: Use 'DrawDetailedTimeView' */
        module.view.details.draw_function = DrawSetTimeView;
        module.set_time.active = true;
        module.set_time.index = 0;
    }
}
