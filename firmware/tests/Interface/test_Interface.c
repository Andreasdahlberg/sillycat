#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <stdio.h>
#include <stdint.h>

#include "test_Interface.h"
#include "Interface.h"

#define NUM_TEST_VIEWS 8
#define view_ptr(idx) (&test_views[idx])

static struct view test_views[8];
static uint8_t libUI_Updated_called;
static uint16_t DummyDrawView_context;

void __wrap_libUI_Update(void)
{
    ++libUI_Updated_called;
    return;
}

static void DummyDrawView(uint16_t context)
{
    DummyDrawView_context = context;
    return;
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
    libUI_Updated_called = 0;
    DummyDrawView_context = 0;

    ResetViews();
    Interface_Init();
    return 0;
}

void test_GetRootView_None(void **state)
{
    assert_null(Interface_GetRootView());
    return;
}

void test_GetRootView_OneView(void **state)
{
    struct view *test_view;
    Interface_Init();

    Interface_AddView(test_view);
    assert_ptr_equal(Interface_GetRootView(), test_view);
    return;
}

void test_GetRootView_TwoViews(void **state)
{
    Interface_AddView(&test_views[0]);
    Interface_AddView(&test_views[1]);
    assert_ptr_equal(Interface_GetRootView(), &test_views[0]);
    return;
}

void test_GetRootView_OneViewWithOneChild(void **state)
{
    Interface_AddChild(&test_views[0], &test_views[1]);
    Interface_AddView(&test_views[0]);
    assert_ptr_equal(Interface_GetRootView(), &test_views[0]);
    return;
}

void test_AddView_NullWithRootView(void **state)
{
    struct view *root_view;


    Interface_AddView(&test_views[0]);
    Interface_AddView(NULL);

    root_view = Interface_GetRootView();

    assert_null(root_view->next);
    return;
}

void test_AddView_NullWithoutRootView(void **state)
{
    Interface_AddView(NULL);
    assert_null(Interface_GetRootView());
    return;
}

void test_AddView_OneView(void **state)
{
    Interface_AddView(view_ptr(0));
    assert_ptr_equal(Interface_GetRootView(), &test_views[0]);
    return;
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
    return;
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
    struct view *test_view;

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
    will_return(__wrap_libTimer_GetMilliseconds, 0);
    will_return(__wrap_libTimer_GetMilliseconds, 0);

    Interface_Update();

    assert_int_equal(libUI_Updated_called, 0);
}

void test_Update_ViewWithNoDrawFunction(void **state)
{
    will_return(__wrap_libTimer_GetMilliseconds, 0);
    will_return(__wrap_libTimer_GetMilliseconds, 0);

    Interface_AddView(view_ptr(0));
    Interface_Update();

    assert_int_equal(libUI_Updated_called, 0);
}

void test_Update_ViewWithDrawFunction(void **state)
{
    will_return(__wrap_libTimer_GetMilliseconds, 0);
    will_return(__wrap_libTimer_GetMilliseconds, 0);
    will_return(__wrap_libTimer_GetMilliseconds, 0);

    view_ptr(0)->draw_function = DummyDrawView;
    view_ptr(0)->context = 1;

    Interface_AddView(view_ptr(0));
    Interface_Update();

    assert_int_equal(DummyDrawView_context, 1);
}

void test_Update_AutoRefresh(void **state)
{
    //First update
    will_return(__wrap_libTimer_GetMilliseconds, 0);
    will_return(__wrap_libTimer_GetMilliseconds, 0);
    will_return(__wrap_libTimer_GetMilliseconds, 0);

    //Second update
    will_return(__wrap_libTimer_GetMilliseconds, 1001);

    //Third update
    will_return(__wrap_libTimer_GetMilliseconds, 1100);
    will_return(__wrap_libTimer_GetMilliseconds, 1100);
    will_return(__wrap_libTimer_GetMilliseconds, 1100);

    //Fourth update
    will_return(__wrap_libTimer_GetMilliseconds, 2101);


    view_ptr(0)->draw_function = DummyDrawView;
    view_ptr(0)->context = 1;

    Interface_AddView(view_ptr(0));
    Interface_Update();
    Interface_Update();
    Interface_Update();
    Interface_Update();

    //Expect a automatic refresh in the first and third update
    assert_int_equal(libUI_Updated_called, 2);
}

void test_Update_ForcedRefresh(void **state)
{
    //First update
    will_return(__wrap_libTimer_GetMilliseconds, 0);
    will_return(__wrap_libTimer_GetMilliseconds, 0);
    will_return(__wrap_libTimer_GetMilliseconds, 0);

    //Second update
    will_return(__wrap_libTimer_GetMilliseconds, 100);

    //Third update
    will_return(__wrap_libTimer_GetMilliseconds, 101);
    will_return(__wrap_libTimer_GetMilliseconds, 101);
    will_return(__wrap_libTimer_GetMilliseconds, 101);

    //Fourth update
    will_return(__wrap_libTimer_GetMilliseconds, 1102);


    view_ptr(0)->draw_function = DummyDrawView;
    view_ptr(0)->context = 1;

    Interface_AddView(view_ptr(0));
    Interface_Update();
    Interface_Update();
    Interface_Refresh();
    Interface_Update();
    Interface_Update();

    //Expect an automatic refresh in the first update and a forced refresh in the third update
    assert_int_equal(libUI_Updated_called, 2);
}

#if 1
int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup(test_GetRootView_None, setup),
        cmocka_unit_test_setup(test_GetRootView_OneView, setup),
        cmocka_unit_test_setup(test_GetRootView_TwoViews, setup),
        cmocka_unit_test_setup(test_GetRootView_OneViewWithOneChild, setup),
        cmocka_unit_test_setup(test_AddView_NullWithRootView, setup),
        cmocka_unit_test_setup(test_AddView_NullWithoutRootView, setup),
        cmocka_unit_test_setup(test_AddView_SeveralViews, setup),
        cmocka_unit_test_setup(test_RemoveView_Null, setup),
        cmocka_unit_test_setup(test_RemoveView_RootViewAlone, setup),
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
    return cmocka_run_group_tests(tests, setup, NULL);
}
#endif
