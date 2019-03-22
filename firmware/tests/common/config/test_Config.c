/**
 * @file   test_Config.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2019-01-22 (Last edit)
 * @brief  Test suite for the configuration module.
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

#include "Config.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static int Setup(void **state)
{
    will_return(__wrap_CRC_16, 0x9C54);
    assert_true(Config_Load());

    return 0;
}

static int Teardown(void **state)
{
    Config_SetReportInterval(60);
    will_return(__wrap_CRC_16, 0x9C54);
    Config_Save();

    return 0;
}

//////////////////////////////////////////////////////////////////////////
//TESTS
//////////////////////////////////////////////////////////////////////////

static void test_Config_GetVersion(void **state)
{
    assert_int_equal(Config_GetVersion(), 1);
}

static void test_Config_GetNetworkId(void **state)
{
    const uint8_t expected_id[6] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

    assert_memory_equal(Config_GetNetworkId(), expected_id, sizeof(expected_id));
}

static void test_Config_SetNetworkId_Null(void **state)
{
    expect_assert_failure(Config_SetNetworkId(NULL));
}

static void test_Config_SetNetworkId(void **state)
{
    uint8_t network_id[6] = {1, 2, 3, 4, 5, 6};

    Config_SetNetworkId(network_id);
    assert_memory_equal(Config_GetNetworkId(), network_id, sizeof(network_id));
}

static void test_Config_GetAESKey(void **state)
{
    const char expected_key[16] = "1DUMMYKEYFOOBAR1";

    assert_memory_equal(Config_GetAESKey(), expected_key, sizeof(expected_key));
}

static void test_Config_SetAESKey_Null(void **state)
{
    expect_assert_failure(Config_SetAESKey(NULL));
}

static void test_Config_SetAESKey(void **state)
{
    char key[16] = "TESTTESTTESTTEST";

    Config_SetAESKey(key);
    assert_memory_equal(Config_GetAESKey(), key, sizeof(key));
}

static void test_Config_GetReportInterval(void **state)
{
    assert_int_equal(Config_GetReportInterval(), 60);
}

static void test_Config_SetReportInterval_Invalid(void **state)
{
    expect_assert_failure(Config_SetReportInterval(0));
}

static void test_Config_SetReportInterval(void **state)
{
    uint32_t test_data[] = {1, 30, 60, UINT32_MAX};

    for (size_t i = 0; i < (sizeof(test_data) / sizeof(test_data[0])); ++i)
    {
        Config_SetReportInterval(test_data[i]);
        assert_int_equal(Config_GetReportInterval(), test_data[i]);
    }
}

static void test_Config_GetMasterAddress(void **state)
{
    assert_int_equal(Config_GetMasterAddress(), 170);
}

static void test_Config_GetAddress(void **state)
{
    assert_int_equal(Config_GetAddress(), 128);
}

static void test_Config_GetBroadcastAddress(void **state)
{
    assert_int_equal(Config_GetBroadcastAddress(), 255);
}

static void test_Config_Load_Config_InvalidCRC(void **state)
{
    will_return(__wrap_CRC_16, 0xF000);
    assert_false(Config_Load());
}

static void test_Config_Load_Config(void **state)
{
    const uint32_t report_interval = 1;

    Config_SetReportInterval(report_interval);
    will_return(__wrap_CRC_16, 0x9C54);
    assert_true(Config_Load());
    assert_int_not_equal(Config_GetReportInterval(), report_interval);
}

static void test_Config_Save_Config(void **state)
{
    const uint32_t report_interval = 1;

    Config_SetReportInterval(report_interval);
    will_return(__wrap_CRC_16, 0xF000);
    Config_Save();

    will_return(__wrap_CRC_16, 0xF000);
    assert_true(Config_Load());
    assert_int_equal(Config_GetReportInterval(), report_interval);
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
        cmocka_unit_test_setup(test_Config_GetVersion, Setup),
        cmocka_unit_test_setup(test_Config_GetNetworkId, Setup),
        cmocka_unit_test_setup(test_Config_SetNetworkId_Null, Setup),
        cmocka_unit_test_setup(test_Config_SetNetworkId, Setup),
        cmocka_unit_test_setup(test_Config_GetAESKey, Setup),
        cmocka_unit_test_setup(test_Config_SetAESKey_Null, Setup),
        cmocka_unit_test_setup(test_Config_SetAESKey, Setup),
        cmocka_unit_test_setup(test_Config_GetReportInterval, Setup),
        cmocka_unit_test_setup(test_Config_SetReportInterval_Invalid, Setup),
        cmocka_unit_test_setup(test_Config_SetReportInterval, Setup),
        cmocka_unit_test_setup(test_Config_GetMasterAddress, Setup),
        cmocka_unit_test_setup(test_Config_GetAddress, Setup),
        cmocka_unit_test_setup(test_Config_GetBroadcastAddress, Setup),
        cmocka_unit_test_setup(test_Config_Load_Config_InvalidCRC, Setup),
        cmocka_unit_test_setup(test_Config_Load_Config, Setup),
        cmocka_unit_test_setup_teardown(test_Config_Save_Config, Setup, Teardown),
    };

    if (argc >= 2)
    {
        cmocka_set_test_filter(argv[1]);
    }

    return cmocka_run_group_tests(tests, NULL, NULL);
}
