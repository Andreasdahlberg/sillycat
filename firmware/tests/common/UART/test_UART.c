/**
 * @file   test_UART.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2019-06-14 (Last edit)
 * @brief  Test suite for the UART module.
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
#include <string.h>

#include "UART.h"
#include "libUART.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static libUART_isr_callback rx_handler;
static libUART_isr_callback tx_handler;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static int Setup(void **state)
{
    will_return(__wrap_libUART_SetBaudRate, true);
    UART_Init();

    assert_non_null(rx_handler);
    assert_non_null(tx_handler);

    return 0;
}

//////////////////////////////////////////////////////////////////////////
//TESTS
//////////////////////////////////////////////////////////////////////////

static void test_UART_Enable(void **state)
{
    expect_value(__wrap_libUART_Enable, enable, true);
    UART_Enable(true);

    expect_value(__wrap_libUART_Enable, enable, false);
    UART_Enable(false);
}

static void test_UART_Write_ZeroBytes(void **state)
{
    uint8_t data = 0;

    assert_int_equal(UART_Write(&data, 0), 0);
}

static void test_UART_Write_BufferFull(void **state)
{
    uint8_t data[1024] = {0};

    assert_true(UART_Write(data, sizeof(data)) < sizeof(data));
}

static void test_UART_Write(void **state)
{
    char data[] = "foobar";
    assert_int_equal(UART_Write(data, strlen(data)), strlen(data));

    char result[32] = {0};
    char *result_p = result;

    while(tx_handler(result_p))
    {
        ++result_p;
    }

    assert_string_equal(result, data);
}

static void test_UART_Read_ZeroBytes(void **state)
{
    uint8_t data = 0;

    assert_int_equal(UART_Read(&data, 0), 0);
}

static void test_UART_Read_Truncated(void **state)
{
    /**
     * Add received data to RX buffer.
     */
    char rx_data[] = "foobar";
    for(size_t i = 0; i < strlen(rx_data); ++i)
    {
        assert_true(rx_handler(&rx_data[i]));
    }

    char data[4] = {0};
    assert_int_equal(UART_Read(&data, sizeof(data)), sizeof(data));
}

static void test_UART_Read(void **state)
{
    /**
     * Add received data to RX buffer.
     */
    char rx_data[] = "foobar";
    for(size_t i = 0; i < strlen(rx_data); ++i)
    {
        assert_true(rx_handler(&rx_data[i]));
    }

    char result[8] = {0};
    assert_int_equal(UART_Read(&result, sizeof(result)), strlen(rx_data));
    assert_string_equal(result, rx_data);
}

static void test_UART_WaitForTx_Empty(void **state)
{
    const uint32_t timeout = 1000;

    will_return_maybe(__wrap_Timer_GetMilliseconds, 0);
    will_return_maybe(__wrap_Timer_TimeDifference, 0);

    assert_true(UART_WaitForTx(timeout));
}

static void test_UART_WaitForTx_Timeout(void **state)
{
    const uint32_t timeout = 1000;

    uint8_t data = 0;
    assert_int_equal(UART_Write(&data, 1), 1);

    will_return_always(__wrap_Timer_GetMilliseconds, 0);
    will_return(__wrap_Timer_TimeDifference, 0);
    will_return(__wrap_Timer_TimeDifference, timeout);
    will_return(__wrap_Timer_TimeDifference, timeout + 1);

    assert_false(UART_WaitForTx(timeout));
}

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void __wrap_cli()
{
    expect_function_call(__wrap_sei);
}

void __wrap_sei()
{
    function_called();
}

void __wrap_libUART_Enable(bool enable)
{
    check_expected(enable);
}

void __wrap_libUART_SetCallbacks(libUART_isr_callback rx_callback, libUART_isr_callback tx_callback)
{
    rx_handler = rx_callback;
    tx_handler = tx_callback;
}

int main(int argc, char *argv[])
{
    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test_setup(test_UART_Enable, Setup),
        cmocka_unit_test_setup(test_UART_Write_ZeroBytes, Setup),
        cmocka_unit_test_setup(test_UART_Write_BufferFull, Setup),
        cmocka_unit_test_setup(test_UART_Write, Setup),
        cmocka_unit_test_setup(test_UART_Read_ZeroBytes, Setup),
        cmocka_unit_test_setup(test_UART_Read_Truncated, Setup),
        cmocka_unit_test_setup(test_UART_Read, Setup),
        cmocka_unit_test_setup(test_UART_WaitForTx_Empty, Setup),
        cmocka_unit_test_setup(test_UART_WaitForTx_Timeout, Setup),
    };

    if (argc >= 2)
    {
        cmocka_set_test_filter(argv[1]);
    }

    return cmocka_run_group_tests(tests, NULL, NULL);
}
