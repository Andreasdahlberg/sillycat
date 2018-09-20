/**
 * @file   Interface.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-09-20 (Last edit)
 * @brief  Implementation of Interface functions
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

#include "libDebug.h"
#include "libDisplay.h"
#include "libUI.h"

#include "Interface.h"
#include "Timer.h"

#include "guiInterface.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define REFRESH_RATE_MS 100

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static bool refresh_flag;
static uint32_t activity_timer;

static struct view *root_view;
static struct view *active_view;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

static void DrawViewIndicator(void);

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

///
/// @brief Init interface, display will be turned on and a font will be set.
///
/// @param  None
/// @return None
///
void Interface_Init(void)
{
    libDisplay_Init();
    libDisplay_On();

    root_view = NULL;
    active_view = NULL;
    refresh_flag = true;

    INFO("Init done");
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

    if (refresh_flag == true)
    {
        if (active_view != NULL && active_view->draw_function != NULL)
        {
            libDisplay_ClearVRAM();
            active_view->draw_function(active_view->context);
            if (Timer_TimeDifference(activity_timer) < 2000)
            {
                DrawViewIndicator();
            }
            libUI_Update();
        }
        refresh_flag = false;
        auto_timer = Timer_GetMilliseconds();
    }

    if (Timer_TimeDifference(auto_timer) > REFRESH_RATE_MS)
    {
        refresh_flag = true;
    }
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

    if (sibling_view != NULL && new_view != NULL)
    {
        //TODO: Add condition to protect against loops, ex view_ptr->next = view_ptr
        while (view_ptr->next != NULL)
        {
            view_ptr = view_ptr->next;
        }

        new_view->prev = view_ptr;
        view_ptr->next = new_view;
    }
    return;
}

///
/// @brief Initialize a new view
///
/// @param  *view Pointer to view
/// @param  *draw_function Pointer to view draw function
/// @param  context View context
/// @return None
///
void Interface_InitView(struct view *view,
                        interface_fp draw_function,
                        uint16_t context)
{
    view->draw_function = draw_function;
    view->action_function = NULL;
    view->context = context;
    view->child = NULL;
    view->prev = NULL;
    view->next = NULL;
    view->parent = NULL;

    return;
}

///
/// @brief Add a new view as a sibling to the root view
///
/// @param  *new_view Pointer to new view
/// @return None
///
void Interface_AddView(struct view *new_view)
{
    if (root_view == NULL)
    {
        INFO("New view added as root");
        //Set view as root view if there is no previous root view
        root_view = new_view;
        active_view = new_view;
    }
    else
    {
        INFO("New view added as sibling to root");
        Interface_AddSibling(root_view, new_view);
    }
    return;
}

///
/// @brief Remove a view from the interface
///
/// @param  *view Pointer to view to remove
/// @return None
///
void Interface_RemoveView(struct view *view)
{
    if (view != NULL)
    {
        if (view->next != NULL)
        {
            view->next->prev = view->prev;
        }

        if (view->prev != NULL)
        {
            view->prev->next = view->next;
        }

        //Check if view is the first child view or the root view
        if (view->parent != NULL && view->prev == NULL && view->next != NULL)
        {
            view->parent->child = view->next;
        }
        else if (view->parent == NULL && view->prev == NULL)
        {
            root_view = view->next;
        }

        if (view == active_view)
        {
            if (view->prev != NULL)
            {
                active_view = view->prev;
            }
            else if (view->next != NULL)
            {
                active_view = view->next;
            }
            else if (view->parent != NULL)
            {
                active_view = view->parent;
            }
        }
    }
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
    if (parent_view != NULL && child_view != NULL)
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
    }
    return;
}

///
/// @brief Add action on long press to view.
///
/// @param  *view Pointer to view
/// @param  *action_function Pointer to view action function
/// @return None
///
void Interface_AddAction(struct view *view, interface_fp action_function)
{
    view->action_function = action_function;
    return;
}

///
/// @brief Get the root view.
///
/// @param  None
/// @return struct view * Pointer to the root view
///
struct view *Interface_GetRootView(void)
{
    return root_view;
}

///
/// @brief Get the active view.
///
/// @param  None
/// @return struct view * Pointer to the active view
///
struct view *Interface_GetActiveView(void)
{
    return active_view;
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
    refresh_flag = true;
    return;
}

///
/// @brief Change the active view to the previous one. If the no previous view
///        is available nothing will happen.
///
/// @param  None
/// @return None
///
void Interface_PreviousView(void)
{
    if (active_view != NULL)
    {
        if (active_view->prev != NULL)
        {
            active_view = active_view->prev;
        }

        refresh_flag = true;
        activity_timer = Timer_GetMilliseconds();
    }
    return;
}

///
/// @brief Change the active view to the next one. If the no next view
///        is available nothing will happen.
///
/// @param  None
/// @return None
///
void Interface_NextView(void)
{
    if (active_view != NULL)
    {
        if (active_view->next != NULL)
        {
            active_view = active_view->next;
        }

        refresh_flag = true;
        activity_timer = Timer_GetMilliseconds();
    }
    return;
}

///
/// @brief Change the active view to the child view of the current active view.
///        If the no child view is available nothing will happen.
///
/// @param  None
/// @return None
///
void Interface_ActivateView(void)
{
    if (active_view != NULL)
    {
        if (active_view->child != NULL)
        {
            active_view = active_view->child;
        }
        else if (active_view->parent != NULL)
        {
            active_view = active_view->parent;
        }

        refresh_flag = true;
        activity_timer = Timer_GetMilliseconds();
    }
    return;
}

///
/// @brief Perform any action associated with the currently active view.
///
/// @param  None
/// @return None
///
void Interface_Action(void)
{
    if (active_view != NULL && active_view->action_function != NULL)
    {
        active_view->action_function(active_view->context);
    }

    return;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static void DrawViewIndicator(void)
{
    if (active_view->next == NULL)
    {
        guiInterface_DrawViewIndicator(INDICATOR_POS_RIGHT);
    }

    if (active_view->prev == NULL)
    {
        guiInterface_DrawViewIndicator(INDICATOR_POS_LEFT);
    }
    return;
}
