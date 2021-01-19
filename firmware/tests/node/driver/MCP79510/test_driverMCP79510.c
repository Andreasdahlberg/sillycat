/**
 * @file   test_driverMCP79510.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2021-01-19 (Last edit)
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
#include <stdlib.h>
#include <stdint.h>
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
static void ExpectSetOscillatorTrimming(void);
static void ExpectIsOscillatorRunning(void);
static void Set24HourMode(bool enabled);

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
    ExpectSetOscillatorTrimming();
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

static void ExpectSetOscillatorTrimming(void)
{
    ExpectWriteValueRegister(REG_TC_OSCTRIM, 0);
    ExpectReadRegister(REG_TC_HOUR, 0);
    ExpectWriteRegister(REG_TC_HOUR);
}

static void ExpectIsOscillatorRunning(void)
{
    ExpectReadRegister(REG_TC_DAY, 0x00);
    ExpectReadRegister(REG_TC_DAY, 0x00);
    ExpectReadRegister(REG_TC_DAY, 0xFF);
}

static void Set24HourMode(bool enabled)
{
    const uint8_t register_data = ((!(uint8_t)enabled) << REG_TC_HOUR_MODE_BIT);
    ExpectReadRegister(REG_TC_HOUR, register_data);
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
    ExpectSetOscillatorTrimming();
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

static void test_driverMCP79510_GetSecond(void **state)
{
    const uint8_t expect_values[] = {0, 1, 30, 58, 59};

    for (size_t i = 0; i < ElementsIn(expect_values); ++i)
    {
        const uint8_t oscillator_enabled = (1 << REG_TC_SEC_OSC_BIT);
        ExpectReadRegister(REG_TC_SEC, oscillator_enabled | __wrap_DecimalToBCD(expect_values[i]));

        uint8_t result = 0;
        driverMCP79510_GetSecond(&result);
        assert_int_equal(result, expect_values[i]);
    }
}

static void test_driverMCP79510_SetSecond_Invalid(void **state)
{
    const uint8_t values[] = {60, UINT8_MAX};

    for (size_t i = 0; i < ElementsIn(values); ++i)
    {
        assert_false(driverMCP79510_SetSecond(values[i]));
    }
}

static void test_driverMCP79510_SetSecond(void **state)
{
    const uint8_t values[] = {0, 1, 30, 58, 59};

    for (size_t i = 0; i < ElementsIn(values); ++i)
    {
        const uint8_t value = values[i];

        ExpectReadRegister(REG_TC_SEC, 0x00);
        ExpectWriteValueRegister(REG_TC_SEC, __wrap_DecimalToBCD(value));
        assert_true(driverMCP79510_SetSecond(value));
    }

    /* Check if the oscillator bit is handled correctly. */
    const uint8_t oscillator_enabled = (1 << REG_TC_SEC_OSC_BIT);
    ExpectReadRegister(REG_TC_SEC, oscillator_enabled);
    ExpectWriteValueRegister(REG_TC_SEC, oscillator_enabled | __wrap_DecimalToBCD(values[1]));
    assert_true(driverMCP79510_SetSecond(values[1]));
}

static void test_driverMCP79510_SetAlarmSecond_InvalidIndex(void **state)
{
    expect_assert_failure(driverMCP79510_SetAlarmSeconds(0, 2));
    expect_assert_failure(driverMCP79510_SetAlarmSeconds(UINT8_MAX, UINT8_MAX));
}

static void test_driverMCP79510_SetAlarmSecond_InvalidSecond(void **state)
{
    const uint8_t values[] = {60, UINT8_MAX};
    const uint8_t max_alarm_index = 1;

    for (uint8_t alarm_index = 0; alarm_index <= max_alarm_index; ++alarm_index)
    {
        for (size_t i = 0; i < ElementsIn(values); ++i)
        {
            const uint8_t second = values[i];
            assert_false(driverMCP79510_SetAlarmSeconds(second, alarm_index));
        }
    }
}

static void test_driverMCP79510_SetAlarmSecond(void **state)
{
    const uint8_t values[] = {0, 1, 30, 58, 59};
    const uint8_t max_alarm_index = 1;

    for (uint8_t alarm_index = 0; alarm_index <= max_alarm_index; ++alarm_index)
    {
        for (size_t i = 0; i < ElementsIn(values); ++i)
        {
            const uint8_t register_address = (alarm_index == 0) ? REG_ALARM0_SEC : REG_ALARM1_SEC;
            const uint8_t value = values[i];

            ExpectWriteValueRegister(register_address, __wrap_DecimalToBCD(value));
            assert_true(driverMCP79510_SetAlarmSeconds(value, alarm_index));
        }
    }
}

