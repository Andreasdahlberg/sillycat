/**
 * @file   test_Interface.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-09-18 (Last edit)
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
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include <stdint.h>
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

static struct view test_views[8];
static uint16_t DummyDrawView_context;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static void DummyDrawView(uint16_t context)
{
    DummyDrawView_context = context;
}

static void ResetViews(void)
{
    uint8_t index;

    for (index = 0; index < NUM_TEST_VIEWS; ++index)
    {
        test_views[index].draw_function = NULL;
        test_views[index].child = NULL;
        test_views[index].prev = NULL;
        test_views[index].next = NULL;
        test_views[index].parent = NULL;
    }
}

static int setup(void **state)
{
    DummyDrawView_context = 0;

    ResetViews();
    Interface_Init();
    return 0;
}

//////////////////////////////////////////////////////////////////////////
//TESTS
//////////////////////////////////////////////////////////////////////////

void test_GetRootView_None(void **state)
{
    assert_null(Interface_GetRootView());
}

void test_GetRootView_OneView(void **state)
{
    struct view *test_view;
    Interface_Init();

    Interface_AddView(test_view);
    assert_ptr_equal(Interface_GetRootView(), test_view);
}

void test_GetRootView_TwoViews(void **state)
{
    Interface_AddView(&test_views[0]);
    Interface_AddView(&test_views[1]);
    assert_ptr_equal(Interface_GetRootView(), &test_views[0]);
}

void test_GetRootView_OneViewWithOneChild(void **state)
{
    Interface_AddChild(&test_views[0], &test_views[1]);
    Interface_AddView(&test_views[0]);
    assert_ptr_equal(Interface_GetRootView(), &test_views[0]);
}

void test_AddView_NullWithRootView(void **state)
{
    struct view *root_view;

    Interface_AddView(&test_views[0]);
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
    assert_ptr_equal(Interface_GetRootView(), &test_views[0]);
}

void test_AddView_SeveralViews(void **state)
{
    Interface_AddView(&test_views[0]);
    Interface_AddView(&test_views[1]);
    Interface_AddView(&test_views[2]);

    assert_ptr_equal(test_views[0].next, &test_views[1]);
    assert_ptr_equal(test_views[1].prev, &test_views[0]);
    assert_ptr_equal(test_views[1].next, &test_views[2]);
    assert_ptr_equal(test_views[2].prev, &test_views[1]);
}

void test_RemoveView_Null(void **state)
{
    //TODO: Fix this test
    Interface_RemoveView(NULL);
}

void test_RemoveView_RootViewAlone(void **state)
{
    Interface_AddView(&test_views[0]);
    Interface_RemoveView(&test_views[0]);

    assert_null(Interface_GetRootView());
}

void test_RemoveView_ActiveView(void **state)
{
    Interface_AddView(&test_views[0]);
    Interface_AddView(&test_views[1]);

    will_return(__wrap_libTimer_GetMilliseconds, 0);
    Interface_NextView();

    Interface_RemoveView(&test_views[1]);

    assert_ptr_not_equal(Interface_GetActiveView(), &test_views[1]);
}

void test_RemoveView_RootViewWithOneSibling(void **state)
{
    Interface_AddView(&test_views[0]);
    Interface_AddView(&test_views[1]);
    Interface_RemoveView(&test_views[0]);

    assert_ptr_equal(Interface_GetRootView(), &test_views[1]);
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

    Interface_AddView(&test_views[0]);
    Interface_AddView(&test_views[1]);
    Interface_RemoveView(&test_views[1]);

    test_view = Interface_GetRootView();
    assert_null(test_view->next);
}

void test_RemoveView_ViewWithPrevAndNextSibling(void **state)
{
    Interface_AddView(&test_views[0]);
    Interface_AddView(&test_views[1]);
    Interface_AddView(&test_views[2]);
    Interface_RemoveView(&test_views[1]);

    assert_ptr_equal(test_views[0].next, &test_views[2]);
    assert_ptr_equal(test_views[2].prev, &test_views[0]);
}

void test_RemoveView_ChildViewWithSibling(void **state)
{
    Interface_AddSibling(view_ptr(1), view_ptr(2));
    Interface_AddChild(view_ptr(0), view_ptr(1));
    Interface_AddView(view_ptr(0));
    Interface_RemoveView(view_ptr(1));

    assert_ptr_equal(view_ptr(0)->child, view_ptr(2));
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
    will_return(__wrap_libTimer_GetMilliseconds, 0);

    Interface_AddView(view_ptr(0));
    Interface_NextView();

    assert_ptr_equal(Interface_GetActiveView(), view_ptr(0));
}

void test_NextView_TwoViews(void **state)
{
    will_return(__wrap_libTimer_GetMilliseconds, 0);

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
    will_return(__wrap_libTimer_GetMilliseconds, 0);

    Interface_AddView(view_ptr(0));
    Interface_PreviousView();

    assert_ptr_equal(Interface_GetActiveView(), view_ptr(0));
}

void test_PreviousView_TwoViews(void **state)
{
    will_return(__wrap_libTimer_GetMilliseconds, 0);
    will_return(__wrap_libTimer_GetMilliseconds, 0);

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
    will_return(__wrap_libTimer_GetMilliseconds, 0);

    Interface_AddView(view_ptr(0));
    Interface_ActivateView();

    assert_ptr_equal(Interface_GetActiveView(), view_ptr(0));
}

void test_ActivateView_ActivateOnceWithChild(void **state)
{
    will_return(__wrap_libTimer_GetMilliseconds, 0);

    Interface_AddChild(view_ptr(0), view_ptr(1));
    Interface_AddView(view_ptr(0));
    Interface_ActivateView();

    assert_ptr_equal(Interface_GetActiveView(), view_ptr(1));
}

void test_ActivateView_ActivateTwiceWithChild(void **state)
{
    will_return(__wrap_libTimer_GetMilliseconds, 0);
    will_return(__wrap_libTimer_GetMilliseconds, 0);

    Interface_AddChild(view_ptr(0), view_ptr(1));
    Interface_AddView(view_ptr(0));
    Interface_ActivateView();
    Interface_ActivateView();

    assert_ptr_equal(Interface_GetActiveView(), view_ptr(0));
}

void test_Update_NoRootView(void **state)
{
    will_return(__wrap_Timer_TimeDifference, 0);
    will_return(__wrap_libTimer_GetMilliseconds, 0);

    Interface_Update();
}

void test_Update_ViewWithNoDrawFunction(void **state)
{
    will_return(__wrap_Timer_TimeDifference, 0);
    will_return(__wrap_libTimer_GetMilliseconds, 0);

    Interface_AddView(view_ptr(0));
    Interface_Update();
}

void test_Update_ViewWithDrawFunction(void **state)
{
    will_return(__wrap_Timer_TimeDifference, 0);
    will_return(__wrap_Timer_TimeDifference, 0);
    will_return(__wrap_libTimer_GetMilliseconds, 0);

    view_ptr(0)->draw_function = DummyDrawView;
    view_ptr(0)->context = 1;

    Interface_AddView(view_ptr(0));
    expect_function_call(__wrap_libUI_Update);
    Interface_Update();

    assert_int_equal(DummyDrawView_context, 1);
}

void test_Update_AutoRefresh(void **state)
{
    /* First update */
    will_return(__wrap_Timer_TimeDifference, 0);
    will_return(__wrap_libTimer_GetMilliseconds, 0);
    will_return(__wrap_Timer_TimeDifference, 0);

    /* Second update */
    will_return(__wrap_Timer_TimeDifference, 101);

    /* Third update */
    will_return(__wrap_Timer_TimeDifference, 110);
    will_return(__wrap_libTimer_GetMilliseconds, 110);
    will_return(__wrap_Timer_TimeDifference, 0);

    /* Fourth update */
    will_return(__wrap_Timer_TimeDifference, 211);

    view_ptr(0)->draw_function = DummyDrawView;
    view_ptr(0)->context = 1;

    /* Expect a automatic refresh in the first and third update */
    Interface_AddView(view_ptr(0));
    expect_function_call(__wrap_libUI_Update);
    Interface_Update();
    Interface_Update();
    expect_function_call(__wrap_libUI_Update);
    Interface_Update();
    Interface_Update();
}

