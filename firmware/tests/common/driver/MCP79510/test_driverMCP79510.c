/**
 * @file   test_driverMCP79510.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2020-01-26 (Last edit)
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
#include <stdint.h>
#include <stdbool.h>

#include "test_driverMCP79510.h"
#include "driverMCP79510.h"
#include "driverMCP79510Registers.h"

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

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test(test_driverMCP79510_Init_InvalidCallbacks),
        cmocka_unit_test(test_driverMCP79510_Init),
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

void __wrap_SetBit(uint8_t bit_index, bool state, uint8_t *data)
{
    assert_true(bit_index < 8);

    if (state == true)
    {
        *data |= (1 << bit_index);
    }
    else
    {
        *data &= ~(1 << bit_index);
    }
    return;
}

bool __wrap_IsBitSet(uint8_t bit_index, const uint8_t *data)
{
    assert_true(bit_index < 8);
    return (bool)(*data & (1 << bit_index));
}
