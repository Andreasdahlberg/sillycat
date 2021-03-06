#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include "CRC.h"
#include "crc_16_table.h"

#define CRC_INIT_VALUE 0x0000

// All tests are validated against CRC values from http://www.sunshine2k.de/coding/javascript/crc/crc_js.html

void test_CRC_16_ZeroLength(void **state __attribute__((__unused__)))
{
    uint16_t crc;
    uint8_t data[] = {0x08}; //Dummy data != 0

    crc = CRC_16(&data, 0);

    assert_int_equal(crc, 0x0000);
}

void test_CRC_16_NullPointerZeroLength(void **state __attribute__((__unused__)))
{
    expect_assert_failure(CRC_16(NULL, 0))
}

void test_CRC_16_NullPointer(void **state __attribute__((__unused__)))
{
    expect_assert_failure(CRC_16(NULL, 1))
}

void test_CRC_16_SingleBytes(void **state __attribute__((__unused__)))
{
    uint16_t idx;
    for (idx = 0; idx < 256; ++idx)
    {
        uint8_t data = (uint8_t)idx;
        uint16_t crc;
        crc = CRC_16(&data, sizeof(data));

        assert_int_equal(crc, GetValueAtIndex((uint8_t)idx));
    }
}

void test_CRC_16_SeveralBytes(void **state __attribute__((__unused__)))
{
    uint16_t crc;

    //Random data
    uint8_t data[] = {0xFE, 0x29, 0x15, 0x7C, 0xA7, 0xAE, 0x7C, 0x42, 0x21, 0xA5, 0xA6, 0xDA, 0x6B, 0x32, 0x12, 0x94};

    crc = CRC_16(&data, sizeof(data));

    assert_int_equal(crc, 0x17FB);
}

int main(int argc, char *argv[])
{
    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test(test_CRC_16_ZeroLength),
        cmocka_unit_test(test_CRC_16_NullPointerZeroLength),
        cmocka_unit_test(test_CRC_16_NullPointer),
        cmocka_unit_test(test_CRC_16_SingleBytes),
        cmocka_unit_test(test_CRC_16_SeveralBytes)
    };

    if (argc >= 2)
    {
        cmocka_set_test_filter(argv[1]);
    }

    return cmocka_run_group_tests(tests, NULL, NULL);
}
