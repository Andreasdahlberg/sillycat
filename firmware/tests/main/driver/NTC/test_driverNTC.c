/**
 * @file   test_driverNTC.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2021-04-12 (Last edit)
 * @brief  Test suite for the NTC driver.
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

#include "test_driverNTC.h"
#include "driverNTC.h"

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
    expect_value(__wrap_ADC_InitChannel, index, 6);
    expect_value(__wrap_ADC_InitChannel, index, 7);
    driverNTC_Init();

    return 0;
}

//////////////////////////////////////////////////////////////////////////
//TESTS
//////////////////////////////////////////////////////////////////////////

static void test_driverNTC_Init(void **state)
{
    expect_value(__wrap_ADC_InitChannel, index, 6);
    expect_value(__wrap_ADC_InitChannel, index, 7);
    driverNTC_Init();
}

static void test_driverNTC_GetSensor_InvalidId(void **state)
{
    expect_assert_failure(driverNTC_GetSensor(3));
}

static void test_driverNTC_GetSensor(void **state)
{
    assert_non_null(driverNTC_GetSensor(0));
    assert_non_null(driverNTC_GetSensor(0));
    assert_true(driverNTC_GetSensor(0) != driverNTC_GetSensor(1));
}

static void test_driverNTC_UpdateInvalidValue(void **state)
{
    const uint16_t test_data[] = {0, 119, 939, 1023};
    struct sensor_t *sensor_p = driverNTC_GetSensor(0);

    for (size_t i = 0; i < sizeof(test_data) / sizeof(test_data[0]); ++i)
    {
        will_return(__wrap_Timer_TimeDifference, UINT32_MAX);
        will_return(__wrap_ADC_Convert, test_data[i]);
        will_return(__wrap_Timer_GetMilliseconds, 0);
        sensor_p->Update(sensor_p);

        assert_false(sensor_p->valid);
    }
}

static void test_driverNTC_UpdateSampleFrequency(void **state)
{
    for (size_t i = 0; i < 2; ++i)
    {
        struct sensor_t *sensor_p = driverNTC_GetSensor(0);

        will_return(__wrap_Timer_TimeDifference, 100);
        sensor_p->Update(sensor_p);

        will_return(__wrap_Timer_TimeDifference, 101);
        will_return(__wrap_ADC_Convert, 0);
        will_return(__wrap_Timer_GetMilliseconds, 200);
        sensor_p->Update(sensor_p);
    }
}

static void test_driverNTC_UpdateUninitializedFilter(void **state)
{
    const int16_t temperature = 250;
    struct sensor_t *sensor_p = driverNTC_GetSensor(1);

    will_return(__wrap_Timer_TimeDifference, UINT32_MAX);
    will_return(__wrap_ADC_Convert, 512);
    will_return(__wrap_Timer_GetMilliseconds, 0);
    will_return(__wrap_Filter_IsInitialized, false);
    expect_value(__wrap_Filter_Init, initial_value, temperature);
    will_return(__wrap_Filter_Output, temperature);

    sensor_p->Update(sensor_p);
    assert_true(sensor_p->valid);
    assert_int_equal(sensor_p->value, temperature);
}

static void test_driverNTC_UpdateInitializedFilter(void **state)
{
    const int16_t temperature = 250;
    struct sensor_t *sensor_p = driverNTC_GetSensor(1);

    will_return(__wrap_Timer_TimeDifference, UINT32_MAX);
    will_return(__wrap_ADC_Convert, 512);
    will_return(__wrap_Timer_GetMilliseconds, 0);
    will_return(__wrap_Filter_IsInitialized, true);
    expect_function_call(__wrap_Filter_Process);
    will_return(__wrap_Filter_Output, temperature);

    sensor_p->Update(sensor_p);
    assert_true(sensor_p->valid);
    assert_int_equal(sensor_p->value, temperature);
}

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test(test_driverNTC_Init),
        cmocka_unit_test_setup(test_driverNTC_GetSensor_InvalidId, Setup),
        cmocka_unit_test_setup(test_driverNTC_GetSensor, Setup),
        cmocka_unit_test_setup(test_driverNTC_UpdateInvalidValue, Setup),
        cmocka_unit_test_setup(test_driverNTC_UpdateSampleFrequency, Setup),
        cmocka_unit_test_setup(test_driverNTC_UpdateUninitializedFilter, Setup),
        cmocka_unit_test_setup(test_driverNTC_UpdateInitializedFilter, Setup)
    };

    if (argc >= 2)
    {
        cmocka_set_test_filter(argv[1]);
    }

    return cmocka_run_group_tests(tests, NULL, NULL);
}
