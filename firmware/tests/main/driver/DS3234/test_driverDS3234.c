/**
 * @file   test_driverDS3234.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2021-01-13 (Last edit)
 * @brief  Test suite for the DS3234 RTC driver.
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
#include <string.h>

#include "test_driverDS3234.h"
#include "driverDS3234.h"
#include "driverDS3234Registers.h"
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

static int Setup(void **state);;
static void StubSPIPreCallback(void);
static void StubSPIPostCallback(void);
static void ExpectWriteRegister(uint8_t address);
static void ExpectWriteValueRegister(uint8_t address, uint8_t data);
static void ExpectReadRegister(uint8_t address, uint8_t data);
static void ExpectModifyRegister(uint8_t address, uint8_t initial_data, uint8_t new_data);
static void ExpectResetControlRegister(void);
static void Set24HourMode(bool enabled);

//////////////////////////////////////////////////////////////////////////
//INTERUPT SERVICE ROUTINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static int Setup(void **state)
{
    ExpectResetControlRegister();
    driverDS3234_Init(StubSPIPreCallback, StubSPIPostCallback);

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
    expect_value(__wrap_libSPI_WriteByte, data, address | WRITE_ADDRESS);
    expect_any(__wrap_libSPI_WriteByte, data);
}

static void ExpectWriteValueRegister(uint8_t address, uint8_t data)
{
    expect_value(__wrap_libSPI_WriteByte, data, address | WRITE_ADDRESS);
    expect_value(__wrap_libSPI_WriteByte, data, data);
}

static void ExpectReadRegister(uint8_t address, uint8_t data)
{
    expect_value(__wrap_libSPI_WriteByte, data, address | READ_ADDRESS);
    will_return(__wrap_libSPI_ReadByte, data);
}

static void ExpectModifyRegister(uint8_t address, uint8_t initial_data, uint8_t new_data)
{
    ExpectReadRegister(address,  initial_data);
    ExpectWriteValueRegister(address, new_data);
}

static void ExpectResetControlRegister(void)
{
    ExpectWriteValueRegister(REG_CONTROL, 0x1C);
}

static void Set24HourMode(bool enabled)
{
    const uint8_t register_data = 0x00;
    ExpectReadRegister(REG_HOUR, register_data);
    expect_value(__wrap_Bit_Get, *data_p, register_data);

    will_return(__wrap_Bit_Get, !enabled);
}

//////////////////////////////////////////////////////////////////////////
//TESTS
//////////////////////////////////////////////////////////////////////////

static void test_driverDS3234_Init_InvalidCallbacks(void **state)
{
    expect_assert_failure(driverDS3234_Init(NULL, NULL));
    expect_assert_failure(driverDS3234_Init(StubSPIPreCallback, NULL));
    expect_assert_failure(driverDS3234_Init(NULL, StubSPIPostCallback));
}

static void test_driverDS3234_Init(void **state)
{
    ExpectResetControlRegister();
    driverDS3234_Init(StubSPIPreCallback, StubSPIPostCallback);
}

static void test_driverDS3234_GetYear(void **state)
{
    const uint8_t values[] = {0, 1, 50, 98, 99};

    for (size_t i = 0; i < ElementsIn(values); ++i)
    {
        ExpectReadRegister(REG_YEAR, __wrap_DecimalToBCD(values[i]));

        uint8_t result;
        driverDS3234_GetYear(&result);
        assert_int_equal(result, values[i]);
    }
}

static void test_driverDS3234_SetYear_InvalidYear(void **state)
{
    const uint8_t values[] = {100, 101, UINT8_MAX};

    for (size_t i = 0; i < ElementsIn(values); ++i)
    {
        assert_false(driverDS3234_SetYear(values[i]));
    }
}

static void test_driverDS3234_SetYear(void **state)
{
    const uint8_t values[] = {0, 1, 50, 98, 99};

    for (size_t i = 0; i < ElementsIn(values); ++i)
    {
        const uint8_t year = values[i];
        ExpectWriteValueRegister(REG_YEAR, __wrap_DecimalToBCD(year));
        assert_true(driverDS3234_SetYear(year));
    }
}

static void test_driverDS3234_GetMonth(void **state)
{
    const uint8_t values[] = {1, 2, 6, 11, 12};
    const uint8_t century_bit = (1 << 7);

    for (size_t i = 0; i < ElementsIn(values); ++i)
    {
        ExpectReadRegister(REG_MONTH_CENTURY, century_bit | __wrap_DecimalToBCD(values[i]));

        uint8_t result;
        driverDS3234_GetMonth(&result);
        assert_int_equal(result, values[i]);
    }
}

static void test_driverDS3234_SetMonth_InvalidMonth(void **state)
{
    const uint8_t values[] = {0, 13, UINT8_MAX};

    for (size_t i = 0; i < ElementsIn(values); ++i)
    {
        assert_false(driverDS3234_SetMonth(values[i]));
    }
}

static void test_driverDS3234_SetMonth(void **state)
{
    const uint8_t values[] = {1, 2, 6, 11, 12};

    for (size_t i = 0; i < ElementsIn(values); ++i)
    {
        const uint8_t month = values[i];
        ExpectModifyRegister(REG_MONTH_CENTURY, 0x00, __wrap_DecimalToBCD(month));
        assert_true(driverDS3234_SetMonth(month));
    }
}

static void test_driverDS3234_GetDate(void **state)
{
    const uint8_t values[] = {1, 2, 15, 30, 31};

    for (size_t i = 0; i < ElementsIn(values); ++i)
    {
        const uint8_t date = values[i];
        ExpectReadRegister(REG_DATE, __wrap_DecimalToBCD(date));

        uint8_t result;
        driverDS3234_GetDate(&result);
        assert_int_equal(result, date);
    }
}

static void test_driverDS3234_SetDate_InvalidDate(void **state)
{
    const uint8_t values[] = {0, 32, UINT8_MAX};

    for (size_t i = 0; i < ElementsIn(values); ++i)
    {
        assert_false(driverDS3234_SetDate(values[i]));
    }
}

static void test_driverDS3234_SetDate(void **state)
{
    const uint8_t values[] = {1, 2, 15, 30, 31};

    for (size_t i = 0; i < ElementsIn(values); ++i)
    {
        const uint8_t date = values[i];
        ExpectWriteValueRegister(REG_DATE, __wrap_DecimalToBCD(date));
        assert_true(driverDS3234_SetDate(date));
    }
}

static void test_driverDS3234_GetDay(void **state)
{
    const uint8_t values[] = {1, 2, 4, 6, 7};

    for (size_t i = 0; i < ElementsIn(values); ++i)
    {
        const uint8_t day = values[i];
        ExpectReadRegister(REG_DAY, __wrap_DecimalToBCD(day));

        uint8_t result;
        driverDS3234_GetDay(&result);
        assert_int_equal(result, day);
    }
}

static void test_driverDS3234_SetDay_InvalidDay(void **state)
{
    const uint8_t values[] = {0, 8, UINT8_MAX};

    for (size_t i = 0; i < ElementsIn(values); ++i)
    {
        assert_false(driverDS3234_SetDay(values[i]));
    }
}

static void test_driverDS3234_SetDay(void **state)
{
    const uint8_t values[] = {1, 2, 4, 6, 7};

    for (size_t i = 0; i < ElementsIn(values); ++i)
    {
        const uint8_t day = values[i];
        ExpectWriteValueRegister(REG_DAY, __wrap_DecimalToBCD(day));
        assert_true(driverDS3234_SetDay(day));
    }
}

static void test_driverDS3234_GetHour(void **state)
{
    const uint8_t values[] = {0, 1, 12, 22, 23};

    for (size_t i = 0; i < ElementsIn(values); ++i)
    {
        const uint8_t value = values[i];
        ExpectReadRegister(REG_HOUR, __wrap_DecimalToBCD(value));

        uint8_t result;
        driverDS3234_GetHour(&result);
        assert_int_equal(result, value);
    }
}

static void test_driverDS3234_SetHour_InvalidHour(void **state)
{
    const uint8_t values_12[] = {0, 13, UINT8_MAX};
    for (size_t i = 0; i < ElementsIn(values_12); ++i)
    {
        Set24HourMode(false);
        assert_false(driverDS3234_SetHour(values_12[i]));
    }

    const uint8_t values_24[] = {24, UINT8_MAX};
    for (size_t i = 0; i < ElementsIn(values_24); ++i)
    {
        Set24HourMode(true);
        assert_false(driverDS3234_SetHour(values_24[i]));
    }
}

static void test_driverDS3234_SetHour(void **state)
{
    const uint8_t values_12[] = {1, 2, 6, 11, 12};
    for (size_t i = 0; i < ElementsIn(values_12); ++i)
    {
        Set24HourMode(false);
        ExpectWriteValueRegister(REG_HOUR, __wrap_DecimalToBCD(values_12[i]));
        assert_true(driverDS3234_SetHour(values_12[i]));
    }

    const uint8_t values_24[] = {0, 1, 12, 22, 23};
    for (size_t i = 0; i < ElementsIn(values_24); ++i)
    {
        Set24HourMode(true);
        ExpectWriteValueRegister(REG_HOUR, __wrap_DecimalToBCD(values_24[i]));
        assert_true(driverDS3234_SetHour(values_24[i]));
    }
}

static void test_driverDS3234_GetMinute(void **state)
{
    const uint8_t values[] = {0, 1, 30, 58, 59};

    for (size_t i = 0; i < ElementsIn(values); ++i)
    {
        const uint8_t value = values[i];
        ExpectReadRegister(REG_MINUTES, __wrap_DecimalToBCD(value));

        uint8_t result;
        driverDS3234_GetMinute(&result);
        assert_int_equal(result, value);
    }
}

static void test_driverDS3234_SetMinute_InvalidMinute(void **state)
{
    const uint8_t values[] = {60, UINT8_MAX};

    for (size_t i = 0; i < ElementsIn(values); ++i)
    {
        assert_false(driverDS3234_SetMinute(values[i]));
    }
}

static void test_driverDS3234_SetMinute(void **state)
{
    const uint8_t values[] = {0, 1, 30, 58, 59};

    for (size_t i = 0; i < ElementsIn(values); ++i)
    {
        const uint8_t value = values[i];
        ExpectWriteValueRegister(REG_MINUTES, __wrap_DecimalToBCD(value));
        assert_true(driverDS3234_SetMinute(value));
    }
}

static void test_driverDS3234_GetSecond(void **state)
{
    const uint8_t values[] = {0, 1, 30, 58, 59};

    for (size_t i = 0; i < ElementsIn(values); ++i)
    {
        const uint8_t value = values[i];
        ExpectReadRegister(REG_SECONDS, __wrap_DecimalToBCD(value));

        uint8_t result;
        driverDS3234_GetSecond(&result);
        assert_int_equal(result, value);
    }
}

static void test_driverDS3234_SetSecond_InvalidSecond(void **state)
{
    const uint8_t values[] = {60, UINT8_MAX};

    for (size_t i = 0; i < ElementsIn(values); ++i)
    {
        assert_false(driverDS3234_SetSecond(values[i]));
    }
}

static void test_driverDS3234_SetSecond(void **state)
{
    const uint8_t values[] = {0, 1, 30, 58, 59};

    for (size_t i = 0; i < ElementsIn(values); ++i)
    {
        const uint8_t value = values[i];
        ExpectWriteValueRegister(REG_SECONDS, __wrap_DecimalToBCD(value));
        assert_true(driverDS3234_SetSecond(value));
    }
}

static void test_driverDS3234_Is24HourMode(void **state)
{
    uint8_t register_data = 0xC0;
    ExpectReadRegister(REG_HOUR, register_data);
    expect_value(__wrap_Bit_Get, *data_p, register_data);
    will_return(__wrap_Bit_Get, true);
    assert_false(driverDS3234_Is24HourMode());

    register_data = 0x80;
    ExpectReadRegister(REG_HOUR, register_data);
    expect_value(__wrap_Bit_Get, *data_p, register_data);
    will_return(__wrap_Bit_Get, false);
    assert_true(driverDS3234_Is24HourMode());
}

static void test_driverDS3234_Enable24HourMode(void **state)
{
    /* Enable */
    uint8_t expected_register_data = ~(1 << HOUR_MODE_BIT);
    ExpectModifyRegister(REG_HOUR, 0xFF, expected_register_data);
    driverDS3234_Enable24HourMode(true);

    /* Disable */
    expected_register_data = (1 << HOUR_MODE_BIT);
    ExpectModifyRegister(REG_HOUR, 0x00, expected_register_data);
    driverDS3234_Enable24HourMode(false);
}

