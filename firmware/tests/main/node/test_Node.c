/**
 * @file   test_Node.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2020-10-21 (Last edit)
 * @brief  Test suite for the node module.
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

#include "Node.h"
#include "Packet.h"
#include "Sensor.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static struct node_t dummy_node;

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
    dummy_node = (__typeof__(dummy_node)) {0};
    Node_Init(&dummy_node, 0);

    return 0;
}

//////////////////////////////////////////////////////////////////////////
//TESTS
//////////////////////////////////////////////////////////////////////////

static void test_Node_Init_NULL(void **state)
{
    expect_assert_failure(Node_Init(NULL, 0));
}

static void test_Node_IsActive_NULL(void **state)
{
    expect_assert_failure(Node_IsActive(NULL));
}

static void test_Node_IsActive_NotConnected(void **state)
{
    will_return_maybe(__wrap_Timer_TimeDifference, 0);
    assert_false(Node_IsActive(&dummy_node));
}

static void test_Node_IsActive(void **state)
{
    will_return_maybe(__wrap_Timer_GetMilliseconds, 0);
    Node_ReportActivity(&dummy_node);

    for (size_t i = 1; i < 3; ++i)
    {
        uint32_t report_interval_ms = i * 1000;

        will_return_count(__wrap_Config_GetReportInterval, report_interval_ms / 1000, 4);

        will_return(__wrap_Timer_TimeDifference, 0);
        assert_true(Node_IsActive(&dummy_node));

        will_return(__wrap_Timer_TimeDifference, report_interval_ms - 1);
        assert_true(Node_IsActive(&dummy_node));

        will_return(__wrap_Timer_TimeDifference, report_interval_ms);
        assert_true(Node_IsActive(&dummy_node));

        will_return(__wrap_Timer_TimeDifference, report_interval_ms + 1);
        assert_false(Node_IsActive(&dummy_node));
    }
}

static void test_Node_ReportActivity_NULL(void **state)
{
    expect_assert_failure(Node_ReportActivity(NULL));
}

static void test_Node_GetLastActivity_NULL(void **state)
{
    expect_assert_failure(Node_GetLastActivity(NULL));
}

static void test_Node_GetLastActivity(void **state)
{
    assert_int_equal(Node_GetLastActivity(&dummy_node), 0);

    uint32_t data[] = {0, 1, 1000, UINT32_MAX};
    for (size_t i = 0; i < sizeof(data) / sizeof(data[0]); ++i)
    {
        const uint32_t timestamp = data[i];

        will_return(__wrap_Timer_GetMilliseconds, timestamp);
        Node_ReportActivity(&dummy_node);
        assert_int_equal(Node_GetLastActivity(&dummy_node), timestamp);
    }
}

static void test_Node_GetTemperatureSensor_NULL(void **state)
{
    expect_assert_failure(Node_GetTemperatureSensor(NULL));
}

static void test_Node_GetTemperatureSensor(void **state)
{
    assert_non_null(Node_GetTemperatureSensor(&dummy_node));
}

static void test_Node_GetHumiditySensor_NULL(void **state)
{
    expect_assert_failure(Node_GetHumiditySensor(NULL));
}

static void test_Node_GetHumiditySensor(void **state)
{
    assert_non_null(Node_GetHumiditySensor(&dummy_node));
}

static void test_Node_IsBatteryOk_NULL(void **state)
{
    expect_assert_failure(Node_IsBatteryOk(NULL));
}

static void test_Node_IsBatteryOk(void **state)
{
    struct packet_t dummy_packet = {0};

    dummy_packet.battery.voltage = 0;
    Node_Update(&dummy_node, &dummy_packet, sizeof(dummy_packet));
    assert_false(Node_IsBatteryOk(&dummy_node));

    dummy_packet.battery.voltage = 2199;
    Node_Update(&dummy_node, &dummy_packet, sizeof(dummy_packet));
    assert_false(Node_IsBatteryOk(&dummy_node));

    dummy_packet.battery.voltage = 2200;
    Node_Update(&dummy_node, &dummy_packet, sizeof(dummy_packet));
    assert_false(Node_IsBatteryOk(&dummy_node));

    dummy_packet.battery.voltage = 2201;
    Node_Update(&dummy_node, &dummy_packet, sizeof(dummy_packet));
    assert_true(Node_IsBatteryOk(&dummy_node));

    dummy_packet.battery.voltage = UINT16_MAX;
    Node_Update(&dummy_node, &dummy_packet, sizeof(dummy_packet));
    assert_true(Node_IsBatteryOk(&dummy_node));
}

static void test_Node_GetBatteryVoltage_NULL(void **state)
{
    expect_assert_failure(Node_GetBatteryVoltage(NULL));
}

static void test_Node_GetBatteryTemperature_NULL(void **state)
{
    expect_assert_failure(Node_GetBatteryTemperature(NULL));
}

static void test_Node_IsBatteryCharging_NULL(void **state)
{
    expect_assert_failure(Node_IsBatteryCharging(NULL));
}

static void test_Node_IsBatteryChargerConnected_NULL(void **state)
{
    expect_assert_failure(Node_IsBatteryChargerConnected(NULL));
}

static void test_Node_GetRSSI_NULL(void **state)
{
    expect_assert_failure(Node_GetRSSI(NULL));
}

static void test_Node_SetRSSI_NULL(void **state)
{
    expect_assert_failure(Node_SetRSSI(NULL, 0));
}

static void test_Node_SetGetRSSI(void **state)
{
    Node_SetRSSI(&dummy_node, INT8_MIN);
    assert_int_equal(Node_GetRSSI(&dummy_node), INT8_MIN);

    Node_SetRSSI(&dummy_node, 0);
    assert_int_equal(Node_GetRSSI(&dummy_node), 0);

    Node_SetRSSI(&dummy_node, INT8_MAX);
    assert_int_equal(Node_GetRSSI(&dummy_node), INT8_MAX);
}

static void test_Node_GetID_NULL(void **state)
{
    expect_assert_failure(Node_GetID(NULL));
}

static void test_Node_GetID(void **state)
{
    uint8_t id;
    dummy_node = (__typeof__(dummy_node)) {0};

    id = 0;
    Node_Init(&dummy_node, id);
    assert_int_equal(Node_GetID(&dummy_node), id);

    id = 128;
    Node_Init(&dummy_node, id);
    assert_int_equal(Node_GetID(&dummy_node), id);

    id = 255;
    Node_Init(&dummy_node, id);
    assert_int_equal(Node_GetID(&dummy_node), id);
}

static void test_Node_Update_NULL(void **state)
{
    struct packet_t dummy_packet;

    expect_assert_failure(Node_Update(NULL, NULL, 0));
    expect_assert_failure(Node_Update(&dummy_node, NULL, 0));
    expect_assert_failure(Node_Update(NULL, &dummy_packet, 0));
}

static void test_Node_Update_IncorrectDataLength(void **state)
{
    const uint16_t voltage = 99;

    struct packet_t dummy_packet =
    {
        .battery = {
            .voltage = voltage
        }
    };

    /**
     * The battery voltage is not expected to be equal to the value in the
     * packet since the packet should be rejected because of the invalid
     * length.
     */

    Node_Update(&dummy_node, &dummy_packet, 0);
    assert_int_not_equal(Node_GetBatteryVoltage(&dummy_node), voltage);

    Node_Update(&dummy_node, &dummy_packet, sizeof(dummy_packet) - 1);
    assert_int_not_equal(Node_GetBatteryVoltage(&dummy_node), 99);

    Node_Update(&dummy_node, &dummy_packet, sizeof(dummy_packet) + 1);
    assert_int_not_equal(Node_GetBatteryVoltage(&dummy_node), 99);
}

