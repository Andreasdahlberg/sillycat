/**
 * @file   test_Bit.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2020-01-27 (Last edit)
 * @brief  Test suite for the Bit utility.
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

#include "Bit.h"

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
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TESTS
//////////////////////////////////////////////////////////////////////////

static void test_Bit_Set_InvalidArguments(void **state)
{
    uint8_t byte;
    expect_assert_failure(Bit_Set(8, true, &byte));
    expect_assert_failure(Bit_Set(__UINT8_MAX__, true, &byte));
    expect_assert_failure(Bit_Set(0, false, NULL));
    expect_assert_failure(Bit_Set(7, false, NULL));
    expect_assert_failure(Bit_Set(8, false, NULL));
}

static void test_Bit_Set(void **state)
{
    uint8_t byte = 0x00;

    Bit_Set(0, true, &byte);
    assert_int_equal(byte, 0x01);

    Bit_Set(7, true, &byte);
    assert_int_equal(byte, 0x81);

    Bit_Set(7, true, &byte);
    assert_int_equal(byte, 0x81);

    Bit_Set(7, false, &byte);
    assert_int_equal(byte, 0x01);

    Bit_Set(0, false, &byte);
    assert_int_equal(byte, 0x00);

    Bit_Set(0, false, &byte);
    assert_int_equal(byte, 0x00);
}

static void test_Bit_Get_InvalidArguments(void **state)
{
    uint8_t byte;
    expect_assert_failure(Bit_Get(8, &byte));
    expect_assert_failure(Bit_Get(__UINT8_MAX__, &byte));
    expect_assert_failure(Bit_Get(0, NULL));
    expect_assert_failure(Bit_Get(7, NULL));
    expect_assert_failure(Bit_Get(8, NULL));
}

static void test_Bit_Get(void **state)
{
    uint8_t byte = 0x81;

    assert_true(Bit_Get(0, &byte));
    assert_true(Bit_Get(7, &byte));

    assert_false(Bit_Get(1, &byte));
    assert_false(Bit_Get(6, &byte));
}

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test(test_Bit_Set_InvalidArguments),
        cmocka_unit_test(test_Bit_Set),
        cmocka_unit_test(test_Bit_Get_InvalidArguments),
        cmocka_unit_test(test_Bit_Get),
    };

    if (argc >= 2)
    {
        cmocka_set_test_filter(argv[1]);
    }

    return cmocka_run_group_tests(tests, NULL, NULL);
}