static void test_driverDS3234_EnableAlarm_InvalidIndex(void **state)
{
    expect_assert_failure(driverDS3234_EnableAlarm(true, 2));
    expect_assert_failure(driverDS3234_EnableAlarm(false, 2));

    expect_assert_failure(driverDS3234_EnableAlarm(true, UINT8_MAX));
    expect_assert_failure(driverDS3234_EnableAlarm(false, UINT8_MAX));
}

static void test_driverDS3234_EnableAlarm(void **state)
{
    const uint8_t max_index = 1;
    for (uint8_t i = 0; i <= max_index; ++i)
    {
        /* Enable */
        ExpectWriteRegister(REG_ALARM_1_DAY_DATE);
        uint8_t expected_register_data = (1 << i);
        ExpectModifyRegister(REG_CONTROL, 0x00, expected_register_data);
        driverDS3234_EnableAlarm(true, i);

        /* Disable */
        expected_register_data = ~(1 << i);
        ExpectModifyRegister(REG_CONTROL, 0xFF, expected_register_data);
        driverDS3234_EnableAlarm(false, i);
    }
}

static void test_driverDS3234_ClearAlarmFlag_InvalidIndex(void **state)
{
    expect_assert_failure(driverDS3234_ClearAlarmFlag(2));
    expect_assert_failure(driverDS3234_ClearAlarmFlag(UINT8_MAX));
}

