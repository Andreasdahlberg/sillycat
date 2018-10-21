/**
 * @file   test_Event.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-10-21 (Last edit)
 * @brief  Test suite for the Event module.
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

#include "Event.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

#define MAX_NUMBER_OF_LISTENERS 10

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static int Setup(void **state)
{
    Event_Init();

    return 0;
}

static void FakeEventHandlerOne(const event_t *event_p)
{
    function_called();
}

static void FakeEventHandlerTwo(const event_t *event_p)
{
    function_called();
}

static void FakeEventHandlerWithAsserts(const event_t *event_p)
{
    assert_non_null(event_p);
    assert_int_equal(Event_GetId(event_p), mock_type(event_id_t));
    assert_int_equal(Event_GetTimestamp(event_p), mock_type(uint32_t));
}

//////////////////////////////////////////////////////////////////////////
//TESTS
//////////////////////////////////////////////////////////////////////////

static void test_Event_Init(void **state)
{
    /**
     * `Event_Init()` is indirectly tested by it's use in `Setup()` right now.
     */
    skip();
}

static void test_Event_New(void **state)
{
    event_id_t expected_id = EVENT_SLEEP;
    uint32_t expected_timestamp = 100;

    will_return(__wrap_Timer_GetMilliseconds, expected_timestamp);
    event_t event = Event_New(expected_id);

    assert_int_equal(Event_GetId(&event), expected_id);
    assert_int_equal(Event_GetTimestamp(&event), expected_timestamp);

    expected_id = EVENT_WAKEUP;
    expected_timestamp = 529;

    will_return(__wrap_Timer_GetMilliseconds, expected_timestamp);
    event = Event_New(expected_id);

    assert_int_equal(Event_GetId(&event), expected_id);
    assert_int_equal(Event_GetTimestamp(&event), expected_timestamp);
}

static void test_Event_AddListener_NULL(void **state)
{
    expect_assert_failure(Event_AddListener(NULL, 0));
}

static void test_Event_AddListener_MaxNumberOfListeners(void **state)
{
    /* Fill up all listener spots. */
    for (size_t i = 0; i < MAX_NUMBER_OF_LISTENERS; ++i)
    {
        Event_AddListener(FakeEventHandlerOne, i);
    }

    expect_assert_failure(Event_AddListener(FakeEventHandlerOne, 0));
}

static void test_Event_Trigger_NULL(void **state)
{
    expect_assert_failure(Event_Trigger(NULL));
}

static void test_Event_Trigger_EventAll(void **state)
{
    Event_AddListener(FakeEventHandlerWithAsserts, EVENT_ALL);

    event_id_t expected_events[] = {EVENT_SLEEP, EVENT_NR_EVENTS - 1};

    for (size_t i = 0; i < sizeof(expected_events) / sizeof(expected_events[0]); ++i)
    {
        const uint32_t expected_timestamp = (uint32_t)i;

        will_return(__wrap_Timer_GetMilliseconds, expected_timestamp);
        will_return(FakeEventHandlerWithAsserts, expected_events[i]);
        will_return(FakeEventHandlerWithAsserts, expected_timestamp);

        event_t event = Event_New(expected_events[i]);

        Event_Trigger(&event);
    }
}

static void test_Event_Trigger(void **state)
{
    Event_AddListener(FakeEventHandlerOne, EVENT_WAKEUP);
    Event_AddListener(FakeEventHandlerTwo, EVENT_SLEEP);

    will_return(__wrap_Timer_GetMilliseconds, 0);
    event_t event = Event_New(EVENT_WAKEUP);

    expect_function_call(FakeEventHandlerOne);
    Event_Trigger(&event);

    will_return(__wrap_Timer_GetMilliseconds, 0);
    event = Event_New(EVENT_SLEEP);

    expect_function_call(FakeEventHandlerTwo);
    Event_Trigger(&event);
}

static void test_Event_GetId_NULL(void **state)
{
    expect_assert_failure(Event_GetId(NULL));
}

static void test_Event_GetTimestamp_NULL(void **state)
{
    expect_assert_failure(Event_GetId(NULL));
}

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test(test_Event_Init),
        cmocka_unit_test_setup(test_Event_New, Setup),
        cmocka_unit_test_setup(test_Event_AddListener_NULL, Setup),
        cmocka_unit_test_setup(test_Event_AddListener_MaxNumberOfListeners, Setup),
        cmocka_unit_test_setup(test_Event_Trigger_NULL, Setup),
        cmocka_unit_test_setup(test_Event_Trigger_EventAll, Setup),
        cmocka_unit_test_setup(test_Event_Trigger, Setup),
        cmocka_unit_test_setup(test_Event_GetId_NULL, Setup),
        cmocka_unit_test_setup(test_Event_GetTimestamp_NULL, Setup)
    };

    if (argc >= 2)
    {
        cmocka_set_test_filter(argv[1]);
    }

    return cmocka_run_group_tests(tests, NULL, NULL);
}
