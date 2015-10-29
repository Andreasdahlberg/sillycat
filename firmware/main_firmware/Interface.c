/**
 * @file   Interface.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2015-10-29 (Last edit)
 * @brief  Implementation of Interface
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

#include <stdio.h>

#include "libDebug.h"
#include "libDisplay.h"
#include "libDS3234.h"
#include "libInput.h"
#include "libTimer.h"
#include "libUI.h"

#include "bitmap_db.h"
#include "Interface.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 32

#define REFRESH_RATE_MS 1000

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static bool refresh_flag;
static uint32_t activity_timer;

static struct view main_view;
static struct view *current_view;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

static void RightRotationDetected(void);
static void LeftRotationDetected(void);
static void PushDetected(void);

static void DrawClockView(void);
static void DrawDetailedTimeView(void);
static void DrawViewIndicator(void);

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

///
/// @brief Init interface, display will be turned on and a font will be set.
///        A main view with time and date is created.
///
/// @param  None
/// @return None
///
void Interface_Init(void)
{
    static struct view time_view;

    libDisplay_Init();
    libDisplay_On();

    libUI_SetFont(&ubuntuMono_10ptFontInfo);

    libInput_SetCallbacks(RightRotationDetected, LeftRotationDetected,
                          PushDetected);

    //TODO: Does this init belong here? Move views to main?
    main_view.draw_function = DrawClockView;
    main_view.child = NULL;
    main_view.prev = NULL;
    main_view.next = NULL;
    main_view.parent = NULL;

    time_view.draw_function = DrawDetailedTimeView;
    time_view.child = NULL;
    time_view.prev = NULL;
    time_view.next = NULL;
    time_view.parent = NULL;

    Interface_AddChild(&main_view, &time_view);

    refresh_flag = TRUE;
    current_view = &main_view;

    INFO("Init done");
    return;
}

///
/// @brief Add a sibling to an existing view
///
/// @param  *sibling_view Pointer to sibling view
/// @param  *new_view Pointer to new view
/// @return None
///
void Interface_AddSibling(struct view *sibling_view, struct view *new_view)
{
    struct view *view_ptr;
    view_ptr = sibling_view;

    //TODO: Add condition to protect against loops, ex next = current
    while (view_ptr->next != NULL)
    {
        view_ptr = view_ptr->next;
    }

    new_view->prev = view_ptr;
    view_ptr->next = new_view;
    return;
}

///
/// @brief Add a new view as a sibling to the main view
///
/// @param  *new_view Pointer to new view
/// @return None
///
void Interface_AddView(struct view *new_view)
{
    Interface_AddSibling(&main_view, new_view);
    return;
}

///
/// @brief Add a child to an existing view. If the parent already got a child
///        the new view will be added as a sibling to the existing one.
///
/// @param  *parent_view Pointer to parent view
/// @param  *child_view Pointer to child view
/// @return None
///
void Interface_AddChild(struct view *parent_view, struct view *child_view)
{
    // If the parent already got a child add the new child as a sibling to
    // the old one.
    if (parent_view->child != NULL)
    {
        Interface_AddSibling(parent_view->child, child_view);
    }
    else
    {
        parent_view->child = child_view;
    }

    child_view->parent = parent_view;
    return;
}

///
/// @brief Update interface if needed, call as fast as possible.
///
/// @param  None
/// @return None
///
void Interface_Update(void)
{
    static uint32_t auto_timer = 0;

    if (refresh_flag == TRUE)
    {
        if (current_view->draw_function != NULL)
        {
            libDisplay_ClearVRAM();
            current_view->draw_function();
            if (libTimer_TimeDifference(activity_timer) < 2000)
            {
                DrawViewIndicator();
            }
            libUI_Update();
        }
        else
        {
            ERROR("Invalid draw function");
        }
        refresh_flag = FALSE;
        auto_timer = libTimer_GetMilliseconds();
    }

    if (libTimer_TimeDifference(auto_timer) > REFRESH_RATE_MS)
    {
        refresh_flag = TRUE;
    }
    return;
}

///
/// @brief Sets the refresh flag to true. The interface will be refreshed
///        next time Interface_Update() is called.
///
/// @param  None
/// @return None
///
void Interface_Refresh(void)
{
    refresh_flag = TRUE;
    return;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static void DrawViewIndicator(void)
{
    if (current_view->next == NULL)
    {
        libUI_DrawLine(DISPLAY_WIDTH - 1, 0, DISPLAY_WIDTH - 1,
                       DISPLAY_HEIGHT - 1);
    }

    if (current_view->prev == NULL)
    {
        libUI_DrawLine(0, 0, 0, DISPLAY_HEIGHT - 1);
    }
    return;
}

static void DrawClockView(void)
{
    char text_buffer[20];
    uint8_t min;
    uint8_t hour;

    libDS3234_GetMinutes(&min);
    libDS3234_GetHour(&hour);
    sprintf(text_buffer, "%02u:%02u", hour, min);

    libUI_PrintText(text_buffer, 49, 11);
    return;
}

static void DrawDetailedTimeView(void)
{
    char text_buffer[20];
    uint8_t year;
    uint8_t month;
    uint8_t date;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;

    libDS3234_GetYear(&year);
    libDS3234_GetMonth(&month);
    libDS3234_GetDate(&date);

    sprintf(text_buffer, "20%02u-%02u-%02u", year, month, date);
    libUI_PrintText(text_buffer, 31, 2);

    libDS3234_GetMinutes(&min);
    libDS3234_GetHour(&hour);
    libDS3234_GetSeconds(&sec);
    sprintf(text_buffer, "%02u:%02u:%02u", hour, min, sec);
    libUI_PrintText(text_buffer, 40, 16);
    return;
}

static void LeftRotationDetected(void)
{
    if (current_view->prev != NULL)
    {
        current_view = current_view->prev;
    }

    refresh_flag = TRUE;
    activity_timer = libTimer_GetMilliseconds();
    return;
}

static void RightRotationDetected(void)
{
    if (current_view->next != NULL)
    {
        current_view = current_view->next;
    }

    refresh_flag = TRUE;
    activity_timer = libTimer_GetMilliseconds();
    return;
}

static void PushDetected(void)
{
    if (current_view->child != NULL)
    {
        current_view = current_view->child;
    }
    else if (current_view->parent != NULL)
    {
        current_view = current_view->parent;
    }

    refresh_flag = TRUE;
    activity_timer = libTimer_GetMilliseconds();
    return;
}