static void test_driverDS3234_ClearAlarmFlag(void **state)
{
    const uint8_t max_index = 1;
    for (uint8_t i = 0; i <= max_index; ++i)
    {
        uint8_t expected_register_data = ~(1 << i);
        ExpectModifyRegister(REG_CONTROL_STATUS, 0xFF, expected_register_data);
        driverDS3234_ClearAlarmFlag(i);
    }
}

static void test_driverDS3234_SetAlarmDate_InvalidIndex(void **state)
{
    expect_assert_failure(driverDS3234_SetAlarmDate(1, 2));
    expect_assert_failure(driverDS3234_SetAlarmDate(UINT8_MAX, UINT8_MAX));
}

static void test_driverDS3234_SetAlarmDate_InvalidDate(void **state)
{
    const uint8_t values[] = {0, 32, UINT8_MAX};
    const uint8_t max_alarm_index = 1;

    for (uint8_t alarm_index = 0; alarm_index <= max_alarm_index; ++alarm_index)
    {
        for (size_t i = 0; i < ElementsIn(values); ++i)
        {
            const uint8_t date = values[i];
            assert_false(driverDS3234_SetAlarmDate(date, alarm_index));
        }
    }
}

static void test_driverDS3234_SetAlarmDate(void **state)
{
    const uint8_t values[] = {1, 2, 15, 30, 31};
    const uint8_t max_alarm_index = 1;

    for (uint8_t alarm_index = 0; alarm_index <= max_alarm_index; ++alarm_index)
    {
        const uint8_t register_address = (alarm_index == 0) ? REG_ALARM_1_DAY_DATE : REG_ALARM_2_DAY_DATE;

        for (size_t i = 0; i < ElementsIn(values); ++i)
        {
            const uint8_t date = values[i];
            ExpectWriteValueRegister(register_address, __wrap_DecimalToBCD(date));
            assert_true(driverDS3234_SetAlarmDate(date, alarm_index));
        }
    }
}

