/**
 * @file   test_Encoder.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-09-22 (Last edit)
 * @brief  Test suite for the Encoder module.
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

#include "test_Encoder.h"
#include "Encoder.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//INTERUPT SERVICE ROUTINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static int Setup(void **state)
{
    expect_function_call(__wrap_driverPEC11_Init);
    Encoder_Init();

    return 0;
}

static void FakeRightCallback(void)
{
    function_called();
}

static void FakeLeftCallback(void)
{
    function_called();
}

static void FakeBriefPushCallback(void)
{
    function_called();
}

static void FakeExtendedPushCallback(void)
{
    function_called();
}

//////////////////////////////////////////////////////////////////////////
//TESTS
//////////////////////////////////////////////////////////////////////////

/**
 * Expect the hardware driver to be initialized during module initialization.
 */
static void test_Encoder_Init(void **state)
{
    expect_function_call(__wrap_driverPEC11_Init);
    Encoder_Init();
}

/**
 * Expect the callbacks to be cleared(NULL) after each call
 * to 'Encoder_Init'.
 */
static void test_Encoder_Init_ClearCallbacks(void **state)
{
    struct encoder_callbacks_t callbacks =
    {
        .right = FakeRightCallback,
        .left = FakeLeftCallback,
        .brief_push = FakeBriefPushCallback,
        .extended_push = FakeExtendedPushCallback
    };
    Encoder_SetCallbacks(&callbacks);

    expect_function_call(__wrap_driverPEC11_Init);
    Encoder_Init();

    callbacks = Encoder_GetCallbacks();
    assert_null(callbacks.right);
    assert_null(callbacks.left);
    assert_null(callbacks.brief_push);
    assert_null(callbacks.extended_push);
}

static void test_Encoder_SetGetCallbacks(void **state)
{
    struct encoder_callbacks_t callbacks =
    {
        .right = FakeRightCallback,
        .left = FakeLeftCallback,
        .brief_push = FakeBriefPushCallback,
        .extended_push = FakeExtendedPushCallback
    };
    Encoder_SetCallbacks(&callbacks);

    callbacks = Encoder_GetCallbacks();
    assert_ptr_equal(callbacks.right, FakeRightCallback);
    assert_ptr_equal(callbacks.left, FakeLeftCallback);
    assert_ptr_equal(callbacks.brief_push, FakeBriefPushCallback);
    assert_ptr_equal(callbacks.extended_push, FakeExtendedPushCallback);

    callbacks = (struct encoder_callbacks_t) {0};
    Encoder_SetCallbacks(&callbacks);

    callbacks = Encoder_GetCallbacks();
    assert_null(callbacks.right);
    assert_null(callbacks.left);
    assert_null(callbacks.brief_push);
    assert_null(callbacks.extended_push);
}

/**
 * Expect nothing to happen if no callbacks are set.
 */
static void test_Encoder_Update_NoCallbacks(void **state)
{
    Encoder_Update();
}

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test(test_Encoder_Init),
        cmocka_unit_test_setup(test_Encoder_Init_ClearCallbacks, Setup),
        cmocka_unit_test_setup(test_Encoder_SetGetCallbacks, Setup),
        cmocka_unit_test_setup(test_Encoder_Update_NoCallbacks, Setup)
    };

    if (argc >= 2)
    {
        cmocka_set_test_filter(argv[1]);
    }

    return cmocka_run_group_tests(tests, NULL, NULL);
}