static void test_Node_Update_InvalidSensorData(void **state)
{
    struct sensor_t *sensor_p;
    struct packet_t dummy_packet =
    {
        .sensor = {
            .valid = false
        }
    };

    Node_Update(&dummy_node, &dummy_packet, sizeof(dummy_packet));

    sensor_p = Node_GetTemperatureSensor(&dummy_node);
    assert_false(sensor_p->valid);

    sensor_p = Node_GetHumiditySensor(&dummy_node);
    assert_false(sensor_p->valid);
}

static void test_Node_Update_ValidSensorData(void **state)
{
    const uint16_t temperature = 250;
    const uint16_t humidity = 650;

    struct sensor_t *sensor_p;
    struct packet_t dummy_packet =
    {
        .sensor = {
            .valid = true,
            .temperature = temperature,
            .humidity = humidity
        }
    };

    Node_Update(&dummy_node, &dummy_packet, sizeof(dummy_packet));

    sensor_p = Node_GetTemperatureSensor(&dummy_node);
    assert_true(sensor_p->valid);
    assert_int_equal(sensor_p->value, temperature);

    sensor_p = Node_GetHumiditySensor(&dummy_node);
    assert_true(sensor_p->valid);
    assert_int_equal(sensor_p->value, humidity);
}

static void test_Node_Update_BatteryData(void **state)
{
    struct packet_t dummy_packet =
    {
        .battery = {
            .voltage = 2431,
            .temperature = 259,
            .charging = false,
            .connected = false
        }
    };

    Node_Update(&dummy_node, &dummy_packet, sizeof(dummy_packet));
    assert_int_equal(Node_GetBatteryVoltage(&dummy_node), dummy_packet.battery.voltage);
    assert_int_equal(Node_GetBatteryTemperature(&dummy_node), dummy_packet.battery.temperature);
    assert_false(Node_IsBatteryCharging(&dummy_node));
    assert_false(Node_IsBatteryChargerConnected(&dummy_node));

    dummy_packet.battery.voltage = 3299;
    dummy_packet.battery.temperature = 352;
    dummy_packet.battery.charging = true;
    dummy_packet.battery.connected = true;

    Node_Update(&dummy_node, &dummy_packet, sizeof(dummy_packet));
    assert_int_equal(Node_GetBatteryVoltage(&dummy_node), dummy_packet.battery.voltage);
    assert_int_equal(Node_GetBatteryTemperature(&dummy_node), dummy_packet.battery.temperature);
    assert_true(Node_IsBatteryCharging(&dummy_node));
    assert_true(Node_IsBatteryChargerConnected(&dummy_node));
}

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    const struct CMUnitTest node_tests[] =
    {
        cmocka_unit_test(test_Node_Init_NULL),
        cmocka_unit_test_setup(test_Node_IsActive_NULL, Setup),
        cmocka_unit_test_setup(test_Node_IsActive_NotConnected, Setup),
        cmocka_unit_test_setup(test_Node_IsActive, Setup),
        cmocka_unit_test_setup(test_Node_ReportActivity_NULL, Setup),
        cmocka_unit_test_setup(test_Node_GetLastActivity_NULL, Setup),
        cmocka_unit_test_setup(test_Node_GetLastActivity, Setup),
        cmocka_unit_test_setup(test_Node_GetTemperatureSensor_NULL, Setup),
        cmocka_unit_test_setup(test_Node_GetTemperatureSensor, Setup),
        cmocka_unit_test_setup(test_Node_GetHumiditySensor_NULL, Setup),
        cmocka_unit_test_setup(test_Node_GetHumiditySensor, Setup),
        cmocka_unit_test_setup(test_Node_IsBatteryOk_NULL, Setup),
        cmocka_unit_test_setup(test_Node_IsBatteryOk, Setup),
        cmocka_unit_test_setup(test_Node_GetBatteryVoltage_NULL, Setup),
        cmocka_unit_test_setup(test_Node_GetBatteryTemperature_NULL, Setup),
        cmocka_unit_test_setup(test_Node_IsBatteryCharging_NULL, Setup),
        cmocka_unit_test_setup(test_Node_IsBatteryChargerConnected_NULL, Setup),
        cmocka_unit_test_setup(test_Node_GetRSSI_NULL, Setup),
        cmocka_unit_test_setup(test_Node_SetRSSI_NULL, Setup),
        cmocka_unit_test_setup(test_Node_SetGetRSSI, Setup),
        cmocka_unit_test_setup(test_Node_GetID_NULL, Setup),
        cmocka_unit_test(test_Node_GetID),
        cmocka_unit_test_setup(test_Node_Update_NULL, Setup),
        cmocka_unit_test_setup(test_Node_Update_IncorrectDataLength, Setup),
        cmocka_unit_test_setup(test_Node_Update_InvalidSensorData, Setup),
        cmocka_unit_test_setup(test_Node_Update_ValidSensorData, Setup),
        cmocka_unit_test_setup(test_Node_Update_BatteryData, Setup),
    };

    if (argc >= 2)
    {
        cmocka_set_test_filter(argv[1]);
    }

    return cmocka_run_group_tests(node_tests, NULL, NULL);
}