static void test_driverDS3234_SetAlarmHour_InvalidIndex(void **state)
{
    expect_assert_failure(driverDS3234_SetAlarmHour(0, 2));
    expect_assert_failure(driverDS3234_SetAlarmHour(UINT8_MAX, UINT8_MAX));
}

static void test_driverDS3234_SetAlarmHour_InvalidHour(void **state)
{
    const uint8_t max_alarm_index = 1;
    for (uint8_t alarm_index = 0; alarm_index <= max_alarm_index; ++alarm_index)
    {
        /* 12 hour mode */
        const uint8_t values_12[] = {0, 13, UINT8_MAX};
        for (size_t i = 0; i < ElementsIn(values_12); ++i)
        {
            Set24HourMode(false);
            assert_false(driverDS3234_SetAlarmHour(values_12[i], alarm_index));
        }

        /* 24 hour mode */
        const uint8_t values_24[] = {24, UINT8_MAX};
        for (size_t i = 0; i < ElementsIn(values_24); ++i)
        {
            Set24HourMode(true);
            assert_false(driverDS3234_SetAlarmHour(values_24[i], alarm_index));
        }
    }
}

static void test_driverDS3234_SetAlarmHour(void **state)
{
    const uint8_t max_alarm_index = 1;
    for (uint8_t alarm_index = 0; alarm_index <= max_alarm_index; ++alarm_index)
    {
        const uint8_t register_address = (alarm_index == 0) ? REG_ALARM_1_HOURS : REG_ALARM_2_HOURS;

        /* 12 hour mode */
        const uint8_t values_12[] = {1, 2, 6, 11, 12};
        for (size_t i = 0; i < ElementsIn(values_12); ++i)
        {
            Set24HourMode(false);
            ExpectWriteValueRegister(register_address, __wrap_DecimalToBCD(values_12[i]));
            assert_true(driverDS3234_SetAlarmHour(values_12[i], alarm_index));
        }

        /* 24 hour mode */
        const uint8_t values_24[] = {0, 1, 12, 22, 23};
        for (size_t i = 0; i < ElementsIn(values_24); ++i)
        {
            Set24HourMode(true);
            ExpectWriteValueRegister(register_address, __wrap_DecimalToBCD(values_24[i]));
            assert_true(driverDS3234_SetAlarmHour(values_24[i], alarm_index));
        }
    }
}

