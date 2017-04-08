/**
 * @file   test_FIFO.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2017-03-26 (Last edit)
 * @brief  Unit tests for the FIFO module.
 *
 * Detailed description of file.
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

#include "FIFO.h"

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

static void FillBuffer(fifo_type *fifo, size_t number_of_elements);

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void test_FIFO_New_Byte_Buffer(void **state __attribute__((__unused__)))
{
    uint8_t buffer[8];


    fifo_type fifo = {NULL, 0xFF, 0xFF, 0xFF, 0xFF};

    fifo = FIFO_New(buffer);

    assert_ptr_equal(fifo.data, buffer);
    assert_int_equal(fifo.head, 0);
    assert_int_equal(fifo.tail, 0);
    assert_int_equal(fifo.size, 8);
    assert_int_equal(fifo.item_size, 1);
}

void test_FIFO_Push_NULL_arguments(void **state __attribute__((__unused__)))
{
    uint8_t dummy_item;
    fifo_type dummy_fifo;

    expect_assert_failure(FIFO_Push(NULL, &dummy_item));
    expect_assert_failure(FIFO_Push(&dummy_fifo, NULL));
    expect_assert_failure(FIFO_Push(NULL, NULL));
}


void test_FIFO_Push_Empty(void **state __attribute__((__unused__)))
{
    uint8_t buffer[8] = {0};
    uint8_t item = 0xAA;
    fifo_type fifo = FIFO_New(buffer);

    // We expect true when pushing to a empty FIFO.
    assert_true(FIFO_Push(&fifo, &item));

    // Make sure the FIFO content is correct.
    uint8_t expected_buffer_content[8] = {0xAA, 0, 0, 0, 0, 0, 0, 0};
    assert_memory_equal(expected_buffer_content, buffer,
                        sizeof(expected_buffer_content));
}

void test_FIFO_Push_Full(void **state __attribute__((__unused__)))
{
    uint8_t buffer[8];
    uint8_t item = 0xAA;
    fifo_type fifo = FIFO_New(buffer);

    // Fill the FIFO with numbers 0 to 7.
    FillBuffer(&fifo, sizeof(buffer));

    // We expect false when pushing to a full FIFO.
    assert_false(FIFO_Push(&fifo, &item));

    // Make sure the FIFO content is unchanged.
    uint8_t expected_buffer_content[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    assert_memory_equal(expected_buffer_content, buffer,
                        sizeof(expected_buffer_content));
}

void test_FIFO_Pop_NULL_arguments(void **state __attribute__((__unused__)))
{
    uint8_t dummy_item;
    fifo_type dummy_fifo;

    expect_assert_failure(FIFO_Pop(NULL, &dummy_item));
    expect_assert_failure(FIFO_Pop(&dummy_fifo, NULL));
    expect_assert_failure(FIFO_Pop(NULL, NULL));
}

void test_FIFO_Pop_Empty(void **state __attribute__((__unused__)))
{
    uint8_t buffer[8];
    uint8_t item;
    fifo_type fifo = FIFO_New(buffer);

    // We expect false when poping from a empty FIFO.
    assert_false(FIFO_Pop(&fifo, &item));
}

void test_FIFO_Pop_NonEmpty(void **state __attribute__((__unused__)))
{
    uint8_t buffer[8];
    uint8_t item = 0xAA;

    fifo_type fifo = FIFO_New(buffer);
    FIFO_Push(&fifo, &item);

    // Clear item so we know for sure that it is set by FIFO_Pop().
    item = 0x00;

    // We expect true when poping from a non empty FIFO.
    assert_true(FIFO_Pop(&fifo, &item));

    // Check if we got the correct value.
    assert_int_equal(0xAA, item);

    // Since we only pushed one item to the FIFO it should be empty here.
    assert_true(FIFO_IsEmpty(&fifo));
}

void test_FIFO_Peek_NULL_arguments(void **state __attribute__((__unused__)))
{
    uint8_t dummy_item;
    fifo_type dummy_fifo;

    expect_assert_failure(FIFO_Peek(NULL, &dummy_item));
    expect_assert_failure(FIFO_Peek(&dummy_fifo, NULL));
    expect_assert_failure(FIFO_Peek(NULL, NULL));
}

void test_FIFO_Peek_Empty(void **state __attribute__((__unused__)))
{
    uint8_t buffer[8];
    uint8_t item;
    fifo_type fifo = FIFO_New(buffer);

    // We expect false when peeking from a empty FIFO.
    assert_false(FIFO_Peek(&fifo, &item));
}

void test_FIFO_Peek_NonEmpty(void **state __attribute__((__unused__)))
{
    uint8_t buffer[8];
    uint8_t item = 0xAA;

    fifo_type fifo = FIFO_New(buffer);
    FIFO_Push(&fifo, &item);

    // Clear item so we know for sure that it is set by FIFO_Peek().
    item = 0x00;

    // We expect true when peeking from a non empty FIFO.
    assert_true(FIFO_Peek(&fifo, &item));

    // Check if we got the correct value.
    assert_int_equal(0xAA, item);

    // The FIFO should be unchanged by the peek.
    assert_false(FIFO_IsEmpty(&fifo));
}

void test_FIFO_IsEmpty_NULL(void **state __attribute__((__unused__)))
{
    expect_assert_failure(FIFO_IsEmpty(NULL));
}

void test_FIFO_IsEmpty_Empty(void **state __attribute__((__unused__)))
{
    uint8_t buffer[8];
    fifo_type fifo = FIFO_New(buffer);

    assert_true(FIFO_IsEmpty(&fifo));
}

void test_FIFO_IsEmpty_Full(void **state __attribute__((__unused__)))
{
    uint8_t buffer[8];
    fifo_type fifo = FIFO_New(buffer);
    FillBuffer(&fifo, sizeof(buffer));

    assert_false(FIFO_IsEmpty(&fifo));
}

void test_FIFO_IsEmpty_NotEmpty(void **state __attribute__((__unused__)))
{
    uint8_t buffer[8];
    fifo_type fifo = FIFO_New(buffer);
    FillBuffer(&fifo, sizeof(buffer) / 2);

    assert_false(FIFO_IsEmpty(&fifo));
}

void test_FIFO_IsFull_NULL(void **state __attribute__((__unused__)))
{
    expect_assert_failure(FIFO_IsFull(NULL));
}

void test_FIFO_IsFull_Empty(void **state __attribute__((__unused__)))
{
    uint8_t buffer[8];
    fifo_type fifo = FIFO_New(buffer);

    assert_false(FIFO_IsFull(&fifo));
}

void test_FIFO_IsFull_Full(void **state __attribute__((__unused__)))
{
    uint8_t buffer[8];
    fifo_type fifo = FIFO_New(buffer);
    FillBuffer(&fifo, sizeof(buffer));

    assert_true(FIFO_IsFull(&fifo));
}

void test_FIFO_IsFull_NotEmpty(void **state __attribute__((__unused__)))
{
    uint8_t buffer[8];
    fifo_type fifo = FIFO_New(buffer);
    FillBuffer(&fifo, sizeof(buffer) / 2);

    assert_false(FIFO_IsFull(&fifo));
}

void test_FIFO_Clear_NULL(void **state __attribute__((__unused__)))
{
    expect_assert_failure(FIFO_Clear(NULL));
}

void test_FIFO_Clear_Full(void **state __attribute__((__unused__)))
{
    uint8_t buffer[8];
    fifo_type fifo = FIFO_New(buffer);
    FillBuffer(&fifo, sizeof(buffer));

    FIFO_Clear(&fifo);
    assert_true(FIFO_IsEmpty(&fifo));
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static void FillBuffer(fifo_type *fifo, size_t number_of_elements)
{
    for (size_t cnt = 0; cnt < number_of_elements; ++cnt)
    {
        FIFO_Push(fifo, &cnt);
    }
    return;
}

int main(void)
{

    const struct CMUnitTest test_FIFO_New[] =
    {
        cmocka_unit_test(test_FIFO_New_Byte_Buffer),
    };

    const struct CMUnitTest test_FIFO_Push[] =
    {
        cmocka_unit_test(test_FIFO_Push_NULL_arguments),
        cmocka_unit_test(test_FIFO_Push_Empty),
        cmocka_unit_test(test_FIFO_Push_Full),
    };

    const struct CMUnitTest test_FIFO_Pop[] =
    {
        cmocka_unit_test(test_FIFO_Pop_NULL_arguments),
        cmocka_unit_test(test_FIFO_Pop_Empty),
        cmocka_unit_test(test_FIFO_Pop_NonEmpty),
    };

    const struct CMUnitTest test_FIFO_Peek[] =
    {
        cmocka_unit_test(test_FIFO_Peek_NULL_arguments),
        cmocka_unit_test(test_FIFO_Peek_Empty),
        cmocka_unit_test(test_FIFO_Peek_NonEmpty),
    };

    const struct CMUnitTest test_FIFO_IsEmpty[] =
    {
        cmocka_unit_test(test_FIFO_IsEmpty_NULL),
        cmocka_unit_test(test_FIFO_IsEmpty_Empty),
        cmocka_unit_test(test_FIFO_IsEmpty_Full),
        cmocka_unit_test(test_FIFO_IsEmpty_NotEmpty),
    };

    const struct CMUnitTest test_FIFO_IsFull[] =
    {
        cmocka_unit_test(test_FIFO_IsFull_NULL),
        cmocka_unit_test(test_FIFO_IsFull_Empty),
        cmocka_unit_test(test_FIFO_IsFull_Full),
        cmocka_unit_test(test_FIFO_IsFull_NotEmpty),
    };

    const struct CMUnitTest test_FIFO_Clear[] =
    {
        cmocka_unit_test(test_FIFO_Clear_NULL),
        cmocka_unit_test(test_FIFO_Clear_Full),
    };

    int result = 0;
    result += cmocka_run_group_tests(test_FIFO_New, NULL, NULL);
    result += cmocka_run_group_tests(test_FIFO_Push, NULL, NULL);
    result += cmocka_run_group_tests(test_FIFO_Pop, NULL, NULL);
    result += cmocka_run_group_tests(test_FIFO_Peek, NULL, NULL);
    result += cmocka_run_group_tests(test_FIFO_IsEmpty, NULL, NULL);
    result += cmocka_run_group_tests(test_FIFO_IsFull, NULL, NULL);
    result += cmocka_run_group_tests(test_FIFO_Clear, NULL, NULL);

    return result;
}
