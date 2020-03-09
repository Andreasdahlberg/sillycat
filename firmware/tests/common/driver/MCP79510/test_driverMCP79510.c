/**
 * @file   test_driverMCP79510.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2020-03-09 (Last edit)
 * @brief  Test suite for the MCP79510 driver.
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
#include <stdbool.h>

#include "test_driverMCP79510.h"
#include "driverMCP79510.h"
#include "driverMCP79510Registers.h"
#include "common.h"

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

static int Setup(void **state);
static void StubSPIPreCallback(void);
static void StubSPIPostCallback(void);
static void ExpectWriteRegister(uint8_t address);
static void ExpectWriteValueRegister(uint8_t address, uint8_t data);
static void ExpectReadRegister(uint8_t address, uint8_t data);
static void ExpectModifyRegister(uint8_t address);
static void ExpectEnableSquareWave(void);
static void ExpectEnableOscillator(void);
static void ExpectIsOscillatorRunning(void);

//////////////////////////////////////////////////////////////////////////
//INTERUPT SERVICE ROUTINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static int Setup(void **state)
{
    ExpectEnableSquareWave();
    ExpectEnableOscillator();
    ExpectIsOscillatorRunning();

    driverMCP79510_Init(StubSPIPreCallback, StubSPIPostCallback);

    return 0;
}

static void StubSPIPreCallback(void)
{

}

static void StubSPIPostCallback(void)
{

}

static void ExpectWriteRegister(uint8_t address)
{
    expect_value(__wrap_libSPI_WriteByte, data, INST_WRITE);
    expect_value(__wrap_libSPI_WriteByte, data, address);
    expect_any(__wrap_libSPI_WriteByte, data);
}

static void ExpectWriteValueRegister(uint8_t address, uint8_t data)
{
    expect_value(__wrap_libSPI_WriteByte, data, INST_WRITE);
    expect_value(__wrap_libSPI_WriteByte, data, address);
    expect_value(__wrap_libSPI_WriteByte, data, data);
}

static void ExpectReadRegister(uint8_t address, uint8_t data)
{
    expect_value(__wrap_libSPI_WriteByte, data, INST_READ);
    expect_value(__wrap_libSPI_WriteByte, data, address);
    will_return(__wrap_libSPI_ReadByte, data);
}

static void ExpectModifyRegister(uint8_t address)
{
    ExpectReadRegister(address, 0);
    ExpectWriteRegister(address);
}

static void ExpectEnableSquareWave(void)
{
    ExpectReadRegister(REG_TC_CONTROL, 0xFF);
    ExpectWriteRegister(REG_TC_CONTROL);
}

static void ExpectEnableOscillator(void)
{
    ExpectReadRegister(REG_TC_SEC, 0xFF);
    ExpectWriteRegister(REG_TC_SEC);
}

static void ExpectIsOscillatorRunning(void)
{
    ExpectReadRegister(REG_TC_DAY, 0x00);
    ExpectReadRegister(REG_TC_DAY, 0x00);
    ExpectReadRegister(REG_TC_DAY, 0xFF);
}

//////////////////////////////////////////////////////////////////////////
//TESTS
//////////////////////////////////////////////////////////////////////////

static void test_driverMCP79510_Init_InvalidCallbacks(void **state)
{
    expect_assert_failure(driverMCP79510_Init(NULL, NULL));
    expect_assert_failure(driverMCP79510_Init(StubSPIPreCallback, NULL));
    expect_assert_failure(driverMCP79510_Init(NULL, StubSPIPostCallback));
}

static void test_driverMCP79510_Init(void **state)
{
    ExpectEnableSquareWave();
    ExpectEnableOscillator();
    ExpectIsOscillatorRunning();

    driverMCP79510_Init(StubSPIPreCallback, StubSPIPostCallback);
}

static void test_driverMCP79510_GetHundredthSecond(void **state)
{
    const uint8_t expect_values[] = {0, 1, 50, 98, 99};

    for (size_t i = 0; i < ElementsIn(expect_values); ++i)
    {
        ExpectReadRegister(REG_TC_SEC_CENT, __wrap_DecimalToBCD(expect_values[i]));

        uint8_t result = 0;
        driverMCP79510_GetHundredthSecond(&result);
        assert_int_equal(result, expect_values[i]);
    }
}

static void test_driverMCP79510_SetHundredthSecond_Invalid(void **state)
{
    assert_false(driverMCP79510_SetHundredthSecond(100));
    assert_false(driverMCP79510_SetHundredthSecond(UINT8_MAX));
}

static void test_driverMCP79510_SetHundredthSecond(void **state)
{
    const uint8_t values[] = {0, 1, 50, 98, 99};

    for (size_t i = 0; i < ElementsIn(values); ++i)
    {
        ExpectWriteValueRegister(REG_TC_SEC_CENT, __wrap_DecimalToBCD(values[i]));
        assert_true(driverMCP79510_SetHundredthSecond(values[i]));
    }
}

static void test_driverMCP79510_SetAlarmHundredthSecond_Invalid(void **state)
{
    assert_false(driverMCP79510_SetAlarmHundredthSecond(100));
    assert_false(driverMCP79510_SetAlarmHundredthSecond(UINT8_MAX));
}

static void test_driverMCP79510_SetAlarmHundredthSecond(void **state)
{
    const uint8_t values[] = {0, 1, 50, 98, 99};

    for (size_t i = 0; i < ElementsIn(values); ++i)
    {
        ExpectWriteValueRegister(REG_ALARM1_SEC_CENT, __wrap_DecimalToBCD(values[i]));
        assert_true(driverMCP79510_SetAlarmHundredthSecond(values[i]));
    }
}

static void test_driverMCP79510_SetHour_Invalid(void **state)
{
    const uint8_t values[] = {24, 25, UINT8_MAX};

    for (size_t i = 0; i < ElementsIn(values); ++i)
    {
        assert_false(driverMCP79510_SetHour(values[i]));
    }
}

static void test_driverMCP79510_SetHour(void **state)
{
    const uint8_t values_24[] = {0, 1, 12, 22, 23};
    for (size_t i = 0; i < ElementsIn(values_24); ++i)
    {
        ExpectReadRegister(REG_TC_HOUR, 0x80);
        ExpectWriteValueRegister(REG_TC_HOUR, 0x80 | __wrap_DecimalToBCD(values_24[i]));
        assert_true(driverMCP79510_SetHour(values_24[i]));
    }

    const uint8_t values_12[] = {0, 1, 6, 10, 11};
    for (size_t i = 0; i < ElementsIn(values_24); ++i)
    {
        ExpectReadRegister(REG_TC_HOUR, 0xE0);
        ExpectWriteValueRegister(REG_TC_HOUR, 0xE0 | __wrap_DecimalToBCD(values_24[i]));
        assert_true(driverMCP79510_SetHour(values_24[i]));
    }
}

static void test_driverMCP79510_SetAlarmHour_InvalidIndex(void **state)
{
    const uint8_t values[] = {2, 3, UINT8_MAX};

    for (size_t i = 0; i < ElementsIn(values); ++i)
    {
        expect_assert_failure(driverMCP79510_SetAlarmHour(0, values[i]));
    }
}

static void test_driverMCP79510_SetAlarmHour_InvalidHour(void **state)
{
    const uint8_t values[] = {24, 25, UINT8_MAX};

    for (size_t i = 0; i < ElementsIn(values); ++i)
    {
        assert_false(driverMCP79510_SetAlarmHour(values[i], 0));
    }
}

static void test_driverMCP79510_SetAlarmHour(void **state)
{
    const uint8_t values_24[] = {0, 1, 12, 22, 23};
    for (size_t i = 0; i < ElementsIn(values_24); ++i)
    {
        ExpectReadRegister(REG_ALARM0_HOUR, 0x80);
        ExpectWriteValueRegister(REG_ALARM0_HOUR, 0x80 | __wrap_DecimalToBCD(values_24[i]));
        assert_true(driverMCP79510_SetAlarmHour(values_24[i], 0));
    }

    const uint8_t values_12[] = {0, 1, 6, 10, 11};
    for (size_t i = 0; i < ElementsIn(values_24); ++i)
    {
        ExpectReadRegister(REG_ALARM1_HOUR, 0xE0);
        ExpectWriteValueRegister(REG_ALARM1_HOUR, 0xE0 | __wrap_DecimalToBCD(values_24[i]));
        assert_true(driverMCP79510_SetAlarmHour(values_24[i], 1));
    }
}

static void test_driverMCP79510_GetHour_12(void **state)
{
    const uint8_t values[] = {1, 2, 11, 12};

    // PM
    for (size_t i = 0; i < ElementsIn(values); ++i)
    {
        ExpectReadRegister(REG_TC_HOUR, (1 << REG_TC_HOUR_MODE_BIT) | __wrap_DecimalToBCD(values[i]));

        uint8_t result;
        driverMCP79510_GetHour(&result);
        assert_int_equal(result, values[i]);
    }

    // AM
    for (size_t i = 0; i < ElementsIn(values); ++i)
    {
        ExpectReadRegister(
            REG_TC_HOUR,
            (1 << REG_TC_HOUR_AMPM_BIT) | (1 << REG_TC_HOUR_MODE_BIT) | __wrap_DecimalToBCD(values[i])
        );

        uint8_t result;
        driverMCP79510_GetHour(&result);
        assert_int_equal(result, values[i]);
    }
}

static void test_driverMCP79510_GetHour_24(void **state)
{
    const uint8_t values[] = {0, 1, 11, 12, 13, 22, 23};

    for (size_t i = 0; i < ElementsIn(values); ++i)
    {
        ExpectReadRegister(REG_TC_HOUR, __wrap_DecimalToBCD(values[i]));

        uint8_t result;
        driverMCP79510_GetHour(&result);
        assert_int_equal(result, values[i]);
    }
}

static void test_driverMCP79510_Is24HourMode(void **state)
{
    ExpectReadRegister(REG_TC_HOUR, 0x00);
    assert_true(driverMCP79510_Is24HourMode());

    ExpectReadRegister(REG_TC_HOUR, ~(1 << REG_TC_HOUR_MODE_BIT));
    assert_true(driverMCP79510_Is24HourMode());

    ExpectReadRegister(REG_TC_HOUR, 0xFF);
    assert_false(driverMCP79510_Is24HourMode());

    ExpectReadRegister(REG_TC_HOUR, (1 << REG_TC_HOUR_MODE_BIT));
    assert_false(driverMCP79510_Is24HourMode());
}

static void test_driverMCP79510_IsLeapYear(void **state)
{
    ExpectReadRegister(REG_TC_MONTH, 0x00);
    assert_false(driverMCP79510_IsLeapYear());

    ExpectReadRegister(REG_TC_MONTH, ~(1 << REG_TC_MONTH_LP_BIT));
    assert_false(driverMCP79510_IsLeapYear());

    ExpectReadRegister(REG_TC_MONTH, 0xFF);
    assert_true(driverMCP79510_IsLeapYear());

    ExpectReadRegister(REG_TC_MONTH, (1 << REG_TC_MONTH_LP_BIT));
    assert_true(driverMCP79510_IsLeapYear());
}

static void test_driverMCP79510_EnableAlarm_InvalidIndex(void **state)
{
    expect_assert_failure(driverMCP79510_EnableAlarm(true, 2));
    expect_assert_failure(driverMCP79510_EnableAlarm(true, 3));
    expect_assert_failure(driverMCP79510_EnableAlarm(true, UINT8_MAX));
}

static void test_driverMCP79510_EnableAlarm(void **state)
{
    ExpectReadRegister(REG_TC_CONTROL, 0x00);
    ExpectWriteValueRegister(REG_TC_CONTROL, (1 << REG_TC_CONTROL_ALM0_BIT));
    driverMCP79510_EnableAlarm(true, 0);

    ExpectReadRegister(REG_TC_CONTROL, 0x00);
    ExpectWriteValueRegister(REG_TC_CONTROL, (1 << (REG_TC_CONTROL_ALM0_BIT + 1)));
    driverMCP79510_EnableAlarm(true, 1);


    ExpectReadRegister(REG_TC_CONTROL, 0xFF);
    ExpectWriteValueRegister(REG_TC_CONTROL, ~(1 << REG_TC_CONTROL_ALM0_BIT));
    driverMCP79510_EnableAlarm(false, 0);

    ExpectReadRegister(REG_TC_CONTROL, 0xFF);
    ExpectWriteValueRegister(REG_TC_CONTROL, ~(1 << (REG_TC_CONTROL_ALM0_BIT + 1)));
    driverMCP79510_EnableAlarm(false, 1);
}

static void test_driverMCP79510_ClearBatterySwitchFlag(void **state)
{
    ExpectReadRegister(REG_TC_DAY, 0xFF);
    ExpectWriteValueRegister(REG_TC_DAY, ~(1 << REG_TC_DAY_VBAT_BIT));
    driverMCP79510_ClearBatterySwitchFlag();

    ExpectReadRegister(REG_TC_DAY, 0x00);
    ExpectWriteValueRegister(REG_TC_DAY, 0x00);
    driverMCP79510_ClearBatterySwitchFlag();
}

static void test_driverMCP79510_ClearAlarmFlag_InvalidIndex(void **state)
{
    expect_assert_failure(driverMCP79510_ClearAlarmFlag(2));
    expect_assert_failure(driverMCP79510_ClearAlarmFlag(3));
    expect_assert_failure(driverMCP79510_ClearAlarmFlag(UINT8_MAX));
}

static void test_driverMCP79510_ClearAlarmFlag(void **state)
{
    ExpectReadRegister(REG_ALARM0_DAY, 0xFF);
    ExpectWriteValueRegister(REG_ALARM0_DAY, ~(1 << REG_ALARM_DAY_ALM0IF_BIT));
    driverMCP79510_ClearAlarmFlag(0);

    ExpectReadRegister(REG_ALARM1_DAY, 0xFF);
    ExpectWriteValueRegister(REG_ALARM1_DAY, ~(1 << REG_ALARM_DAY_ALM0IF_BIT));
    driverMCP79510_ClearAlarmFlag(1);

    ExpectReadRegister(REG_ALARM0_DAY, 0x00);
    ExpectWriteValueRegister(REG_ALARM0_DAY, 0x00);
    driverMCP79510_ClearAlarmFlag(0);

    ExpectReadRegister(REG_ALARM1_DAY, 0x00);
    ExpectWriteValueRegister(REG_ALARM1_DAY, 0x00);
    driverMCP79510_ClearAlarmFlag(1);
}

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test(test_driverMCP79510_Init_InvalidCallbacks),
        cmocka_unit_test(test_driverMCP79510_Init),
        cmocka_unit_test_setup(test_driverMCP79510_GetHundredthSecond, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_SetHundredthSecond_Invalid, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_SetHundredthSecond, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_SetAlarmHundredthSecond_Invalid, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_SetAlarmHundredthSecond, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_SetHour_Invalid, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_SetHour, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_SetAlarmHour_InvalidIndex, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_SetAlarmHour_InvalidHour, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_SetAlarmHour, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_Is24HourMode, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_IsLeapYear, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_EnableAlarm_InvalidIndex, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_EnableAlarm, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_ClearBatterySwitchFlag, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_ClearAlarmFlag_InvalidIndex, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_ClearAlarmFlag, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_GetHour_12, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_GetHour_24, Setup),
    };

    if (argc >= 2)
    {
        cmocka_set_test_filter(argv[1]);
    }

    return cmocka_run_group_tests(tests, NULL, NULL);
}

uint8_t __wrap_BCDToDecimal(uint8_t bcd_data)
{
    uint8_t data;

    data = ((bcd_data & 0xF0) >> 4) * 10;
    data += (bcd_data & 0x0F);

    return data;
}

uint16_t __wrap_DecimalToBCD(uint8_t decimal_data)
{
    uint16_t data = 0;
    uint8_t shift = 0;

    do
    {
        data |= (decimal_data % 10) << shift;
        shift += 4;
        decimal_data = decimal_data / 10;
    }
    while (decimal_data > 0);

    return data;
}