static void test_driverDS3234_SetAlarmMinute_InvalidIndex(void **state)
{
    expect_assert_failure(driverDS3234_SetAlarmMinute(0, 2));
    expect_assert_failure(driverDS3234_SetAlarmMinute(UINT8_MAX, UINT8_MAX));
}

static void test_driverDS3234_SetAlarmMinute_InvalidMinute(void **state)
{
    const uint8_t values[] = {60, UINT8_MAX};
    const uint8_t max_alarm_index = 1;

    for (uint8_t alarm_index = 0; alarm_index <= max_alarm_index; ++alarm_index)
    {
        for (size_t i = 0; i < ElementsIn(values); ++i)
        {
            const uint8_t minute = values[i];
            assert_false(driverDS3234_SetAlarmMinute(minute, alarm_index));
        }
    }
}

static void test_driverDS3234_SetAlarmMinute(void **state)
{
    const uint8_t values[] = {0, 1, 30, 58, 59};
    const uint8_t max_alarm_index = 1;

    for (uint8_t alarm_index = 0; alarm_index <= max_alarm_index; ++alarm_index)
    {
        const uint8_t register_address = (alarm_index == 0) ? REG_ALARM_1_MINUTES : REG_ALARM_2_MINUTES;

        for (size_t i = 0; i < ElementsIn(values); ++i)
        {
            const uint8_t minute = values[i];
            ExpectWriteValueRegister(register_address, __wrap_DecimalToBCD(minute));
            assert_true(driverDS3234_SetAlarmMinute(minute, alarm_index));
        }
    }
}

static void test_driverDS3234_SetAlarmSecond_InvalidSecond(void **state)
{
    const uint8_t values[] = {60, UINT8_MAX};

    for (size_t i = 0; i < ElementsIn(values); ++i)
    {
        const uint8_t second = values[i];
        assert_false(driverDS3234_SetAlarmSecond(second));
    }

}

static void test_driverDS3234_SetAlarmSecond(void **state)
{
    const uint8_t values[] = {0, 1, 30, 58, 59};

    for (size_t i = 0; i < ElementsIn(values); ++i)
    {
        const uint8_t second = values[i];
        ExpectWriteValueRegister(REG_ALARM_1_SECONDS, __wrap_DecimalToBCD(second));
        assert_true(driverDS3234_SetAlarmSecond(second));
    }
}

static void test_driverDS3234_WriteToSRAM_InvalidParameters(void **state)
{
    /* data_p */
    expect_assert_failure(driverDS3234_WriteToSRAM(0, NULL, 0));

    /* address and length */
    const uint16_t sram_size = 256;
    const uint8_t data[sram_size];
    assert_false(driverDS3234_WriteToSRAM(0x00, &data, sram_size + 1));
    assert_false(driverDS3234_WriteToSRAM(UINT8_MAX, &data, 2));
    assert_false(driverDS3234_WriteToSRAM(0x81, &data, 128));
    assert_false(driverDS3234_WriteToSRAM(UINT8_MAX, &data, SIZE_MAX));
}

