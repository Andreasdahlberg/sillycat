/**
 * @file   test_Display.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2021-02-06 (Last edit)
 * @brief  Test suite for the Display module.
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

#include "common.h"
#include "driverNHD223.h"
#include "Display.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static uint8_t expected_vram[NHD223_NUMBER_OF_PAGES][NHD223_NUMBER_OF_COLUMNS];

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static void ClearExpectedVRAM(void)
{
    memset(expected_vram, 0x00, sizeof(expected_vram));
}

static int Setup(void **state)
{
    ClearExpectedVRAM();

    expect_function_call(__wrap_driverNHD223_Init);
    expect_function_call_any(__wrap_driverNHD223_SetHorizontalAddressingMode);
    expect_any(__wrap_driverNHD223_SetColumnAddressRange, start_address);
    expect_any(__wrap_driverNHD223_SetColumnAddressRange, end_address);
    expect_any(__wrap_driverNHD223_SetPageAddressRange, start_address);
    expect_any(__wrap_driverNHD223_SetPageAddressRange, end_address);

    Display_Init();

    return 0;
}

static void ExpectFlush(void)
{
    for (uint8_t page = 0; page < NHD223_NUMBER_OF_PAGES; ++page)
    {
        for (uint8_t column = 0; column < NHD223_NUMBER_OF_COLUMNS; ++column)
        {
            expect_value(__wrap_driverNHD223_WriteData, data, expected_vram[page][column]);
        }
    }
}

static void CheckVRAM(void)
{
    ExpectFlush();
    Display_Flush();
}

//////////////////////////////////////////////////////////////////////////
//TESTS
//////////////////////////////////////////////////////////////////////////

void test_Display_Init(void **state)
{
    expect_function_call(__wrap_driverNHD223_Init);
    expect_function_call(__wrap_driverNHD223_SetHorizontalAddressingMode);

    const uint8_t max_column_address = 127;
    expect_value(__wrap_driverNHD223_SetColumnAddressRange, start_address, 0);
    expect_value(__wrap_driverNHD223_SetColumnAddressRange, end_address, max_column_address);

    const uint8_t max_page_address = 3;
    expect_value(__wrap_driverNHD223_SetPageAddressRange, start_address, 0);
    expect_value(__wrap_driverNHD223_SetPageAddressRange, end_address, max_page_address);

    Display_Init();
}

static void test_Display_On(void **state)
{
    expect_value(__wrap_driverNHD223_WriteCommand, command, SSD1305_DISPLAYON);
    Display_On();
}

static void test_Display_Off(void **state)
{
    expect_value(__wrap_driverNHD223_WriteCommand, command, SSD1305_DISPLAYOFF);
    Display_Off();
}

static void test_Display_Flush(void **state)
{
    CheckVRAM();
}

static void test_Display_SetBrightness(void **state)
{
    uint8_t values[] = {0, 128, UINT8_MAX};

    for (size_t i = 0; i < ElementsIn(values); ++i)
    {
        expect_value(__wrap_driverNHD223_WriteCommand, command, SSD1305_SETCONTRAST);
        expect_value(__wrap_driverNHD223_WriteCommand, command, values[i]);
        Display_SetBrightness(values[i]);
    }
}

static void test_Display_SetPixel_InvalidCoordinates(void **state)
{
    /* VRAM is zeroed in the setup function and should never change. */

    Display_SetPixel(NHD223_NUMBER_OF_COLUMNS, 0);
    CheckVRAM();

    Display_SetPixel(UINT8_MAX, 0);
    CheckVRAM();

    Display_SetPixel(0, NHD223_NUMBER_OF_PAGES * 8);
    CheckVRAM();

    Display_SetPixel(0, UINT8_MAX);
    CheckVRAM();

    Display_SetPixel(NHD223_NUMBER_OF_COLUMNS, NHD223_NUMBER_OF_PAGES * 8);
    CheckVRAM();
}

static void test_Display_SetPixel(void **state)
{
    uint8_t expected_column;
    uint8_t expected_page;
    uint8_t expected_value;

    expected_column = 0;
    expected_page = 0;
    expected_value = 0x01;
    expected_vram[expected_page][expected_column] = expected_value;
    Display_SetPixel(0, 0);
    CheckVRAM();

    expected_column = 0;
    expected_page = 0;
    expected_value = 0x03;
    expected_vram[expected_page][expected_column] = expected_value;
    Display_SetPixel(0, 1);
    CheckVRAM();

    expected_column = 127;
    expected_page = 3;
    expected_value = 0x80;
    expected_vram[expected_page][expected_column] = expected_value;
    Display_SetPixel(127, 31);
    CheckVRAM();
}

static void test_Display_Clear(void **state)
{
    Display_SetPixel(0, 0);
    ExpectFlush();
    Display_Clear();
}

static void test_Display_Reset(void **state)
{
    Display_SetPixel(0, 0);
    expect_function_call(__wrap_driverNHD223_ResetDisplay);
    ExpectFlush();
    Display_Reset();
}

static void test_Display_Rotate(void **state)
{
    /* Check that VRAM is cleared when changing rotation. */
    Display_SetPixel(0, 0);
    Display_Rotate(true);
    CheckVRAM();

    /* Check that the correct pixel is set, lower right corner instead of upper left corner. */
    uint8_t expected_column = 127;
    uint8_t expected_page = 3;
    uint8_t expected_value = 0x80;
    expected_vram[expected_page][expected_column] = expected_value;
    Display_SetPixel(0, 0);
    CheckVRAM();

    /* Rotate back again. */
    ClearExpectedVRAM();
    expected_column = 0;
    expected_page = 0;
    expected_value = 0x01;
    expected_vram[expected_page][expected_column] = expected_value;
    Display_Rotate(false);
    Display_SetPixel(0, 0);
    CheckVRAM();
}

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test(test_Display_Init),
        cmocka_unit_test_setup(test_Display_On, Setup),
        cmocka_unit_test_setup(test_Display_Off, Setup),
        cmocka_unit_test_setup(test_Display_Flush, Setup),
        cmocka_unit_test_setup(test_Display_SetBrightness, Setup),
        cmocka_unit_test_setup(test_Display_SetPixel_InvalidCoordinates, Setup),
        cmocka_unit_test_setup(test_Display_SetPixel, Setup),
        cmocka_unit_test_setup(test_Display_Clear, Setup),
        cmocka_unit_test_setup(test_Display_Reset, Setup),
        cmocka_unit_test_setup(test_Display_Rotate, Setup),
    };

    if (argc >= 2)
    {
        cmocka_set_test_filter(argv[1]);
    }

    return cmocka_run_group_tests(tests, NULL, NULL);
}

