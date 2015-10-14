/**
 * @file   Interface.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2015-10-04 (Last edit)
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

#include <avr/interrupt.h> 
#include <util/atomic.h>
#include <string.h>
#include <stdio.h>

#include "common.h"
#include "libDebug.h"
#include "libTimer.h"
#include "libDS3234.h"
#include "libInput.h"
#include "libDisplay.h"
#include "libUI.h"

#include "Interface.h"
#include "bitmap_db.h"

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
static void DrawViewIndicator(void);

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void Interface_Init(void)
{
    libDisplay_Init();
    libDisplay_On();

    libUI_SetFont(&ubuntuMono_10ptFontInfo);

    libInput_SetCallbacks(RightRotationDetected, LeftRotationDetected,
                          PushDetected);
    
    main_view.draw_function = DrawClockView;
    main_view.child = NULL;
    main_view.prev = NULL;
    main_view.next = NULL;
    main_view.parent = NULL;
    
    refresh_flag = TRUE;      
    current_view = &main_view;
   
    INFO("Init done");
    return;
}

void Interface_AddSibling(struct view *sibling_view, struct view *new_view)
{
    struct view *view_ptr;
    view_ptr = sibling_view;
    
    while (view_ptr->next != NULL)
    {
        view_ptr = view_ptr->next;
    }
    
    new_view->prev = view_ptr;
    view_ptr->next = new_view;    
}

void Interface_AddView(struct view *new_view)
{
    Interface_AddSibling(&main_view, new_view);     
    return;
}

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

void Interface_Update(void)
{   
    static uint32_t auto_timer = 0;
    
    if (refresh_flag == TRUE)
    {
        libDisplay_ClearVRAM();
        current_view->draw_function();
        if (libTimer_TimeDifference(activity_timer) < 2000)
        {
            DrawViewIndicator();
        }            
        libUI_Update();
        
        refresh_flag = FALSE;
        auto_timer = libTimer_GetMilliseconds();        
    }
    
    if (libTimer_TimeDifference(auto_timer) > REFRESH_RATE_MS)
    {
        refresh_flag = TRUE;
    }
}

///
/// @brief Sets the refresh flag to true. The interface will be refreshed
///        next time the Interface-module is updated.
///
/// @param  None
/// @return None
///
void Interface_Refresh(void)
{
    refresh_flag = TRUE;
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
    sprintf(text_buffer, "%02u:%02u",hour, min);  
    
    libUI_PrintText(text_buffer, 49,11);       
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