static void test_driverMCP79510_GetMinute(void **state)
{
    const uint8_t expect_values[] = {0, 1, 30, 58, 59};

    for (size_t i = 0; i < ElementsIn(expect_values); ++i)
    {
        ExpectReadRegister(REG_TC_MIN, __wrap_DecimalToBCD(expect_values[i]));

        uint8_t result = 0;
        driverMCP79510_GetMinute(&result);
        assert_int_equal(result, expect_values[i]);
    }
}

static void test_driverMCP79510_SetMinute_Invalid(void **state)
{
    const uint8_t values[] = {60, UINT8_MAX};

    for (size_t i = 0; i < ElementsIn(values); ++i)
    {
        assert_false(driverMCP79510_SetMinute(values[i]));
    }
}

static void test_driverMCP79510_SetMinute(void **state)
{
    const uint8_t values[] = {0, 1, 30, 58, 59};

    for (size_t i = 0; i < ElementsIn(values); ++i)
    {
        const uint8_t value = values[i];

        ExpectWriteValueRegister(REG_TC_MIN, __wrap_DecimalToBCD(value));
        assert_true(driverMCP79510_SetMinute(value));
    }
}

static void test_driverMCP79510_SetAlarmMinute_InvalidIndex(void **state)
{
    expect_assert_failure(driverMCP79510_SetAlarmMinute(0, 2));
    expect_assert_failure(driverMCP79510_SetAlarmMinute(UINT8_MAX, UINT8_MAX));
}

static void test_driverMCP79510_SetAlarmMinute_InvalidMinute(void **state)
{
    const uint8_t values[] = {60, UINT8_MAX};
    const uint8_t max_alarm_index = 1;

    for (uint8_t alarm_index = 0; alarm_index <= max_alarm_index; ++alarm_index)
    {
        for (size_t i = 0; i < ElementsIn(values); ++i)
        {
            assert_false(driverMCP79510_SetAlarmMinute(values[i], alarm_index));
        }
    }
}

static void test_driverMCP79510_SetAlarmMinute(void **state)
{
    const uint8_t values[] = {0, 1, 30, 58, 59};
    const uint8_t max_alarm_index = 1;

    for (uint8_t alarm_index = 0; alarm_index <= max_alarm_index; ++alarm_index)
    {
        for (size_t i = 0; i < ElementsIn(values); ++i)
        {
            const uint8_t register_address = (alarm_index == 0) ? REG_ALARM0_MIN : REG_ALARM1_MIN;
            const uint8_t value = values[i];

            ExpectWriteValueRegister(register_address, __wrap_DecimalToBCD(value));
            assert_true(driverMCP79510_SetAlarmMinute(value, alarm_index));
        }
    }
}

static void test_driverMCP79510_SetHour_Invalid(void **state)
{
    const uint8_t values_12[] = {0, 13, UINT8_MAX};
    for (size_t i = 0; i < ElementsIn(values_12); ++i)
    {
        Set24HourMode(false);
        assert_false(driverMCP79510_SetHour(values_12[i]));
    }

    const uint8_t values_24[] = {24, UINT8_MAX};
    for (size_t i = 0; i < ElementsIn(values_24); ++i)
    {
        Set24HourMode(true);
        assert_false(driverMCP79510_SetHour(values_24[i]));
    }
}

