/**
 * @file   test_driverNHD223.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2021-01-28 (Last edit)
 * @brief  Test suite NHD2.23(SSD1305) graphic OLED display driver.
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

#include "test_driverNHD223.h"
#include "driverNHD223.h"
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

//////////////////////////////////////////////////////////////////////////
//INTERUPT SERVICE ROUTINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static int Setup(void **state)
{

    return 0;
}

static void ExpectResetSequence(void)
{
    expect_function_call(__wrap_Board_NHD223_SetResetLow);
    expect_in_range(__wrap__delay_us, delay_us, 3, 10);
    expect_function_call(__wrap_Board_NHD223_SetResetHigh);
}

static void ExpectSetWriteMode(void)
{
    expect_function_call(__wrap_Board_NHD223_SetReadWriteLow);
    expect_function_call(__wrap_Board_NHD223_SetDataPinsAsOutputs);
}

static void ExpectSetCommandMode(void)
{
    expect_function_call(__wrap_Board_NHD223_SetRegSelectLow);
}

static void ExpectSetDataMode(void)
{
    expect_function_call(__wrap_Board_NHD223_SetRegSelectHigh);
}

static void ExpectWriteCommand(uint8_t command)
{
    ExpectSetWriteMode();
    ExpectSetCommandMode();

    expect_function_call(__wrap_Board_NHD223_SetEnableHigh);
    expect_function_call(__wrap_Board_NHD223_SetChipSelectLow);

    expect_value(__wrap_Board_NHD223_SetDataPins, data, command);

    expect_function_call(__wrap_Board_NHD223_SetChipSelectHigh);
    expect_function_call(__wrap_Board_NHD223_SetEnableLow);
}

static void ExpectAnyWriteCommand(void)
{
    ExpectSetWriteMode();
    ExpectSetCommandMode();

    expect_function_call(__wrap_Board_NHD223_SetEnableHigh);
    expect_function_call(__wrap_Board_NHD223_SetChipSelectLow);

    expect_any(__wrap_Board_NHD223_SetDataPins, data);

    expect_function_call(__wrap_Board_NHD223_SetChipSelectHigh);
    expect_function_call(__wrap_Board_NHD223_SetEnableLow);
}

static void ExpectWriteData(uint8_t data)
{
    ExpectSetWriteMode();
    ExpectSetDataMode();

    expect_function_call(__wrap_Board_NHD223_SetEnableHigh);
    expect_function_call(__wrap_Board_NHD223_SetChipSelectLow);

    expect_value(__wrap_Board_NHD223_SetDataPins, data, data);

    expect_function_call(__wrap_Board_NHD223_SetChipSelectHigh);
    expect_function_call(__wrap_Board_NHD223_SetEnableLow);
}

static void ExpectSetPageAddress(uint8_t address)
{
    ExpectWriteCommand(SSD1305_SET_PAGE | address);
}

static void ExpectClearDisplay()
{
    for (size_t i = 0; i < NHD223_NUMBER_OF_PAGES; ++i)
    {
        ExpectSetPageAddress(i);

        for (size_t i = 0; i < NHD223_NUMBER_OF_COLUMNS; ++i)
        {
            ExpectWriteData(0x00);
        }
    }
}

//////////////////////////////////////////////////////////////////////////
//TESTS
//////////////////////////////////////////////////////////////////////////

static void test_driverNHD223_WriteCommand(void **state)
{
    uint8_t commands[] = {0, 128, UINT8_MAX};

    for ( size_t i = 0; i < ElementsIn(commands); ++i)
    {
        ExpectWriteCommand(commands[i]);
        driverNHD223_WriteCommand(commands[i]);
    }
}

static void test_driverNHD223_WriteData(void **state)
{
    uint8_t data[] = {0, 128, UINT8_MAX};

    for ( size_t i = 0; i < ElementsIn(data); ++i)
    {
        ExpectWriteData(data[i]);
        driverNHD223_WriteData(data[i]);
    }
}

static void test_driverNHD223_ResetDisplay(void **state)
{
    ExpectResetSequence();
    driverNHD223_ResetDisplay();
}

static void test_driverNHD223_Init(void **state)
{
    ExpectResetSequence();
    ExpectClearDisplay();
    ExpectResetSequence();
    driverNHD223_Init();
}

static void test_driverNHD223_SetHorizontalAddressingMode(void **state)
{
    ExpectWriteCommand(SSD1305_ADDRESSINGMODE);
    ExpectWriteCommand(SSD1305_HORIZONTALADDRESSINGMODE);
    driverNHD223_SetHorizontalAddressingMode();
}

static void test_driverNHD223_SetPageAddress_Invalid(void **state)
{
    const uint8_t values[] = {NHD223_NUMBER_OF_PAGES, UINT8_MAX};
    for (size_t i = 0; i < ElementsIn(values); ++i)
    {
        expect_assert_failure(driverNHD223_SetPageAddress(values[i]));
    }
}

static void test_driverNHD223_SetPageAddress(void **state)
{
    for (uint8_t page_address = 0; page_address < NHD223_NUMBER_OF_PAGES; ++page_address)
    {
        ExpectWriteCommand(SSD1305_SET_PAGE | page_address);
        driverNHD223_SetPageAddress(page_address);
    }
}

static void test_driverNHD223_SetPageAddressRange_Invalid(void **state)
{
    expect_assert_failure(driverNHD223_SetPageAddressRange(2, 1));
    expect_assert_failure(driverNHD223_SetPageAddressRange(UINT8_MAX, UINT8_MAX - 1));
}

static void test_driverNHD223_SetPageAddressRange(void **state)
{
    uint8_t start_address = 0;
    uint8_t end_address = UINT8_MAX;
    ExpectWriteCommand(SSD1305_SETSTARTPAGE);
    ExpectWriteCommand(start_address);
    ExpectWriteCommand(end_address);
    driverNHD223_SetPageAddressRange(start_address, end_address);

    start_address = 0;
    end_address = 0;
    ExpectWriteCommand(SSD1305_SETSTARTPAGE);
    ExpectWriteCommand(start_address);
    ExpectWriteCommand(end_address);
    driverNHD223_SetPageAddressRange(start_address, end_address);
}

static void test_driverNHD223_SetColumnAddress_Invalid(void **state)
{
    const uint8_t values[] = {NHD223_NUMBER_OF_COLUMNS, UINT8_MAX};
    for (size_t i = 0; i < ElementsIn(values); ++i)
    {
        expect_assert_failure(driverNHD223_SetColumnAddress(values[i]));
    }
}

static void test_driverNHD223_SetColumnAddress(void **state)
{
    for (uint8_t column_address = 0; column_address < NHD223_NUMBER_OF_COLUMNS; ++column_address)
    {
        ExpectWriteCommand(SSD1305_SETLOWCOLUMN | (column_address & 0x0F));
        ExpectWriteCommand(SSD1305_SETHIGHCOLUMN | (column_address >> 4));
        driverNHD223_SetColumnAddress(column_address);
    }
}

static void test_driverNHD223_SetColumnAddressRange_Invalid(void **state)
{
    expect_assert_failure(driverNHD223_SetColumnAddressRange(2, 1));
    expect_assert_failure(driverNHD223_SetColumnAddressRange(UINT8_MAX, UINT8_MAX - 1));
}

static void test_driverNHD223_SetColumnAddressRange(void **state)
{
    uint8_t start_address = 0;
    uint8_t end_address = UINT8_MAX;
    ExpectWriteCommand(SSD1305_SETSTARTCOLUMN);
    ExpectWriteCommand(start_address);
    ExpectWriteCommand(end_address);
    driverNHD223_SetColumnAddressRange(start_address, end_address);

    start_address = 0;
    end_address = 0;
    ExpectWriteCommand(SSD1305_SETSTARTCOLUMN);
    ExpectWriteCommand(start_address);
    ExpectWriteCommand(end_address);
    driverNHD223_SetColumnAddressRange(start_address, end_address);
}

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test(test_driverNHD223_WriteCommand),
        cmocka_unit_test(test_driverNHD223_WriteData),
        cmocka_unit_test(test_driverNHD223_ResetDisplay),
        cmocka_unit_test(test_driverNHD223_Init),
        cmocka_unit_test(test_driverNHD223_SetHorizontalAddressingMode),
        cmocka_unit_test(test_driverNHD223_SetPageAddress_Invalid),
        cmocka_unit_test(test_driverNHD223_SetPageAddress),
        cmocka_unit_test(test_driverNHD223_SetPageAddressRange_Invalid),
        cmocka_unit_test(test_driverNHD223_SetPageAddressRange),
        cmocka_unit_test(test_driverNHD223_SetColumnAddress_Invalid),
        cmocka_unit_test(test_driverNHD223_SetColumnAddress),
        cmocka_unit_test(test_driverNHD223_SetColumnAddressRange_Invalid),
        cmocka_unit_test(test_driverNHD223_SetColumnAddressRange)
    };

    if (argc >= 2)
    {
        cmocka_set_test_filter(argv[1]);
    }

    return cmocka_run_group_tests(tests, NULL, NULL);
}

//////////////////////////////////////////////////////////////////////////
//MOCK FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void __wrap__delay_us(double delay_us)
{
    check_expected(delay_us);
}

void __wrap_Board_NHD223_SetChipSelectLow(void)
{
    function_called();
}

void __wrap_Board_NHD223_SetChipSelectHigh(void)
{
    function_called();
}

void __wrap_Board_NHD223_SetResetLow(void)
{
    function_called();
}

void __wrap_Board_NHD223_SetResetHigh(void)
{
    function_called();
}

void __wrap_Board_NHD223_SetEnableHigh(void)
{
    function_called();
}

void __wrap_Board_NHD223_SetEnableLow(void)
{
    function_called();
}

void __wrap_Board_NHD223_SetRegSelectLow(void)
{
    function_called();
}

void __wrap_Board_NHD223_SetRegSelectHigh(void)
{
    function_called();
}

void __wrap_Board_NHD223_SetReadWriteLow(void)
{
    function_called();
}

void __wrap_Board_NHD223_SetDataPinsAsOutputs(void)
{
    function_called();
}

void __wrap_Board_NHD223_SetDataPins(uint8_t data)
{
    check_expected(data);
}