static void test_driverDS3234_WriteToSRAM(void **state)
{
    const uint16_t sram_size = 256;
    uint8_t data[sram_size];

    for (size_t i = 0; i < sram_size; ++i)
    {
        data[i] = i;
    }

    /* Write zero bytes. */
    ExpectWriteValueRegister(REG_SRAM_ADDRESS, 0x00);
    assert_true(driverDS3234_WriteToSRAM(0x00, &data, 0));

    /* Write one byte at the last available address. */
    ExpectWriteValueRegister(REG_SRAM_ADDRESS, 0xFF);
    ExpectWriteValueRegister(REG_SRAM_DATA, 0);
    assert_true(driverDS3234_WriteToSRAM(0xFF, &data, 1));

    /* Write to the entire SRAM in one call. */
    uint8_t address = 0x00;
    size_t length = sram_size;
    ExpectWriteValueRegister(REG_SRAM_ADDRESS, address);
    for (size_t i = 0; i < length; ++i)
    {
        ExpectWriteValueRegister(REG_SRAM_DATA, i);
    }
    assert_true(driverDS3234_WriteToSRAM(address, &data, length));

    /* Write in the middle of the SRAM. */
    address = 0x80;
    length = 32;
    ExpectWriteValueRegister(REG_SRAM_ADDRESS, address);
    for (size_t i = 0; i < length; ++i)
    {
        ExpectWriteValueRegister(REG_SRAM_DATA, i);
    }
    assert_true(driverDS3234_WriteToSRAM(address, &data, length));
}

static void test_driverDS3234_ReadFromSRAM_InvalidParameters(void **state)
{
    /* data_p */
    expect_assert_failure(driverDS3234_ReadFromSRAM(0, NULL, 0));

    /* address and length */
    const uint16_t sram_size = 256;
    uint8_t data[sram_size];
    assert_false(driverDS3234_ReadFromSRAM(0, &data, sram_size + 1));
    assert_false(driverDS3234_ReadFromSRAM(UINT8_MAX, &data, 2));
    assert_false(driverDS3234_ReadFromSRAM(129, &data, 128));
    assert_false(driverDS3234_ReadFromSRAM(UINT8_MAX, &data, SIZE_MAX));
}

static void test_driverDS3234_ReadFromSRAM(void **state)
{
    /* address and length */
    const uint16_t sram_size = 256;
    uint8_t data[sram_size];

    /* Read zero bytes. */
    uint8_t address = 0x00;
    ExpectWriteValueRegister(REG_SRAM_ADDRESS, address);
    assert_true(driverDS3234_ReadFromSRAM(address, &data, 0));

    /* Read one byte at the last available address. */
    address = 0xFF;
    ExpectWriteValueRegister(REG_SRAM_ADDRESS, address);
    ExpectReadRegister(REG_SRAM_DATA, 0xAA);
    assert_true(driverDS3234_ReadFromSRAM(address, &data, 1));
    assert_int_equal(data[0], 0xAA);

    /* Read the entire SRAM in one call. */
    address = 0x00;
    size_t length = sram_size;
    ExpectWriteValueRegister(REG_SRAM_ADDRESS, address);
    for (size_t i = 0; i < length; ++i)
    {
        ExpectReadRegister(REG_SRAM_DATA, i);
    }
    assert_true(driverDS3234_ReadFromSRAM(address, &data, length));
    for (size_t i = 0; i < length; ++i)
    {
        assert_int_equal(data[i], i);
    }

    memset(&data, 0, sizeof(data));

    /* Read from the middle of the SRAM. */
    address = 0x80;
    length = 32;
    ExpectWriteValueRegister(REG_SRAM_ADDRESS, address);
    for (size_t i = 0; i < length; ++i)
    {
        ExpectReadRegister(REG_SRAM_DATA, i);
    }
    assert_true(driverDS3234_ReadFromSRAM(address, &data, length));
    for (size_t i = 0; i < length; ++i)
    {
        assert_int_equal(data[i], i);
    }
}

