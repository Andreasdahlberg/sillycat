/**
 * @file   test_driverMCUTemperature.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2020-10-21 (Last edit)
 * @brief  Test suite for the MCU temperature driver.
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

#include "test_driverMCUTemperature.h"
#include "driverMCUTemperature.h"

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
    expect_any(__wrap_ADC_InitChannel, index);
    driverMCUTemperature_Init();

    return 0;
}

//////////////////////////////////////////////////////////////////////////
//TESTS
//////////////////////////////////////////////////////////////////////////

static void test_driverMCUTemperature_Init(void **state)
{
    const uint8_t expected_channel_index = 8;

    expect_value(__wrap_ADC_InitChannel, index, expected_channel_index);
    driverMCUTemperature_Init();
}

static void test_driverMCUTemperature_GetSensor(void **state)
{
    assert_non_null(driverMCUTemperature_GetSensor());
}

static void test_driverMCUTemperature_UpdateSampleFrequency(void **state)
{
    for (size_t i = 0; i < 2; ++i)
    {
        struct sensor_t *sensor_p = driverMCUTemperature_GetSensor();

        will_return(__wrap_Timer_TimeDifference, 100);
        sensor_p->Update(sensor_p);

        will_return(__wrap_Timer_TimeDifference, 101);
        will_return(__wrap_ADC_Convert, 0);
        will_return(__wrap_Timer_GetMilliseconds, 200);
        sensor_p->Update(sensor_p);
    }
}

static void test_driverMCUTemperature_Update(void **state)
{
    struct sensor_t *sensor_p = driverMCUTemperature_GetSensor();

    will_return(__wrap_Timer_TimeDifference, UINT32_MAX);
    will_return(__wrap_ADC_Convert, 100);
    will_return(__wrap_Timer_GetMilliseconds, 200);
    sensor_p->Update(sensor_p);

    assert_int_equal(sensor_p->value, 33);
    assert_true(sensor_p->valid);
}

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test(test_driverMCUTemperature_Init),
        cmocka_unit_test_setup(test_driverMCUTemperature_GetSensor, Setup),
        cmocka_unit_test_setup(test_driverMCUTemperature_UpdateSampleFrequency, Setup),
        cmocka_unit_test_setup(test_driverMCUTemperature_Update, Setup),
    };

    if (argc >= 2)
    {
        cmocka_set_test_filter(argv[1]);
    }

    return cmocka_run_group_tests(tests, NULL, NULL);
}
