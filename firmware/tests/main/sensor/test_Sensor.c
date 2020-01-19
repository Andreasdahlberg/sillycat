/**
 * @file   test_Sensor.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2020-01-19 (Last edit)
 * @brief  Test suite for the sensor module.
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

#include "test_Sensor.h"
#include "Sensor.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define MAX_NUMBER_OF_SENSORS 9

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

struct mock_sensor_t
{
    struct sensor_t base;
};

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

struct mock_sensor_t mock_sensors[MAX_NUMBER_OF_SENSORS];

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
    for (size_t i = 0; i < MAX_NUMBER_OF_SENSORS; ++i)
    {
        mock_sensors[i] = (struct mock_sensor_t) {0};
    }

    Sensor_Init();

    return 0;
}

static struct sensor_t *GetMockSensor(size_t index)
{
    return (struct sensor_t *)&mock_sensors[index];
}

static void SetSensorValidFlag(struct sensor_t *sensor_p, bool flag)
{
    sensor_p->valid = flag;
}

static void SetSensorStatisticsValidFlag(struct sensor_t *sensor_p, bool flag)
{
    sensor_p->statistics.valid = flag;
}

static void SetSensorValue(struct sensor_t *sensor_p, int16_t value)
{
    sensor_p->value = value;
}

static void SetSensorMaxValue(struct sensor_t *sensor_p, int16_t value)
{
    sensor_p->statistics.max = value;
}

static void SetSensorMinValue(struct sensor_t *sensor_p, int16_t value)
{
    sensor_p->statistics.min = value;
}

static void BasicMockUpdate(struct sensor_t *super)
{
    function_called();
}

static void MockUpdate(struct sensor_t *super)
{
    struct mock_sensor_t *self = (struct mock_sensor_t *)super;

    self->base.value = mock_type(int16_t);
    self->base.valid = mock_type(bool);
}

//////////////////////////////////////////////////////////////////////////
//TESTS
//////////////////////////////////////////////////////////////////////////

static void test_Sensor_Register_NULL(void **state)
{
    expect_assert_failure(Sensor_Register(NULL));
}

static void test_Sensor_Register_Full(void **state)
{
    size_t index;

    will_return_maybe(__wrap_CRC_16, 0);

    for (index = 0; index < 9; ++index)
    {
        Sensor_Register(GetMockSensor(index));
    }

    expect_assert_failure(Sensor_Register(GetMockSensor(index)));
}

static void test_Sensor_Register(void **state)
{
    will_return_maybe(__wrap_CRC_16, 0);

    Sensor_Register(GetMockSensor(0));

    //TODO: Add test
}

static void test_Sensor_Register_CRCError(void **state)
{
    /**
     * The entire sensor struct is set to zero in 'Setup()', including the
     * saved CRC. A non-zero value is returned to trigger the CRC error.
     */
    will_return(__wrap_CRC_16, 1);

    Sensor_Register(GetMockSensor(0));

    /**
     * The current implementation of the 'driverMCP79510_ReadFromSRAM()' mock
     * returns all zeros. But these values are not used if an CRC occurred.
     * Therefore we can expect min and max to be set to their reset values
     * instead of zero.
     */
    int16_t value;
    assert_false(Sensor_GetMaxValue(GetMockSensor(0), &value));
    assert_int_equal(value, INT16_MIN);
    assert_false(Sensor_GetMinValue(GetMockSensor(0), &value));
    assert_int_equal(value, INT16_MAX);
}

static void test_Sensor_Update(void **state)
{
    will_return_maybe(__wrap_CRC_16, 0);

    /* Nothing should happen here since no sensors are registered. */
    Sensor_Update();

    /**
     * Register a sensor with no update function. Since the sensor values are
     * invalid nothing should happen.
     */
    Sensor_Register(GetMockSensor(0));
    Sensor_Update();

    expect_function_call(BasicMockUpdate);
    GetMockSensor(1)->Update = BasicMockUpdate;
    Sensor_Register(GetMockSensor(1));
    Sensor_Update();

    expect_function_call(BasicMockUpdate);
    expect_function_call(__wrap_driverMCP79510_WriteToSRAM);
    SetSensorValidFlag(GetMockSensor(1), true);
    Sensor_Update();
}

