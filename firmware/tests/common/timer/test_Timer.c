/**
 * @file   test_Timer.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2020-10-21 (Last edit)
 * @brief  Test suite for the Timer module.
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

#include "test_Timer.h"
#include "Timer.h"

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

//////////////////////////////////////////////////////////////////////////
//TESTS
//////////////////////////////////////////////////////////////////////////

void test_Timer_Init(void **state)
{
    expect_function_call(__wrap_driverTimer_Init);
    Timer_Init();
}

void test_Timer_Reset(void **state)
{
    expect_function_call(__wrap_driverTimer_Reset);
    Timer_Reset();
}

void test_Timer_GetMilliseconds(void **state)
{
    will_return(__wrap_driverTimer_GetMilliseconds, 0);
    will_return(__wrap_driverTimer_GetMilliseconds, 100);

    assert_int_equal(Timer_GetMilliseconds(), 0);
    assert_int_equal(Timer_GetMilliseconds(), 100);
}

void test_Timer_TimeDifference_zero(void **state)
{
    will_return(__wrap_driverTimer_GetMilliseconds, 0);
    will_return(__wrap_driverTimer_GetMilliseconds, 500);

    assert_int_equal(Timer_TimeDifference(0), 0);
    assert_int_equal(Timer_TimeDifference(500), 0);
}

void test_Timer_TimeDifference_nonzero(void **state)
{
    will_return(__wrap_driverTimer_GetMilliseconds, 100);
    will_return(__wrap_driverTimer_GetMilliseconds, 500);

    assert_int_equal(Timer_TimeDifference(0), 100);
    assert_int_equal(Timer_TimeDifference(100), 400);
}

void test_Timer_TimeDifference_wraparound(void **state)
{
    uint32_t timestamp = (0xFFFFFFFF - 100);

    will_return(__wrap_driverTimer_GetMilliseconds, 0xFFFFFFFF);
    will_return(__wrap_driverTimer_GetMilliseconds, 100);

    assert_int_equal(Timer_TimeDifference(timestamp), 100);
    assert_int_equal(Timer_TimeDifference(timestamp), 200);
}

void test_Timer_GetSeconds_exact(void **state)
{
    will_return(__wrap_driverTimer_GetMilliseconds, 0);
    will_return(__wrap_driverTimer_GetMilliseconds, 2000);

    assert_int_equal(Timer_GetSeconds(), 0);
    assert_int_equal(Timer_GetSeconds(), 2);
}

void test_Timer_GetSeconds_ceil(void **state)
{
    will_return(__wrap_driverTimer_GetMilliseconds, 0);
    will_return(__wrap_driverTimer_GetMilliseconds, 500);
    will_return(__wrap_driverTimer_GetMilliseconds, 501);

    assert_int_equal(Timer_GetSeconds(), 0);
    assert_int_equal(Timer_GetSeconds(), 1);
    assert_int_equal(Timer_GetSeconds(), 1);
}

void test_Timer_GetSeconds_floor(void **state)
{
    will_return(__wrap_driverTimer_GetMilliseconds, 0);
    will_return(__wrap_driverTimer_GetMilliseconds, 499);
    will_return(__wrap_driverTimer_GetMilliseconds, 1200);

    assert_int_equal(Timer_GetSeconds(), 0);
    assert_int_equal(Timer_GetSeconds(), 0);
    assert_int_equal(Timer_GetSeconds(), 1);
}

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test(test_Timer_Init),
        cmocka_unit_test(test_Timer_Reset),
        cmocka_unit_test(test_Timer_GetMilliseconds),
        cmocka_unit_test(test_Timer_TimeDifference_zero),
        cmocka_unit_test(test_Timer_TimeDifference_nonzero),
        cmocka_unit_test(test_Timer_TimeDifference_wraparound),
        cmocka_unit_test(test_Timer_GetSeconds_exact),
        cmocka_unit_test(test_Timer_GetSeconds_ceil),
        cmocka_unit_test(test_Timer_GetSeconds_floor),
    };

    if (argc >= 2)
    {
        cmocka_set_test_filter(argv[1]);
    }

    return cmocka_run_group_tests(tests, NULL, NULL);
}