static void test_driverMCP79510_SetHour(void **state)
{
    const uint8_t values_12[] = {1, 2, 6, 11, 12};
    for (size_t i = 0; i < ElementsIn(values_12); ++i)
    {
        Set24HourMode(false);
        ExpectReadRegister(REG_TC_HOUR, 0xE0);
        ExpectWriteValueRegister(REG_TC_HOUR, 0xE0 | __wrap_DecimalToBCD(values_12[i]));
        assert_true(driverMCP79510_SetHour(values_12[i]));
    }

    const uint8_t values_24[] = {0, 1, 12, 22, 23};
    for (size_t i = 0; i < ElementsIn(values_24); ++i)
    {
        Set24HourMode(true);
        ExpectReadRegister(REG_TC_HOUR, 0x80);
        ExpectWriteValueRegister(REG_TC_HOUR, 0x80 | __wrap_DecimalToBCD(values_24[i]));
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
    const uint8_t max_alarm_index = 1;
    for (uint8_t alarm_index = 0; alarm_index <= max_alarm_index; ++alarm_index)
    {
        const uint8_t values_12[] = {0, 13, UINT8_MAX};
        for (size_t i = 0; i < ElementsIn(values_12); ++i)
        {
            Set24HourMode(false);
            assert_false(driverMCP79510_SetAlarmHour(values_12[i], alarm_index));
        }

        const uint8_t values_24[] = {24, UINT8_MAX};
        for (size_t i = 0; i < ElementsIn(values_24); ++i)
        {
            Set24HourMode(true);
            assert_false(driverMCP79510_SetAlarmHour(values_24[i], alarm_index));
        }
    }
}

static void test_driverMCP79510_SetAlarmHour(void **state)
{
    const uint8_t max_alarm_index = 1;
    for (uint8_t alarm_index = 0; alarm_index <= max_alarm_index; ++alarm_index)
    {
        const uint8_t register_address = (alarm_index == 0) ? REG_ALARM0_HOUR : REG_ALARM1_HOUR;

        const uint8_t values_12[] = {1, 2, 6, 11, 12};
        for (size_t i = 0; i < ElementsIn(values_12); ++i)
        {
            Set24HourMode(false);
            ExpectReadRegister(register_address, 0xE0);
            ExpectWriteValueRegister(register_address, 0xE0 | __wrap_DecimalToBCD(values_12[i]));
            assert_true(driverMCP79510_SetAlarmHour(values_12[i], alarm_index));
        }

        const uint8_t values_24[] = {0, 1, 12, 22, 23};
        for (size_t i = 0; i < ElementsIn(values_24); ++i)
        {
            Set24HourMode(true);
            ExpectReadRegister(register_address, 0x80);
            ExpectWriteValueRegister(register_address, 0x80 | __wrap_DecimalToBCD(values_24[i]));
            assert_true(driverMCP79510_SetAlarmHour(values_24[i], alarm_index));
        }
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

static void test_driverMCP79510_GetDay(void **state)
{
    const uint8_t expect_values[] = {1, 2, 4, 6, 7};

    for (size_t i = 0; i < ElementsIn(expect_values); ++i)
    {
        const uint8_t flags = (1 << REG_TC_DAY_OSCRUN_BIT) |
                              (1 << REG_TC_DAY_PWRFAIL_BIT) |
                              (1 << REG_TC_DAY_VBATEN_BIT);

        ExpectReadRegister(REG_TC_DAY, flags | __wrap_DecimalToBCD(expect_values[i]));

        uint8_t result = 0;
        driverMCP79510_GetDay(&result);
        assert_int_equal(result, expect_values[i]);
    }
}

static void test_driverMCP79510_SetDay_Invalid(void **state)
{
    const uint8_t values[] = {0, 8, UINT8_MAX};

    for (size_t i = 0; i < ElementsIn(values); ++i)
    {
        assert_false(driverMCP79510_SetDay(values[i]));
    }
}

static void test_driverMCP79510_SetDay(void **state)
{
    const uint8_t values[] = {1, 2, 4, 6, 7};

    for (size_t i = 0; i < ElementsIn(values); ++i)
    {
        const uint8_t value = values[i];
        const uint8_t flags = (1 << REG_TC_DAY_OSCRUN_BIT) | (1 << REG_TC_DAY_VBATEN_BIT);

        ExpectReadRegister(REG_TC_DAY, flags | i);

        ExpectWriteValueRegister(REG_TC_DAY, flags | __wrap_DecimalToBCD(value));
        assert_true(driverMCP79510_SetDay(value));
    }
}

static void test_driverMCP79510_GetDate(void **state)
{
    const uint8_t expect_values[] = {1, 2, 15, 30, 31};

    for (size_t i = 0; i < ElementsIn(expect_values); ++i)
    {
        ExpectReadRegister(REG_TC_DATE, __wrap_DecimalToBCD(expect_values[i]));

        uint8_t result = 0;
        driverMCP79510_GetDate(&result);
        assert_int_equal(result, expect_values[i]);
    }
}

static void test_driverMCP79510_SetDate_Invalid(void **state)
{
    const uint8_t values[] = {0, 32, UINT8_MAX};

    for (size_t i = 0; i < ElementsIn(values); ++i)
    {
        assert_false(driverMCP79510_SetDate(values[i]));
    }
}

static void test_driverMCP79510_SetDate(void **state)
{
    const uint8_t values[] = {1, 2, 15, 30, 31};

    for (size_t i = 0; i < ElementsIn(values); ++i)
    {
        const uint8_t value = values[i];

        ExpectWriteValueRegister(REG_TC_DATE, __wrap_DecimalToBCD(value));
        assert_true(driverMCP79510_SetDate(value));
    }
}

static void test_driverMCP79510_SetAlarmDate_InvalidIndex(void **state)
{
    expect_assert_failure(driverMCP79510_SetAlarmDate(0, 2));
    expect_assert_failure(driverMCP79510_SetAlarmDate(UINT8_MAX, UINT8_MAX));
}

static void test_driverMCP79510_SetAlarmDate_InvalidDate(void **state)
{
    const uint8_t values[] = {0, 32, UINT8_MAX};
    const uint8_t max_alarm_index = 1;

    for (uint8_t alarm_index = 0; alarm_index <= max_alarm_index; ++alarm_index)
    {
        for (size_t i = 0; i < ElementsIn(values); ++i)
        {
            assert_false(driverMCP79510_SetAlarmDate(values[i], alarm_index));
        }
    }
}

static void test_driverMCP79510_SetAlarmDate(void **state)
{
    const uint8_t values[] = {1, 2, 15, 30, 31};
    const uint8_t max_alarm_index = 1;

    for (uint8_t alarm_index = 0; alarm_index <= max_alarm_index; ++alarm_index)
    {
        for (size_t i = 0; i < ElementsIn(values); ++i)
        {
            const uint8_t register_address = (alarm_index == 0) ? REG_ALARM0_DATE : REG_ALARM1_DATE;
            const uint8_t value = values[i];

            ExpectWriteValueRegister(register_address, __wrap_DecimalToBCD(value));
            assert_true(driverMCP79510_SetAlarmDate(value, alarm_index));
        }
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

    ExpectReadRegister(REG_TC_MONTH, ~(1 << REG_TC_MONTH_LPYR_BIT));
    assert_false(driverMCP79510_IsLeapYear());

    ExpectReadRegister(REG_TC_MONTH, 0xFF);
    assert_true(driverMCP79510_IsLeapYear());

    ExpectReadRegister(REG_TC_MONTH, (1 << REG_TC_MONTH_LPYR_BIT));
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
    ExpectWriteValueRegister(REG_TC_CONTROL, (1 << REG_TC_CONTROL_ALM0EN_BIT));
    driverMCP79510_EnableAlarm(true, 0);

    ExpectReadRegister(REG_TC_CONTROL, 0x00);
    ExpectWriteValueRegister(REG_TC_CONTROL, (1 << (REG_TC_CONTROL_ALM0EN_BIT + 1)));
    driverMCP79510_EnableAlarm(true, 1);


    ExpectReadRegister(REG_TC_CONTROL, 0xFF);
    ExpectWriteValueRegister(REG_TC_CONTROL, ~(1 << REG_TC_CONTROL_ALM0EN_BIT));
    driverMCP79510_EnableAlarm(false, 0);

    ExpectReadRegister(REG_TC_CONTROL, 0xFF);
    ExpectWriteValueRegister(REG_TC_CONTROL, ~(1 << (REG_TC_CONTROL_ALM0EN_BIT + 1)));
    driverMCP79510_EnableAlarm(false, 1);
}

static void test_driverMCP79510_IsExternalBatteryEnabled(void **state)
{
    ExpectReadRegister(REG_TC_DAY, 0x00);
    assert_false(driverMCP79510_IsExternalBatteryEnabled());

    ExpectReadRegister(REG_TC_DAY, ~(1 << REG_TC_DAY_VBATEN_BIT));
    assert_false(driverMCP79510_IsExternalBatteryEnabled());

    ExpectReadRegister(REG_TC_DAY, 0xFF);
    assert_true(driverMCP79510_IsExternalBatteryEnabled());

    ExpectReadRegister(REG_TC_DAY, (1 << REG_TC_DAY_VBATEN_BIT));
    assert_true(driverMCP79510_IsExternalBatteryEnabled());
}

static void test_driverMCP79510_EnableExternalBattery(void **state)
{
    ExpectReadRegister(REG_TC_DAY, 0x00);
    ExpectWriteValueRegister(REG_TC_DAY, (1 << REG_TC_DAY_VBATEN_BIT));
    driverMCP79510_EnableExternalBattery(true);

    ExpectReadRegister(REG_TC_DAY, 0xFF);
    ExpectWriteValueRegister(REG_TC_DAY, ~(1 << REG_TC_DAY_VBATEN_BIT));
    driverMCP79510_EnableExternalBattery(false);
}

static void test_driverMCP79510_GetBatterySwitchFlag(void **state)
{
    ExpectReadRegister(REG_TC_DAY, 0x00);
    assert_false(driverMCP79510_GetBatterySwitchFlag());

    ExpectReadRegister(REG_TC_DAY, ~(1 << REG_TC_DAY_PWRFAIL_BIT));
    assert_false(driverMCP79510_GetBatterySwitchFlag());

    ExpectReadRegister(REG_TC_DAY, 0xFF);
    assert_true(driverMCP79510_GetBatterySwitchFlag());

    ExpectReadRegister(REG_TC_DAY, (1 << REG_TC_DAY_PWRFAIL_BIT));
    assert_true(driverMCP79510_GetBatterySwitchFlag());
}

static void test_driverMCP79510_ClearBatterySwitchFlag(void **state)
{
    ExpectReadRegister(REG_TC_DAY, 0xFF);
    ExpectWriteValueRegister(REG_TC_DAY, ~(1 << REG_TC_DAY_PWRFAIL_BIT));
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

static void test_driverMCP79510_SetOscillatorTrimming_Negative(void **state)
{
    const int16_t values[] = {INT16_MIN, -256, -255, -254, -2, -1};

    for (size_t i = 0; i < ElementsIn(values); ++i)
    {
        if (values[i] < -UINT8_MAX)
        {
            ExpectWriteValueRegister(REG_TC_OSCTRIM, UINT8_MAX);
        }
        else
        {
            ExpectWriteValueRegister(REG_TC_OSCTRIM, abs(values[i]));
        }

        ExpectReadRegister(REG_TC_HOUR, 0xFF);
        ExpectWriteValueRegister(REG_TC_HOUR, (uint8_t)~(1 << REG_TC_HOUR_TRIMSIGN_BIT));
        driverMCP79510_SetOscillatorTrimming(values[i]);
    }
}

static void test_driverMCP79510_SetOscillatorTrimming_Positive(void **state)
{
    const int16_t values[] = {0, 1, 255, 256, INT16_MAX};

    for (size_t i = 0; i < ElementsIn(values); ++i)
    {
        if (values[i] > UINT8_MAX)
        {
            ExpectWriteValueRegister(REG_TC_OSCTRIM, UINT8_MAX);
        }
        else
        {
            ExpectWriteValueRegister(REG_TC_OSCTRIM, values[i]);
        }

        ExpectReadRegister(REG_TC_HOUR, 0x00);
        ExpectWriteValueRegister(REG_TC_HOUR, (uint8_t)(1 << REG_TC_HOUR_TRIMSIGN_BIT));
        driverMCP79510_SetOscillatorTrimming(values[i]);
    }
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
        cmocka_unit_test_setup(test_driverMCP79510_GetSecond, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_SetSecond_Invalid, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_SetSecond, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_SetAlarmSecond_InvalidIndex, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_SetAlarmSecond_InvalidSecond, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_SetAlarmSecond, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_GetMinute, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_SetMinute_Invalid, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_SetMinute, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_SetAlarmMinute_InvalidIndex, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_SetAlarmMinute_InvalidMinute, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_SetAlarmMinute, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_SetHour_Invalid, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_SetHour, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_SetAlarmHour_InvalidIndex, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_SetAlarmHour_InvalidHour, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_SetAlarmHour, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_GetDay, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_SetDay_Invalid, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_SetDay, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_GetDate, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_SetDate_Invalid, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_SetDate, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_SetAlarmDate_InvalidIndex, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_SetAlarmDate_InvalidDate, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_SetAlarmDate, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_Is24HourMode, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_IsLeapYear, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_EnableAlarm_InvalidIndex, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_EnableAlarm, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_IsExternalBatteryEnabled, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_EnableExternalBattery, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_GetBatterySwitchFlag, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_ClearBatterySwitchFlag, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_ClearAlarmFlag_InvalidIndex, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_ClearAlarmFlag, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_GetHour_12, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_GetHour_24, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_SetOscillatorTrimming_Negative, Setup),
        cmocka_unit_test_setup(test_driverMCP79510_SetOscillatorTrimming_Positive, Setup),
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

uint8_t __wrap_DecimalToBCD(uint8_t decimal_data)
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

    return (uint8_t)data;
}