/* TODO: Refactor */
static void test_Sensor_Update_Statistics(void **state)
{
    /**
     * Trigger a CRC error so that the values from SRAM are not used.
     */
    will_return_maybe(__wrap_CRC_16, 1);
    ignore_function_calls(__wrap_driverMCP79510_WriteToSRAM);

    GetMockSensor(0)->Update = MockUpdate;
    Sensor_Register(GetMockSensor(0));

    int16_t expected_value = 16;
    will_return(MockUpdate, expected_value);
    will_return(MockUpdate, true);

    Sensor_Update();

    int16_t value;
    assert_true(Sensor_GetValue(GetMockSensor(0), &value));
    assert_int_equal(value, expected_value);
    assert_true(Sensor_GetMaxValue(GetMockSensor(0), &value));
    assert_int_equal(value, expected_value);
    assert_true(Sensor_GetMinValue(GetMockSensor(0), &value));
    assert_int_equal(value, expected_value);

    expected_value = 20;
    will_return(MockUpdate, expected_value);
    will_return(MockUpdate, true);

    Sensor_Update();

    assert_true(Sensor_GetValue(GetMockSensor(0), &value));
    assert_int_equal(value, expected_value);
    assert_true(Sensor_GetMaxValue(GetMockSensor(0), &value));
    assert_int_equal(value, expected_value);
    assert_true(Sensor_GetMinValue(GetMockSensor(0), &value));
    assert_int_equal(value, 16);

    expected_value = -10;
    will_return(MockUpdate, expected_value);
    will_return(MockUpdate, true);

    Sensor_Update();

    assert_true(Sensor_GetValue(GetMockSensor(0), &value));
    assert_int_equal(value, expected_value);
    assert_true(Sensor_GetMaxValue(GetMockSensor(0), &value));
    assert_int_equal(value, 20);
    assert_true(Sensor_GetMinValue(GetMockSensor(0), &value));
    assert_int_equal(value, expected_value);

    will_return(MockUpdate, expected_value);
    will_return(MockUpdate, false);

    Sensor_Update();

    assert_false(Sensor_GetValue(GetMockSensor(0), &value));
    assert_true(Sensor_GetMaxValue(GetMockSensor(0), &value));
    assert_true(Sensor_GetMinValue(GetMockSensor(0), &value));
}

static void test_Sensor_IsValid(void **state)
{
    SetSensorValidFlag(GetMockSensor(0), false);
    assert_false(Sensor_IsValid(GetMockSensor(0)));

    SetSensorValidFlag(GetMockSensor(0), true);
    assert_true(Sensor_IsValid(GetMockSensor(0)));
}

static void test_Sensor_IsStatisticsValid(void **state)
{
    SetSensorStatisticsValidFlag(GetMockSensor(0), false);
    assert_false(Sensor_IsStatisticsValid(GetMockSensor(0)));

    SetSensorStatisticsValidFlag(GetMockSensor(0), true);
    assert_true(Sensor_IsStatisticsValid(GetMockSensor(0)));
}

static void test_Sensor_GetValue_NULL(void **state)
{
    int16_t value;

    expect_assert_failure(Sensor_GetValue(NULL, NULL));
    expect_assert_failure(Sensor_GetValue(GetMockSensor(0), NULL));
    expect_assert_failure(Sensor_GetValue(NULL, &value));
}

static void test_Sensor_GetValue_ValidFlag(void **state)
{
    int16_t value;

    SetSensorValidFlag(GetMockSensor(0), true);
    assert_true(Sensor_GetValue(GetMockSensor(0), &value));

    SetSensorValidFlag(GetMockSensor(0), false);
    assert_false(Sensor_GetValue(GetMockSensor(0), &value));
}

static void test_Sensor_GetValue_Value(void **state)
{
    int16_t value;
    int16_t expected_value;

    expected_value = 1;
    SetSensorValidFlag(GetMockSensor(0), true);
    SetSensorValue(GetMockSensor(0), expected_value);
    Sensor_GetValue(GetMockSensor(0), &value);
    assert_int_equal(value, expected_value);

    expected_value = 3;
    SetSensorValidFlag(GetMockSensor(0), false);
    SetSensorValue(GetMockSensor(0), expected_value);
    Sensor_GetValue(GetMockSensor(0), &value);
    assert_int_equal(value, expected_value);
}

static void test_Sensor_GetMaxValue_NULL(void **state)
{
    int16_t value;

    expect_assert_failure(Sensor_GetMaxValue(NULL, NULL));
    expect_assert_failure(Sensor_GetMaxValue(GetMockSensor(0), NULL));
    expect_assert_failure(Sensor_GetMaxValue(NULL, &value));
}

static void test_Sensor_GetMaxValue_ValidFlag(void **state)
{
    int16_t value;

    SetSensorStatisticsValidFlag(GetMockSensor(0), true);
    assert_true(Sensor_GetMaxValue(GetMockSensor(0), &value));

    SetSensorStatisticsValidFlag(GetMockSensor(0), false);
    assert_false(Sensor_GetMaxValue(GetMockSensor(0), &value));
}