void test_Update_ForcedRefresh(void **state)
{
    /* First update */
    will_return(__wrap_Timer_TimeDifference, 0);
    will_return(__wrap_libTimer_GetMilliseconds, 0);
    will_return(__wrap_Timer_TimeDifference, 0);

    /* Second update */
    will_return(__wrap_Timer_TimeDifference, 101);

    /* Third update */
    will_return(__wrap_Timer_TimeDifference, 0);
    will_return(__wrap_libTimer_GetMilliseconds, 0);
    will_return(__wrap_Timer_TimeDifference, 0);
    /* Fourth update */
    will_return(__wrap_Timer_TimeDifference, 0);


    view_ptr(0)->draw_function = DummyDrawView;
    view_ptr(0)->context = 1;

    /**
     * Expect an automatic refresh in the first update and a forced refresh in
     *  the third update
     */
    Interface_AddView(view_ptr(0));
    expect_function_call(__wrap_libUI_Update);
    Interface_Update();
    Interface_Update();
    Interface_Refresh();
    expect_function_call(__wrap_libUI_Update);
    Interface_Update();
    Interface_Update();
}

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

int main(void)
{
    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test_setup(test_GetRootView_None, setup),
        cmocka_unit_test_setup(test_GetRootView_OneView, setup),
        cmocka_unit_test_setup(test_GetRootView_TwoViews, setup),
        cmocka_unit_test_setup(test_GetRootView_OneViewWithOneChild, setup),
        cmocka_unit_test_setup(test_AddView_NullWithRootView, setup),
        cmocka_unit_test_setup(test_AddView_NullWithoutRootView, setup),
        cmocka_unit_test_setup(test_AddView_SeveralViews, setup),
        cmocka_unit_test_setup(test_RemoveView_Null, setup),
        cmocka_unit_test_setup(test_RemoveView_RootViewAlone, setup),
        cmocka_unit_test_setup(test_RemoveView_ActiveView, setup),
        cmocka_unit_test_setup(test_RemoveView_RootViewWithChild, setup),
        cmocka_unit_test_setup(test_RemoveView_RootViewWithOneSibling, setup),
        cmocka_unit_test_setup(test_RemoveView_ViewWithPrevSibling, setup),
        cmocka_unit_test_setup(test_RemoveView_ViewWithPrevAndNextSibling, setup),
        cmocka_unit_test_setup(test_RemoveView_ChildViewWithSibling, setup),
        cmocka_unit_test_setup(test_AddChild_NullChildWithNoExistingChild, setup),
        cmocka_unit_test_setup(test_AddChild_NullChildWithExistingChild, setup),
        cmocka_unit_test_setup(test_AddChild_NullParentWithNoExistingChild, setup),
        cmocka_unit_test_setup(test_AddChild_OneChild, setup),
        cmocka_unit_test_setup(test_AddChild_TwoChildren, setup),
        cmocka_unit_test_setup(test_NextView_NoRootView, setup),
        cmocka_unit_test_setup(test_NextView_OneView, setup),
        cmocka_unit_test_setup(test_NextView_TwoViews, setup),
        cmocka_unit_test_setup(test_PreviousView_NoRootView, setup),
        cmocka_unit_test_setup(test_PreviousView_OneView, setup),
        cmocka_unit_test_setup(test_PreviousView_TwoViews, setup),
        cmocka_unit_test_setup(test_ActivateView_NoRootView, setup),
        cmocka_unit_test_setup(test_ActivateView_ActivateOnceWithNoChild, setup),
        cmocka_unit_test_setup(test_ActivateView_ActivateOnceWithChild, setup),
        cmocka_unit_test_setup(test_ActivateView_ActivateTwiceWithChild, setup),
        cmocka_unit_test_setup(test_Update_NoRootView, setup),
        cmocka_unit_test_setup(test_Update_ViewWithNoDrawFunction, setup),
        cmocka_unit_test_setup(test_Update_ViewWithDrawFunction, setup),
        cmocka_unit_test_setup(test_Update_AutoRefresh, setup),
        cmocka_unit_test_setup(test_Update_ForcedRefresh, setup),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
