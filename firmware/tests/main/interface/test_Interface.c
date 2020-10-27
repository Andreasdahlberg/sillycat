/**
 * @file   test_Interface.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2020-10-21 (Last edit)
 * @brief  Test suite for the Interface module.
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
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include <stdbool.h>

#include "test_Interface.h"
#include "Interface.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define NUM_TEST_VIEWS 8
#define view_ptr(idx) (&test_views[idx])

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static struct view test_views[NUM_TEST_VIEWS];

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static void MockDrawView(uint16_t context)
{
    check_expected(context);
}

static void MockAction(uint16_t context)
{
    check_expected(context);
}

static void ResetViews(void)
{
    for (uint8_t index = 0; index < NUM_TEST_VIEWS; ++index)
    {
        view_ptr(index)->draw_function = NULL;
        view_ptr(index)->child = NULL;
        view_ptr(index)->prev = NULL;
        view_ptr(index)->next = NULL;
        view_ptr(index)->parent = NULL;
    }
}

static int Setup(void **state)
{
    ResetViews();

    expect_function_call(__wrap_libDisplay_Init);
    expect_function_call(__wrap_libDisplay_On);
    expect_any(__wrap_libDisplay_Rotate, state);

    Interface_Init();

    return 0;
}

//////////////////////////////////////////////////////////////////////////
//TESTS
//////////////////////////////////////////////////////////////////////////

void test_Init(void **state)
{
    expect_function_call(__wrap_libDisplay_Init);
    expect_function_call(__wrap_libDisplay_On);
    expect_any(__wrap_libDisplay_Rotate, state);

    Interface_Init();
}

void test_InitView(void **state)
{
    const uint16_t context = 2;

    view_ptr(0)->child = view_ptr(1);
    view_ptr(0)->parent = view_ptr(2);
    view_ptr(0)->next = view_ptr(3);
    view_ptr(0)->prev = view_ptr(4);

    Interface_InitView(view_ptr(0), MockDrawView, context);

    assert_ptr_equal(view_ptr(0)->draw_function, MockDrawView);
    assert_ptr_equal(view_ptr(0)->context, context);
    assert_null(view_ptr(0)->action_function);
    assert_null(view_ptr(0)->child);
    assert_null(view_ptr(0)->prev);
    assert_null(view_ptr(0)->next);
    assert_null(view_ptr(0)->parent);

    Interface_InitView(view_ptr(0), NULL, 0);
    assert_null(view_ptr(0)->draw_function);
}

void test_GetRootView_None(void **state)
{
    assert_null(Interface_GetRootView());
}

void test_GetRootView_OneView(void **state)
{
    Interface_AddView(view_ptr(0));
    assert_ptr_equal(Interface_GetRootView(), view_ptr(0));
}

void test_GetRootView_TwoViews(void **state)
{
    Interface_AddView(view_ptr(0));
    Interface_AddView(view_ptr(1));
    assert_ptr_equal(Interface_GetRootView(), view_ptr(0));
}

void test_GetRootView_OneViewWithOneChild(void **state)
{
    Interface_AddChild(view_ptr(0), view_ptr(1));
    Interface_AddView(view_ptr(0));
    assert_ptr_equal(Interface_GetRootView(), view_ptr(0));
}

void test_AddView_NullWithRootView(void **state)
{
    struct view *root_view;

    Interface_AddView(view_ptr(0));
    Interface_AddView(NULL);

    root_view = Interface_GetRootView();

    assert_null(root_view->next);
}

void test_AddView_NullWithoutRootView(void **state)
{
    Interface_AddView(NULL);
    assert_null(Interface_GetRootView());
}

void test_AddView_OneView(void **state)
{
    Interface_AddView(view_ptr(0));
    assert_ptr_equal(Interface_GetRootView(), view_ptr(0));
}

void test_AddView_SeveralViews(void **state)
{
    Interface_AddView(view_ptr(0));
    Interface_AddView(view_ptr(1));
    Interface_AddView(view_ptr(2));

    assert_ptr_equal(test_views[0].next, view_ptr(1));
    assert_ptr_equal(test_views[1].prev, view_ptr(0));
    assert_ptr_equal(test_views[1].next, view_ptr(2));
    assert_ptr_equal(test_views[2].prev, view_ptr(1));
}

void test_RemoveView_Null(void **state)
{
    //TODO: Fix this test
    Interface_RemoveView(NULL);
}

void test_RemoveView_RootViewAlone(void **state)
{
    Interface_AddView(view_ptr(0));
    Interface_RemoveView(view_ptr(0));

    assert_null(Interface_GetRootView());
}

void test_RemoveView_ActiveView(void **state)
{
    Interface_AddView(view_ptr(0));
    Interface_AddView(view_ptr(1));

    will_return(__wrap_Timer_GetMilliseconds, 0);
    Interface_NextView();

    Interface_RemoveView(view_ptr(1));

    assert_ptr_not_equal(Interface_GetActiveView(), view_ptr(1));
}

void test_RemoveView_ActiveViewWithParent(void **state)
{
    Interface_AddChild(view_ptr(0), view_ptr(1));
    Interface_AddView(view_ptr(0));

    will_return(__wrap_Timer_GetMilliseconds, 0);
    Interface_ActivateView();

    Interface_RemoveView(view_ptr(1));
    assert_ptr_equal(Interface_GetActiveView(), view_ptr(0));
}

void test_RemoveView_RootViewWithOneSibling(void **state)
{
    Interface_AddView(view_ptr(0));
    Interface_AddView(view_ptr(1));
    Interface_RemoveView(view_ptr(0));

    assert_ptr_equal(Interface_GetRootView(), view_ptr(1));
    assert_ptr_equal(Interface_GetActiveView(), view_ptr(1));
}

void test_RemoveView_RootViewWithChild(void **state)
{
    Interface_AddChild(view_ptr(0), view_ptr(1));
    Interface_AddView(view_ptr(0));
    Interface_RemoveView(view_ptr(0));

    assert_null(Interface_GetRootView());
}

void test_RemoveView_ViewWithPrevSibling(void **state)
{
    struct view *test_view;

    Interface_AddView(view_ptr(0));
    Interface_AddView(view_ptr(1));
    Interface_RemoveView(view_ptr(1));

    test_view = Interface_GetRootView();
    assert_null(test_view->next);
}

void test_RemoveView_ViewWithPrevAndNextSibling(void **state)
{
    Interface_AddView(view_ptr(0));
    Interface_AddView(view_ptr(1));
    Interface_AddView(view_ptr(2));
    Interface_RemoveView(view_ptr(1));

    assert_ptr_equal(test_views[0].next, view_ptr(2));
    assert_ptr_equal(test_views[2].prev, view_ptr(0));
}

void test_RemoveView_ChildViewWithSibling(void **state)
{
    Interface_AddSibling(view_ptr(1), view_ptr(2));
    Interface_AddChild(view_ptr(0), view_ptr(1));
    Interface_AddView(view_ptr(0));
    Interface_RemoveView(view_ptr(1));

    assert_ptr_equal(view_ptr(0)->child, view_ptr(2));
}

void test_AddAction(void **state)
{
    Interface_AddAction(view_ptr(0), MockAction);
    assert_ptr_equal(view_ptr(0)->action_function, MockAction);

    Interface_AddAction(view_ptr(0), NULL);
    assert_null(view_ptr(0)->action_function);
}

void test_Action(void **state)
{
    const uint16_t context = 3;
    view_ptr(0)->context = context;;

    /* Expect nothing to happen if there is no active view. */
    Interface_Action();

    /* Expect nothing to happen if there is no action set. */
    Interface_AddView(view_ptr(0));
    Interface_Action();

    Interface_AddAction(view_ptr(0), MockAction);
    expect_value(MockAction, context, context);
    Interface_Action();
}

