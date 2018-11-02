/**
 * @file   test_Time.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-11-02 (Last edit)
 * @brief  Test suite for the Time module.
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

#include "common.h"
#include "Time.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TESTS
//////////////////////////////////////////////////////////////////////////

static void test_Time_FormatTimestamp_InvalidArguments(void **state)
{
    struct time_t time;
    char timestamp[11];

    expect_assert_failure(Time_FormatTimestamp(NULL, NULL, 0));
    expect_assert_failure(Time_FormatTimestamp(&time, NULL, 0));
    expect_assert_failure(
        Time_FormatTimestamp(NULL, timestamp, sizeof(timestamp))
    );
    expect_assert_failure(
        Time_FormatTimestamp(&time, timestamp, 0)
    );
}

static void test_Time_FormatTimestamp_Truncated(void **state)
{
    struct time_t time;
    char timestamp[20];

    time.year = 18;
    time.month = 11;
    time.date = 11;
    time.hour = 21;
    time.minute= 45;
    time.second = 30;

    /**
     * Set these positions to 0xFF so that the NULL termination can be checked.
     */
    timestamp[4] = 0xFF;
    timestamp[5] = 0xFF;

    Time_FormatTimestamp(&time, timestamp, 5);
    assert_string_equal(timestamp, "2018");
    assert_int_equal(timestamp[4], '\0');
    assert_int_equal((uint8_t)timestamp[5], 0xFF);
}

static void test_Time_FormatTimestamp(void **state)
{
    struct time_t time;
    char timestamp[20];

    time.year = 18;
    time.month = 11;
    time.date = 11;
    time.hour = 21;
    time.minute= 45;
    time.second = 30;
    Time_FormatTimestamp(&time, timestamp, sizeof(timestamp));
    assert_string_equal(timestamp, "2018-11-11 21:45:30");

    time.year = 0;
    time.month = 1;
    time.date = 2;
    time.hour = 3;
    time.minute= 4;
    time.second = 5;
    Time_FormatTimestamp(&time, timestamp, sizeof(timestamp));
    assert_string_equal(timestamp, "2000-01-02 03:04:05");
}

static void test_Time_IsLeapYear_NULL(void **state)
{
    expect_assert_failure(Time_IsLeapYear(NULL));
}

static void test_Time_IsLeapYear(void **state)
{
    const uint8_t leap_years[] = {0, 4, 8, 252};
    for (size_t i = 0; i < ElementsIn(leap_years); ++i)
    {
        const struct time_t time = {.year = leap_years[i]};
        assert_true(Time_IsLeapYear(&time));
    }

    const uint8_t non_leap_years[] = {1, 2, 3, 5, 6, 7, 9, 255};
    for (size_t i = 0; i < ElementsIn(non_leap_years); ++i)
    {
        const struct time_t time = {.year = non_leap_years[i]};
        assert_false(Time_IsLeapYear(&time));
    }
}

static void test_Time_GetDaysInMonth_NULL(void **state)
{
    expect_assert_failure(Time_GetDaysInMonth(NULL));
}

static void test_Time_GetDaysInMonth_InvalidMonth(void **state)
{
    struct time_t time = {0};

    time.month = 0;
    expect_assert_failure(Time_GetDaysInMonth(&time));

    time.month = 13;
    expect_assert_failure(Time_GetDaysInMonth(&time));
}

static void test_Time_GetDaysInMonth(void **state)
{
    /* No leap year. */
    struct time_t time = {.year = 1};
    uint8_t expected_days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    for (size_t i = 0; i < ElementsIn(expected_days); ++i)
    {
        time.month = (uint8_t)(i + 1);
        assert_int_equal(Time_GetDaysInMonth(&time), expected_days[i]);
    }

    /* Leap year. */
    time.year = 0;
    expected_days[1] = 29;

    for (size_t i = 0; i < ElementsIn(expected_days); ++i)
    {
        time.month = (uint8_t)(i + 1);
        assert_int_equal(Time_GetDaysInMonth(&time), expected_days[i]);
    }
}

static void test_Time_AddHours_NULL(void **state)
{
    expect_assert_failure(Time_AddHours(NULL, 1));
}

