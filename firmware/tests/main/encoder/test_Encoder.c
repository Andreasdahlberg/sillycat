/**
 * @file   test_Encoder.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2020-10-21 (Last edit)
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
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
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

static void SetCallbacks(void)
{
    struct encoder_callbacks_t callbacks =
    {
        .right = FakeRightCallback,
        .left = FakeLeftCallback,
        .brief_push = FakeBriefPushCallback,
        .extended_push = FakeExtendedPushCallback
    };
    Encoder_SetCallbacks(&callbacks);
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
    SetCallbacks();

    expect_function_call(__wrap_driverPEC11_Init);
    Encoder_Init();

    struct encoder_callbacks_t callbacks;
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

static void test_Encoder_Update_NoInput(void **state)
{
    SetCallbacks();

    will_return(__wrap_driverPEC11_PopRightRotation, false);
    will_return(__wrap_driverPEC11_PopLeftRotation, false);
    will_return(__wrap_driverPEC11_PopBriefPush, false);
    will_return(__wrap_driverPEC11_PopExtendedPush, false);

    Encoder_Update();
}

static void test_Encoder_Update(void **state)
{
    SetCallbacks();

    will_return(__wrap_driverPEC11_PopRightRotation, true);
    expect_function_call(FakeRightCallback);
    Encoder_Update();

    will_return_always(__wrap_driverPEC11_PopRightRotation, false);
    will_return(__wrap_driverPEC11_PopLeftRotation, true);
    expect_function_call(FakeLeftCallback);
    Encoder_Update();

    will_return_always(__wrap_driverPEC11_PopLeftRotation, false);
    will_return(__wrap_driverPEC11_PopBriefPush, true);
    expect_function_call(FakeBriefPushCallback);
    Encoder_Update();

    will_return_always(__wrap_driverPEC11_PopBriefPush, false);
    will_return(__wrap_driverPEC11_PopExtendedPush, true);
    expect_function_call(FakeExtendedPushCallback);
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
        cmocka_unit_test_setup(test_Encoder_Update_NoCallbacks, Setup),
        cmocka_unit_test_setup(test_Encoder_Update_NoInput, Setup),
        cmocka_unit_test_setup(test_Encoder_Update, Setup)
    };

    if (argc >= 2)
    {
        cmocka_set_test_filter(argv[1]);
    }

    return cmocka_run_group_tests(tests, NULL, NULL);
}