static void test_driverDS3234_ClearSRAM(void **state)
{
    const uint16_t sram_size = 256;

    ExpectWriteValueRegister(REG_SRAM_ADDRESS, 0x00);
    for (size_t i = 0; i < sram_size; ++i)
    {
        ExpectWriteValueRegister(REG_SRAM_DATA, 0);
    }
    driverDS3234_ClearSRAM();
}

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test(test_driverDS3234_Init_InvalidCallbacks),
        cmocka_unit_test(test_driverDS3234_Init),
        cmocka_unit_test_setup(test_driverDS3234_GetYear, Setup),
        cmocka_unit_test_setup(test_driverDS3234_SetYear_InvalidYear, Setup),
        cmocka_unit_test_setup(test_driverDS3234_SetYear, Setup),
        cmocka_unit_test_setup(test_driverDS3234_GetMonth, Setup),
        cmocka_unit_test_setup(test_driverDS3234_SetMonth_InvalidMonth, Setup),
        cmocka_unit_test_setup(test_driverDS3234_SetMonth, Setup),
        cmocka_unit_test_setup(test_driverDS3234_GetDate, Setup),
        cmocka_unit_test_setup(test_driverDS3234_SetDate_InvalidDate, Setup),
        cmocka_unit_test_setup(test_driverDS3234_SetDate, Setup),
        cmocka_unit_test_setup(test_driverDS3234_GetDay, Setup),
        cmocka_unit_test_setup(test_driverDS3234_SetDay_InvalidDay, Setup),
        cmocka_unit_test_setup(test_driverDS3234_SetDay, Setup),
        cmocka_unit_test_setup(test_driverDS3234_GetHour, Setup),
        cmocka_unit_test_setup(test_driverDS3234_SetHour_InvalidHour, Setup),
        cmocka_unit_test_setup(test_driverDS3234_SetHour, Setup),
        cmocka_unit_test_setup(test_driverDS3234_GetMinute, Setup),
        cmocka_unit_test_setup(test_driverDS3234_SetMinute_InvalidMinute, Setup),
        cmocka_unit_test_setup(test_driverDS3234_SetMinute, Setup),
        cmocka_unit_test_setup(test_driverDS3234_GetSecond, Setup),
        cmocka_unit_test_setup(test_driverDS3234_SetSecond_InvalidSecond, Setup),
        cmocka_unit_test_setup(test_driverDS3234_SetSecond, Setup),
        cmocka_unit_test_setup(test_driverDS3234_Is24HourMode, Setup),
        cmocka_unit_test_setup(test_driverDS3234_Enable24HourMode, Setup),
        cmocka_unit_test_setup(test_driverDS3234_EnableAlarm_InvalidIndex, Setup),
        cmocka_unit_test_setup(test_driverDS3234_EnableAlarm, Setup),
        cmocka_unit_test_setup(test_driverDS3234_ClearAlarmFlag_InvalidIndex, Setup),
        cmocka_unit_test_setup(test_driverDS3234_ClearAlarmFlag, Setup),
        cmocka_unit_test_setup(test_driverDS3234_SetAlarmDate_InvalidIndex, Setup),
        cmocka_unit_test_setup(test_driverDS3234_SetAlarmDate_InvalidDate, Setup),
        cmocka_unit_test_setup(test_driverDS3234_SetAlarmDate, Setup),
        cmocka_unit_test_setup(test_driverDS3234_SetAlarmHour_InvalidIndex, Setup),
        cmocka_unit_test_setup(test_driverDS3234_SetAlarmHour_InvalidHour, Setup),
        cmocka_unit_test_setup(test_driverDS3234_SetAlarmHour, Setup),
        cmocka_unit_test_setup(test_driverDS3234_SetAlarmMinute_InvalidIndex, Setup),
        cmocka_unit_test_setup(test_driverDS3234_SetAlarmMinute_InvalidMinute, Setup),
        cmocka_unit_test_setup(test_driverDS3234_SetAlarmMinute, Setup),
        cmocka_unit_test_setup(test_driverDS3234_SetAlarmSecond_InvalidSecond, Setup),
        cmocka_unit_test_setup(test_driverDS3234_SetAlarmSecond, Setup),
        cmocka_unit_test_setup(test_driverDS3234_WriteToSRAM_InvalidParameters, Setup),
        cmocka_unit_test_setup(test_driverDS3234_WriteToSRAM, Setup),
        cmocka_unit_test_setup(test_driverDS3234_ReadFromSRAM_InvalidParameters, Setup),
        cmocka_unit_test_setup(test_driverDS3234_ReadFromSRAM, Setup),
        cmocka_unit_test_setup(test_driverDS3234_ClearSRAM, Setup),
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