void test_AddChild_NullChildWithNoExistingChild(void **state)
{
    Interface_AddChild(view_ptr(0), NULL);
    assert_null(view_ptr(0)->child);
}

void test_AddChild_NullChildWithExistingChild(void **state)
{
    Interface_AddChild(view_ptr(0), view_ptr(1));
    Interface_AddChild(view_ptr(0), NULL);
    assert_ptr_equal(view_ptr(0)->child, view_ptr(1));
}

void test_AddChild_NullParentWithNoExistingChild(void **state)
{
    Interface_AddChild(NULL, view_ptr(0));
    //TODO: What to assert here, is no exception good enough?
}

void test_AddChild_OneChild(void **state)
{
    Interface_AddChild(view_ptr(0), view_ptr(1));
    assert_ptr_equal(view_ptr(0)->child, view_ptr(1));
}

void test_AddChild_TwoChildren(void **state)
{
    Interface_AddChild(view_ptr(0), view_ptr(1));
    Interface_AddChild(view_ptr(0), view_ptr(2));

    assert_ptr_equal(view_ptr(0)->child, view_ptr(1));
    assert_ptr_equal(view_ptr(1)->next, view_ptr(2));
}

void test_NextView_NoRootView(void **state)
{
    Interface_NextView();
    assert_null(Interface_GetActiveView());
}

void test_NextView_OneView(void **state)
{
    will_return(__wrap_Timer_GetMilliseconds, 0);

    Interface_AddView(view_ptr(0));
    Interface_NextView();

    assert_ptr_equal(Interface_GetActiveView(), view_ptr(0));
}

