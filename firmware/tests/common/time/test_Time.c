/**
 * @file   test_Time.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2020-10-21 (Last edit)
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
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
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

    const uint8_t non_leap_years[] = {1, 2, 3, 5, 6, 7, 9, 100, 255};
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

static void test_Time_AddSeconds_NULL(void **state)
{
    expect_assert_failure(Time_AddSeconds(NULL, 1));
}

static void test_Time_AddSeconds(void **state)
{
    struct test_data_t
    {
        uint8_t seconds;
        uint8_t expected_second;
        uint8_t expected_minute;
    };

    struct time_t time = {.month = 1, .date = 1};
    struct test_data_t data[] =
    {
        {.seconds = 0, .expected_second = 0, .expected_minute = 0},
        {.seconds = 1, .expected_second = 1, .expected_minute = 0},
        {.seconds = 1, .expected_second = 2, .expected_minute = 0},
        {.seconds = 57, .expected_second = 59, .expected_minute = 0},
        {.seconds = 1, .expected_second = 0, .expected_minute = 1},
        {.seconds = 121, .expected_second = 1, .expected_minute = 3},
    };

    for (size_t i = 0; i < ElementsIn(data); ++i)
    {
        Time_AddSeconds(&time, data[i].seconds);
        assert_int_equal(time.second, data[i].expected_second);
        assert_int_equal(time.minute, data[i].expected_minute);
    }
}

static void test_Time_AddMinutes_NULL(void **state)
{
    expect_assert_failure(Time_AddMinutes(NULL, 1));
}

static void test_Time_AddMinutes(void **state)
{
    struct test_data_t
    {
        uint8_t minutes;
        uint8_t expected_minute;
        uint8_t expected_hour;
    };

    struct time_t time = {.month = 1, .date = 1};
    struct test_data_t data[] =
    {
        {.minutes = 0, .expected_minute = 0, .expected_hour = 0},
        {.minutes = 1, .expected_minute = 1, .expected_hour = 0},
        {.minutes = 1, .expected_minute = 2, .expected_hour = 0},
        {.minutes = 57, .expected_minute = 59, .expected_hour = 0},
        {.minutes = 1, .expected_minute = 0, .expected_hour = 1},
        {.minutes = 121, .expected_minute = 1, .expected_hour = 3},
    };

    for (size_t i = 0; i < ElementsIn(data); ++i)
    {
        Time_AddMinutes(&time, data[i].minutes);
        assert_int_equal(time.minute, data[i].expected_minute);
        assert_int_equal(time.hour, data[i].expected_hour);
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

static void test_Time_AddDays_NULL(void **state)
{
    expect_assert_failure(Time_AddDays(NULL, 1));
}

static void test_Time_AddDays(void **state)
{
    struct test_data_t
    {
        uint8_t days;
        uint8_t expected_date;
        uint8_t expected_month;
    };

    struct time_t time = {.month = 1, .date = 1};
    struct test_data_t data[] =
    {
        {.days = 0, .expected_date = 1, .expected_month = 1},
        {.days = 1, .expected_date = 2, .expected_month = 1},
        {.days = 1, .expected_date = 3, .expected_month = 1},
        {.days = 27, .expected_date = 30, .expected_month = 1},
        {.days = 1, .expected_date = 31, .expected_month = 1},
        {.days = 1, .expected_date = 1, .expected_month = 2},
    };

    for (size_t i = 0; i < ElementsIn(data); ++i)
    {
        Time_AddDays(&time, data[i].days);
        assert_int_equal(time.date, data[i].expected_date);
        assert_int_equal(time.month, data[i].expected_month);
    }
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

static void test_Time_ConvertToTimestamp_NULL(void **state)
{
    expect_assert_failure(Time_ConvertToTimestamp(NULL));
}

static void test_Time_ConvertToTimestamp(void **state)
{
    struct test_data_t
    {
        struct time_t time;
        uint32_t expected_timestamp;
    };

    const struct test_data_t data[] =
    {
        {
            .time = {
                .year = 0,
                .month = JANUARY,
                .date = 1,
                .hour = 0,
                .minute = 0,
                .second = 0
            },
            .expected_timestamp = 0
        },
        {
            .time = {
                .year = 0,
                .month = FEBRUARY,
                .date = 1,
                .hour = 0,
                .minute = 0,
                .second = 0
            },
            .expected_timestamp = 2678400
        },
        {
            .time = {
                .year = 0,
                .month = MARCH,
                .date = 1,
                .hour = 0,
                .minute = 0,
                .second = 0
            },
            .expected_timestamp = 5184000
        },
        {
            .time = {
                .year = 0,
                .month = AUGUST,
                .date = 1,
                .hour = 0,
                .minute = 0,
                .second = 0
            },
            .expected_timestamp = 18403200
        },
        {
            .time = {
                .year = 0,
                .month = JANUARY,
                .date = 1,
                .hour = 1,
                .minute = 2,
                .second = 3
            },
            .expected_timestamp = 3723
        },
        {
            .time = {
                .year = 1,
                .month = JANUARY,
                .date = 1,
                .hour = 0,
                .minute = 0,
                .second = 0
            },
            .expected_timestamp = 31622400
        },
        {
            .time = {
                .year = 4,
                .month = JANUARY,
                .date = 1,
                .hour = 0,
                .minute = 0,
                .second = 0
            },
            .expected_timestamp = 126230400
        },
    };

    for (size_t i = 0; i < ElementsIn(data); ++i)
    {
        assert_int_equal(Time_ConvertToTimestamp(&data[i].time),
                         data[i].expected_timestamp
                        );
    }
}

static void test_Time_ConvertFromTimestamp(void **state)
{
    struct test_data_t
    {
        struct time_t expected_time;
        uint32_t timestamp;
    };

    const struct test_data_t data[] =
    {
        {
            .expected_time = {
                .year = 0,
                .month = JANUARY,
                .date = 1,
                .hour = 0,
                .minute = 0,
                .second = 0
            },
            .timestamp = 0
        },
        {
            .expected_time = {
                .year = 0,
                .month = FEBRUARY,
                .date = 1,
                .hour = 0,
                .minute = 0,
                .second = 0
            },
            .timestamp = 2678400
        },
        {
            .expected_time = {
                .year = 0,
                .month = MARCH,
                .date = 1,
                .hour = 0,
                .minute = 0,
                .second = 0
            },
            .timestamp = 5184000
        },
        {
            .expected_time = {
                .year = 0,
                .month = AUGUST,
                .date = 1,
                .hour = 0,
                .minute = 0,
                .second = 0
            },
            .timestamp = 18403200
        },
        {
            .expected_time = {
                .year = 0,
                .month = JANUARY,
                .date = 1,
                .hour = 1,
                .minute = 2,
                .second = 3
            },
            .timestamp = 3723
        },
        {
            .expected_time = {
                .year = 1,
                .month = JANUARY,
                .date = 1,
                .hour = 0,
                .minute = 0,
                .second = 0
            },
            .timestamp = 31622400
        },
        {
            .expected_time = {
                .year = 4,
                .month = JANUARY,
                .date = 1,
                .hour = 0,
                .minute = 0,
                .second = 0
            },
            .timestamp = 126230400
        },
    };

    for (size_t i = 0; i < ElementsIn(data); ++i)
    {
        struct time_t time = Time_ConvertFromTimestamp(data[i].timestamp);
        assert_int_equal(time.year, data[i].expected_time.year);
        assert_int_equal(time.month, data[i].expected_time.month);
        assert_int_equal(time.date, data[i].expected_time.date);
        assert_int_equal(time.hour, data[i].expected_time.hour);
        assert_int_equal(time.minute, data[i].expected_time.minute);
        assert_int_equal(time.second, data[i].expected_time.second);
    }
}

static void test_Time_ConvertFromAndToTimestamp(void **state)
{
    uint32_t timestamps[] = {0, UINT32_MAX / 2, UINT32_MAX};

    for (size_t i = 0; i < ElementsIn(timestamps); ++i)
    {
        struct time_t time = Time_ConvertFromTimestamp(timestamps[i]);
        assert_int_equal(timestamps[i], Time_ConvertToTimestamp(&time));
    }
}

struct div_t Divide(int32_t numerator, int32_t denominator)
{
    struct div_t result;

    result.quotient = numerator / denominator;
    result.remainder = numerator % denominator;

    return result;
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
        cmocka_unit_test(test_Time_AddSeconds_NULL),
        cmocka_unit_test(test_Time_AddSeconds),
        cmocka_unit_test(test_Time_AddMinutes_NULL),
        cmocka_unit_test(test_Time_AddMinutes),
        cmocka_unit_test(test_Time_AddHours_NULL),
        cmocka_unit_test(test_Time_AddHours),
        cmocka_unit_test(test_Time_AddDays_NULL),
        cmocka_unit_test(test_Time_AddDays),
        cmocka_unit_test(test_Time_IsDaylightSavingActive_NULL),
        cmocka_unit_test(test_Time_IsDaylightSavingActive),
        cmocka_unit_test(test_Time_ConvertToTimestamp_NULL),
        cmocka_unit_test(test_Time_ConvertToTimestamp),
        cmocka_unit_test(test_Time_ConvertFromTimestamp),
        cmocka_unit_test(test_Time_ConvertFromAndToTimestamp)
    };

    if (argc >= 2)
    {
        cmocka_set_test_filter(argv[1]);
    }

    return cmocka_run_group_tests(tests, NULL, NULL);
}