static void test_Time_AddHours(void **state)
{
    struct test_data_t
    {
        uint8_t hours;
        uint8_t expected_hour;
        uint8_t expected_date;
    };

    struct time_t time = {.month = 1, .date = 1};
    struct test_data_t data[] =
    {
        {.hours = 0, .expected_hour = 0, .expected_date = 1},
        {.hours = 1, .expected_hour = 1, .expected_date = 1},
        {.hours = 1, .expected_hour = 2, .expected_date = 1},
        {.hours = 20, .expected_hour = 22, .expected_date = 1},
        {.hours = 1, .expected_hour = 23, .expected_date = 1},
        {.hours = 1, .expected_hour = 0, .expected_date = 2},
        {.hours = 48, .expected_hour = 0, .expected_date = 4},
    };

    for (size_t i = 0; i < ElementsIn(data); ++i)
    {
        Time_AddHours(&time, data[i].hours);
        assert_int_equal(time.hour, data[i].expected_hour);
        assert_int_equal(time.date, data[i].expected_date);
    }
}

static void test_Time_AddYears_NULL(void **state)
{
    expect_assert_failure(Time_AddYears(NULL, 1));
}

static void test_Time_AddYears(void **state)
{
    struct time_t time = {0};

    Time_AddYears(&time, 1);
    assert_int_equal(time.year, 1);
    Time_AddYears(&time, 98);
    assert_int_equal(time.year, 99);
    Time_AddYears(&time, 1);
    assert_int_equal(time.year, 99);
}

static void test_Time_IsDaylightSavingActive_NULL(void **state)
{
    expect_assert_failure(Time_IsDaylightSavingActive(NULL));
}

static void test_Time_IsDaylightSavingActive(void **state)
{
    struct time_t time = {.year = 18, .date = 1};
    time_month_t months_without_dst[] = {11, 12, 1, 2};
    time_month_t months_with_dst[] = {4, 5, 6, 7, 8, 9};

    for (size_t i = 0; i < ElementsIn(months_with_dst); ++i)
    {
        time.month = months_with_dst[i];
        assert_true(Time_IsDaylightSavingActive(&time));
    }

    for (size_t i = 0; i < ElementsIn(months_without_dst); ++i)
    {
        time.month = months_without_dst[i];
        assert_false(Time_IsDaylightSavingActive(&time));
    }

    time.month = OCTOBER;
    time.date = 1;
    assert_true(Time_IsDaylightSavingActive(&time));
    time.date = 27;
    assert_true(Time_IsDaylightSavingActive(&time));
    time.date = 28;
    assert_false(Time_IsDaylightSavingActive(&time));

    time.month = MARCH;
    time.date = 1;
    assert_false(Time_IsDaylightSavingActive(&time));
    time.date = 24;
    assert_false(Time_IsDaylightSavingActive(&time));
    time.date = 25;
    assert_true(Time_IsDaylightSavingActive(&time));
}

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test(test_Time_FormatTimestamp_InvalidArguments),
        cmocka_unit_test(test_Time_FormatTimestamp_Truncated),
        cmocka_unit_test(test_Time_FormatTimestamp),
        cmocka_unit_test(test_Time_IsLeapYear_NULL),
        cmocka_unit_test(test_Time_IsLeapYear),
        cmocka_unit_test(test_Time_GetDaysInMonth_NULL),
        cmocka_unit_test(test_Time_GetDaysInMonth_InvalidMonth),
        cmocka_unit_test(test_Time_GetDaysInMonth),
        cmocka_unit_test(test_Time_AddHours_NULL),
        cmocka_unit_test(test_Time_AddHours),
        cmocka_unit_test(test_Time_AddYears_NULL),
        cmocka_unit_test(test_Time_AddYears),
        cmocka_unit_test(test_Time_IsDaylightSavingActive_NULL),
        cmocka_unit_test(test_Time_IsDaylightSavingActive)
    };

    if (argc >= 2)
    {
        cmocka_set_test_filter(argv[1]);
    }

    return cmocka_run_group_tests(tests, NULL, NULL);
}
