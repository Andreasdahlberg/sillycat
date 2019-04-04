/**
 * @file   test_ErrorHandler.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2019-04-04 (Last edit)
 * @brief  Test suite for the error handler module.
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
#include <string.h>

#include "ErrorHandler.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define ERROR_LOG_SIZE 80

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

struct error_message_t
{
    uint32_t id;
    uint32_t timestamp;
    uint8_t code;
    uint8_t information;
};

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static void *error_log_p;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static int Setup(void **state)
{
    /**
     * Call the initialization function once to catch the address to the EEPROM
     * location in RAM.
     */
    ErrorHandler_Init();
    assert_non_null(error_log_p);

    /**
     * Call the initialization function again when the EEPROM is cleared to
     * get a clean starting state.
     */
    memset(error_log_p, 0, sizeof(struct error_message_t) * ERROR_LOG_SIZE);
    ErrorHandler_Init();

    return 0;
}

static void dummy_assert_fail_handler(
    const char *file,
    int line_number,
    const char *expression)
{
    function_called();
}

static const struct error_message_t *get_error_message_by_index(uint8_t index)
{
    assert_true(index < ERROR_LOG_SIZE);

    struct error_message_t *error_message_p = (struct error_message_t *)error_log_p + index;

    if (error_message_p->id > 0)
    {
        return error_message_p;
    }

    return NULL;
}

static const struct error_message_t *get_error_message_by_id(uint8_t id)
{
    assert_int_not_equal(id, 0);

    for (size_t i = 0; i < ERROR_LOG_SIZE; ++i)
    {
        const struct error_message_t *error_message_p;
        error_message_p = get_error_message_by_index(i);

        if (error_message_p != NULL && error_message_p->id == id)
        {
            return error_message_p;
        }
    }
    return NULL;
}

static void log_errors(size_t number_of_errors, uint8_t code_offset, uint8_t information_offset)
{
    for (size_t i = 0; i < number_of_errors; ++i)
    {
        will_return(__wrap_RTC_GetTimeStamp, true);
        ErrorHandler_LogError(code_offset + i, information_offset + i);
    }
}

static void check_error_message(uint8_t id, uint8_t code, uint8_t information)
{
    const struct error_message_t *error_message_p;
    error_message_p = get_error_message_by_id(id);

    assert_non_null(error_message_p);
    assert_int_equal(error_message_p->code, code);
    assert_int_equal(error_message_p->information, information);
}

//////////////////////////////////////////////////////////////////////////
//TESTS
//////////////////////////////////////////////////////////////////////////

static void test_ErrorHandler_Init(void **state)
{
    ErrorHandler_Init();
    assert_non_null(error_log_p);
}

static void test_ErrorHandler_Init_LogFull(void **state)
{
    /**
     * The log is already cleared and initialized once in the Setup
     * function so it's ok to start log errors.
     */
    const uint8_t error_code_offset = 10;
    const uint8_t error_information_offset = 100;
    const size_t number_of_errors = 80;
    log_errors(number_of_errors, error_code_offset, error_information_offset);

    ErrorHandler_Init();
    assert_non_null(error_log_p);

    check_error_message(1, error_code_offset, error_information_offset);
    check_error_message(number_of_errors,
                        error_code_offset + number_of_errors - 1,
                        error_information_offset + number_of_errors - 1);
}

static void test_ErrorHandler_Init_LogWrapAround(void **state)
{
    /**
     * The log is already cleared and initialized once in the Setup
     * function so it's ok to start log errors.
     */
    const uint8_t error_code_offset = 20;
    const uint8_t error_information_offset = 110;
    const size_t number_of_errors = 81;
    log_errors(number_of_errors, error_code_offset, error_information_offset);

    ErrorHandler_Init();
    assert_non_null(error_log_p);

    check_error_message(number_of_errors - 1,
                        error_code_offset + number_of_errors - 2,
                        error_information_offset + number_of_errors - 2);
    check_error_message(number_of_errors,
                        error_code_offset + number_of_errors - 1,
                        error_information_offset + number_of_errors - 1);
}

static void test_ErrorHandler_AssertFailAndHandler(void **state)
{
    const char file[] = "foo.c";
    const int line_number = 2;
    const char expression[] = "1 == 2";

    expect_function_call(__wrap_wdt_disable);
    ErrorHandler_AssertFail(file, line_number, expression);

    ErrorHandler_SetAssertFailHandler(dummy_assert_fail_handler);
    expect_function_call(dummy_assert_fail_handler);
    expect_function_call(__wrap_wdt_disable);
    ErrorHandler_AssertFail(file, line_number, expression);

    ErrorHandler_SetAssertFailHandler(NULL);
    expect_function_call(__wrap_wdt_disable);
    ErrorHandler_AssertFail(file, line_number, expression);
}

static void test_ErrorHandler_LogError(void **state)
{
    const uint8_t error_code_offset = 30;
    const uint8_t error_information_offset = 120;
    const size_t number_of_errors = 3;

    log_errors(number_of_errors, error_code_offset, error_information_offset);

    for (size_t i = 0; i < number_of_errors; ++i)
    {
        check_error_message(i + 1,
                            error_code_offset + i,
                            error_information_offset + i);
    }
}

static void test_ErrorHandler_LogError_LogWrapAround(void **state)
{
    const uint8_t error_code_offset = 40;
    const uint8_t error_information_offset = 130;
    const size_t number_of_errors = 81;

    log_errors(number_of_errors, error_code_offset, error_information_offset);

    const struct error_message_t *error_message_p;

    /**
     * Make sure that the first entry is overwritten.
     */
    error_message_p = get_error_message_by_id(1);
    assert_null(error_message_p);

    /**
     * Check the entry before the wraparound.
     */
    check_error_message(number_of_errors - 1,
                        error_code_offset + number_of_errors - 2,
                        error_information_offset + number_of_errors - 2);

    /**
     * Check the entry after the wraparound.
     */
    check_error_message(number_of_errors,
                        error_code_offset + number_of_errors - 1,
                        error_information_offset + number_of_errors - 1);
}

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void __wrap_wdt_disable(void)
{
    function_called();
}

void __wrap_eeprom_read_block(void *dst, void *src, size_t size)
{
    /**
     * Store a pointer to the start of the error log so it can be used later
     * on to verify the error log content.
     */
    if (error_log_p == NULL)
    {
        error_log_p = src;
    }

    memcpy(dst, src, size);
}

void __wrap_eeprom_write_block(void *src, void *dst, size_t size)
{
    memcpy(dst, src, size);
}

int main(int argc, char *argv[])
{
    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test(test_ErrorHandler_Init),
        cmocka_unit_test_setup(test_ErrorHandler_Init_LogFull, Setup),
        cmocka_unit_test_setup(test_ErrorHandler_Init_LogWrapAround, Setup),
        cmocka_unit_test_setup(test_ErrorHandler_AssertFailAndHandler, Setup),
        cmocka_unit_test_setup(test_ErrorHandler_LogError, Setup),
        cmocka_unit_test_setup(test_ErrorHandler_LogError_LogWrapAround, Setup)

    };

    if (argc >= 2)
    {
        cmocka_set_test_filter(argv[1]);
    }

    return cmocka_run_group_tests(tests, NULL, NULL);
}