static void test_Sensor_GetMaxValue_Value(void **state)
{
    int16_t value;
    int16_t expected_value;

    expected_value = 2;
    SetSensorStatisticsValidFlag(GetMockSensor(0), true);
    SetSensorMaxValue(GetMockSensor(0), expected_value);
    Sensor_GetMaxValue(GetMockSensor(0), &value);
    assert_int_equal(value, expected_value);

    expected_value = 5;
    SetSensorStatisticsValidFlag(GetMockSensor(0), false);
    SetSensorMaxValue(GetMockSensor(0), expected_value);
    Sensor_GetMaxValue(GetMockSensor(0), &value);
    assert_int_equal(value, expected_value);
}

static void test_Sensor_GetMinValue_NULL(void **state)
{
    int16_t value;

    expect_assert_failure(Sensor_GetMinValue(NULL, NULL));
    expect_assert_failure(Sensor_GetMinValue(GetMockSensor(0), NULL));
    expect_assert_failure(Sensor_GetMinValue(NULL, &value));
}

static void test_Sensor_GetMinValue_ValidFlag(void **state)
{
    int16_t value;

    SetSensorStatisticsValidFlag(GetMockSensor(0), true);
    assert_true(Sensor_GetMinValue(GetMockSensor(0), &value));

    SetSensorStatisticsValidFlag(GetMockSensor(0), false);
    assert_false(Sensor_GetMinValue(GetMockSensor(0), &value));
}

static void test_Sensor_GetMinValue_Value(void **state)
{
    int16_t value;
    int16_t expected_value;

    expected_value = 3;
    SetSensorStatisticsValidFlag(GetMockSensor(0), true);
    SetSensorMinValue(GetMockSensor(0), expected_value);
    Sensor_GetMinValue(GetMockSensor(0), &value);
    assert_int_equal(value, expected_value);

    expected_value = 1;
    SetSensorStatisticsValidFlag(GetMockSensor(0), false);
    SetSensorMinValue(GetMockSensor(0), expected_value);
    Sensor_GetMinValue(GetMockSensor(0), &value);
    assert_int_equal(value, expected_value);
}

static void test_Sensor_Reset(void **state)
{
    will_return_maybe(__wrap_CRC_16, 0);
    expect_function_call_any(__wrap_driverMCP79510_WriteToSRAM);

    SetSensorValidFlag(GetMockSensor(0), true);
    SetSensorStatisticsValidFlag(GetMockSensor(0), true);
    SetSensorValue(GetMockSensor(0), 1);
    SetSensorMaxValue(GetMockSensor(0), 2);
    SetSensorMinValue(GetMockSensor(0), -2);

    Sensor_Reset(GetMockSensor(0));

    assert_false(Sensor_IsValid(GetMockSensor(0)));
    assert_false(Sensor_IsStatisticsValid(GetMockSensor(0)));

    int16_t value;
    assert_false(Sensor_GetValue(GetMockSensor(0), &value));
    assert_int_equal(value, 0);

    assert_false(Sensor_GetMaxValue(GetMockSensor(0), &value));
    assert_int_equal(value, INT16_MIN);

    assert_false(Sensor_GetMinValue(GetMockSensor(0), &value));
    assert_int_equal(value, INT16_MAX);
}

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

uint16_t __wrap_CRC_16(const void *data, size_t length)
{
    return mock_type(uint16_t);
}

int main(int argc, char *argv[])
{
    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test_setup(test_Sensor_Register_NULL, Setup),
        cmocka_unit_test_setup(test_Sensor_Register_Full, Setup),
        cmocka_unit_test_setup(test_Sensor_Register, Setup),
        cmocka_unit_test_setup(test_Sensor_Register_CRCError, Setup),
        cmocka_unit_test_setup(test_Sensor_Update, Setup),
        cmocka_unit_test_setup(test_Sensor_Update_Statistics, Setup),
        cmocka_unit_test_setup(test_Sensor_IsValid, Setup),
        cmocka_unit_test_setup(test_Sensor_IsStatisticsValid, Setup),
        cmocka_unit_test_setup(test_Sensor_GetValue_NULL, Setup),
        cmocka_unit_test_setup(test_Sensor_GetValue_ValidFlag, Setup),
        cmocka_unit_test_setup(test_Sensor_GetValue_Value, Setup),
        cmocka_unit_test_setup(test_Sensor_GetMaxValue_NULL, Setup),
        cmocka_unit_test_setup(test_Sensor_GetMaxValue_ValidFlag, Setup),
        cmocka_unit_test_setup(test_Sensor_GetMaxValue_Value, Setup),
        cmocka_unit_test_setup(test_Sensor_GetMinValue_NULL, Setup),
        cmocka_unit_test_setup(test_Sensor_GetMinValue_ValidFlag, Setup),
        cmocka_unit_test_setup(test_Sensor_GetMinValue_Value, Setup),
        cmocka_unit_test_setup(test_Sensor_Reset, Setup),
    };

    if (argc >= 2)
    {
        cmocka_set_test_filter(argv[1]);
    }

    return cmocka_run_group_tests(tests, NULL, NULL);
}
