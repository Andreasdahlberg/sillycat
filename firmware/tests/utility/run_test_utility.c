/**
 * @file   tests/utility/run_test_utility.c
 * @date   2016-05-27 (Generated)
 * @brief  Application for running all detected tests.
 *
 * This is a auto-generated file, please do not edit manually!
 */

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "test_CRC.h"

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

int main(void) {
    const struct CMUnitTest tests[] = {
       cmocka_unit_test(test_CRC_Calculate16_ZeroLength),
       cmocka_unit_test(test_CRC_Calculate16_NullPointerZeroLength),
       cmocka_unit_test(test_CRC_Calculate16_NullPointer),
       cmocka_unit_test(test_CRC_Calculate16_SingleBytes),
       cmocka_unit_test(test_CRC_Calculate16_SeveralBytes)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}