void test_NextView_TwoViews(void **state)
{
    will_return(__wrap_Timer_GetMilliseconds, 0);

    Interface_AddView(view_ptr(0));
    Interface_AddView(view_ptr(1));
    Interface_NextView();

    assert_ptr_equal(Interface_GetActiveView(), view_ptr(1));
}

void test_PreviousView_NoRootView(void **state)
{
    Interface_PreviousView();
    assert_null(Interface_GetActiveView());
}

void test_PreviousView_OneView(void **state)
{
    will_return(__wrap_Timer_GetMilliseconds, 0);

    Interface_AddView(view_ptr(0));
    Interface_PreviousView();

    assert_ptr_equal(Interface_GetActiveView(), view_ptr(0));
}

void test_PreviousView_TwoViews(void **state)
{
    will_return(__wrap_Timer_GetMilliseconds, 0);
    will_return(__wrap_Timer_GetMilliseconds, 0);

    Interface_AddView(view_ptr(0));
    Interface_AddView(view_ptr(1));
    Interface_NextView();
    Interface_PreviousView();

    assert_ptr_equal(Interface_GetActiveView(), view_ptr(0));
}

void test_ActivateView_NoRootView(void **state)
{
    Interface_ActivateView();
    assert_null(Interface_GetActiveView());
}

void test_ActivateView_ActivateOnceWithNoChild(void **state)
{
    will_return(__wrap_Timer_GetMilliseconds, 0);

    Interface_AddView(view_ptr(0));
    Interface_ActivateView();

    assert_ptr_equal(Interface_GetActiveView(), view_ptr(0));
}

void test_ActivateView_ActivateOnceWithChild(void **state)
{
    will_return(__wrap_Timer_GetMilliseconds, 0);

    Interface_AddChild(view_ptr(0), view_ptr(1));
    Interface_AddView(view_ptr(0));
    Interface_ActivateView();

    assert_ptr_equal(Interface_GetActiveView(), view_ptr(1));
}

void test_ActivateView_ActivateTwiceWithChild(void **state)
{
    will_return(__wrap_Timer_GetMilliseconds, 0);
    will_return(__wrap_Timer_GetMilliseconds, 0);

    Interface_AddChild(view_ptr(0), view_ptr(1));
    Interface_AddView(view_ptr(0));
    Interface_ActivateView();
    Interface_ActivateView();

    assert_ptr_equal(Interface_GetActiveView(), view_ptr(0));
}

void test_Update_NoRootView(void **state)
{
    will_return(__wrap_Timer_TimeDifference, 0);
    will_return(__wrap_Timer_GetMilliseconds, 0);

    Interface_Update();
}

void test_Update_ViewWithNoDrawFunction(void **state)
{
    will_return(__wrap_Timer_TimeDifference, 0);
    will_return(__wrap_Timer_GetMilliseconds, 0);

    Interface_AddView(view_ptr(0));
    Interface_Update();
}

void test_Update_ViewWithDrawFunction(void **state)
{
    will_return(__wrap_Timer_TimeDifference, 0);
    will_return(__wrap_Timer_TimeDifference, 0);
    will_return(__wrap_Timer_GetMilliseconds, 0);

    const uint16_t context = 1;

    Interface_InitView(view_ptr(0), MockDrawView, context);
    Interface_AddView(view_ptr(0));

    expect_function_call(__wrap_libUI_Update);
    expect_value(MockDrawView, context, context);
    Interface_Update();
}

void test_Update_AutoRefresh(void **state)
{
    /* First update */
    will_return(__wrap_Timer_TimeDifference, 0);
    will_return(__wrap_Timer_GetMilliseconds, 0);
    will_return(__wrap_Timer_TimeDifference, 0);

    /* Second update */
    will_return(__wrap_Timer_TimeDifference, 101);

    /* Third update */
    will_return(__wrap_Timer_TimeDifference, 110);
    will_return(__wrap_Timer_GetMilliseconds, 110);
    will_return(__wrap_Timer_TimeDifference, 0);

    /* Fourth update */
    will_return(__wrap_Timer_TimeDifference, 211);

    const uint16_t context = 2;
    Interface_InitView(view_ptr(0), MockDrawView, context);
    Interface_AddView(view_ptr(0));

    /* Expect a automatic refresh in the first and third update */
    expect_function_call(__wrap_libUI_Update);
    expect_value(MockDrawView, context, context);
    Interface_Update();
    Interface_Update();
    expect_function_call(__wrap_libUI_Update);
    expect_value(MockDrawView, context, context);
    Interface_Update();
    Interface_Update();
}

void test_Update_ForcedRefresh(void **state)
{
    /* First update */
    will_return(__wrap_Timer_TimeDifference, 0);
    will_return(__wrap_Timer_GetMilliseconds, 0);
    will_return(__wrap_Timer_TimeDifference, 0);

    /* Second update */
    will_return(__wrap_Timer_TimeDifference, 101);

    /* Third update */
    will_return(__wrap_Timer_TimeDifference, 0);
    will_return(__wrap_Timer_GetMilliseconds, 0);
    will_return(__wrap_Timer_TimeDifference, 0);
    /* Fourth update */
    will_return(__wrap_Timer_TimeDifference, 0);

    const uint16_t context = 3;
    Interface_InitView(view_ptr(0), MockDrawView, context);

    /**
     * Expect an automatic refresh in the first update and a forced refresh in
     *  the third update
     */
    Interface_AddView(view_ptr(0));
    expect_function_call(__wrap_libUI_Update);
    expect_value(MockDrawView, context, context);
    Interface_Update();
    Interface_Update();
    Interface_Refresh();
    expect_function_call(__wrap_libUI_Update);
    expect_value(MockDrawView, context, context);
    Interface_Update();
    Interface_Update();
}

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test(test_Init),
        cmocka_unit_test_setup(test_InitView, Setup),
        cmocka_unit_test_setup(test_GetRootView_None, Setup),
        cmocka_unit_test_setup(test_GetRootView_OneView, Setup),
        cmocka_unit_test_setup(test_GetRootView_TwoViews, Setup),
        cmocka_unit_test_setup(test_GetRootView_OneViewWithOneChild, Setup),
        cmocka_unit_test_setup(test_AddView_NullWithRootView, Setup),
        cmocka_unit_test_setup(test_AddView_NullWithoutRootView, Setup),
        cmocka_unit_test_setup(test_AddView_SeveralViews, Setup),
        cmocka_unit_test_setup(test_RemoveView_Null, Setup),
        cmocka_unit_test_setup(test_RemoveView_RootViewAlone, Setup),
        cmocka_unit_test_setup(test_RemoveView_ActiveView, Setup),
        cmocka_unit_test_setup(test_RemoveView_ActiveViewWithParent, Setup),
        cmocka_unit_test_setup(test_RemoveView_RootViewWithChild, Setup),
        cmocka_unit_test_setup(test_RemoveView_RootViewWithOneSibling, Setup),
        cmocka_unit_test_setup(test_RemoveView_ViewWithPrevSibling, Setup),
        cmocka_unit_test_setup(test_RemoveView_ViewWithPrevAndNextSibling, Setup),
        cmocka_unit_test_setup(test_RemoveView_ChildViewWithSibling, Setup),
        cmocka_unit_test_setup(test_AddAction, Setup),
        cmocka_unit_test_setup(test_Action, Setup),
        cmocka_unit_test_setup(test_AddChild_NullChildWithNoExistingChild, Setup),
        cmocka_unit_test_setup(test_AddChild_NullChildWithExistingChild, Setup),
        cmocka_unit_test_setup(test_AddChild_NullParentWithNoExistingChild, Setup),
        cmocka_unit_test_setup(test_AddChild_OneChild, Setup),
        cmocka_unit_test_setup(test_AddChild_TwoChildren, Setup),
        cmocka_unit_test_setup(test_NextView_NoRootView, Setup),
        cmocka_unit_test_setup(test_NextView_OneView, Setup),
        cmocka_unit_test_setup(test_NextView_TwoViews, Setup),
        cmocka_unit_test_setup(test_PreviousView_NoRootView, Setup),
        cmocka_unit_test_setup(test_PreviousView_OneView, Setup),
        cmocka_unit_test_setup(test_PreviousView_TwoViews, Setup),
        cmocka_unit_test_setup(test_ActivateView_NoRootView, Setup),
        cmocka_unit_test_setup(test_ActivateView_ActivateOnceWithNoChild, Setup),
        cmocka_unit_test_setup(test_ActivateView_ActivateOnceWithChild, Setup),
        cmocka_unit_test_setup(test_ActivateView_ActivateTwiceWithChild, Setup),
        cmocka_unit_test_setup(test_Update_NoRootView, Setup),
        cmocka_unit_test_setup(test_Update_ViewWithNoDrawFunction, Setup),
        cmocka_unit_test_setup(test_Update_ViewWithDrawFunction, Setup),
        cmocka_unit_test_setup(test_Update_AutoRefresh, Setup),
        cmocka_unit_test_setup(test_Update_ForcedRefresh, Setup),
    };

    if (argc >= 2)
    {
        cmocka_set_test_filter(argv[1]);
    }

    return cmocka_run_group_tests(tests, NULL, NULL);
}
