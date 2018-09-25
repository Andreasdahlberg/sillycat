/**
 * @file   test_List.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-09-25 (Last edit)
 * @brief  Test suite for the List module.
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

#include "List.h"

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

//////////////////////////////////////////////////////////////////////////
//INTERUPT SERVICE ROUTINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TESTS
//////////////////////////////////////////////////////////////////////////

void test_List_New(void **state)
{
    struct list_t list = {0};
    const uint32_t type_ids[3] = {1, 0, 2};

    for (size_t i = 0; i < 3; ++i)
    {
        list = List_New(type_ids[i]);

        assert_int_equal(list.type_id, type_ids[i]);
        assert_null(list.root);
    }
}

void test_List_Append_NULL(void **state)
{
    struct list_t list;
    struct list_node_t node;

    expect_assert_failure(List_Append(NULL, NULL));
    expect_assert_failure(List_Append(&list, NULL));
    expect_assert_failure(List_Append(NULL, &node));
}

void test_List_Append(void **state)
{
    struct list_node_t nodes[3];
    struct list_t list = List_New(1);

    List_Append(&list, &nodes[0]);
    assert_ptr_equal(list.root, &nodes[0]);

    List_Append(&list, &nodes[1]);
    assert_ptr_equal(list.root, &nodes[0]);
    assert_ptr_equal(nodes[0].next, &nodes[1]);

    List_Append(&list, &nodes[2]);
    assert_ptr_equal(list.root, &nodes[0]);
    assert_ptr_equal(nodes[0].next, &nodes[1]);
    assert_ptr_equal(nodes[1].next, &nodes[2]);
}

void test_List_Remove_NULL(void **state)
{
    struct list_t list;
    struct list_node_t node;

    expect_assert_failure(List_Remove(NULL, NULL));
    expect_assert_failure(List_Remove(&list, NULL));
    expect_assert_failure(List_Remove(NULL, &node));
}

void test_List_Remove(void **state)
{
    struct list_node_t nodes[3];
    struct list_t list = List_New(1);

    List_Append(&list, &nodes[0]);
    List_Append(&list, &nodes[1]);
    List_Append(&list, &nodes[2]);

    /* Try to remove a node not in the list. */
    List_Remove(&list, &nodes[3]);
    assert_ptr_equal(list.root, &nodes[0]);
    assert_ptr_equal(nodes[0].next, &nodes[1]);
    assert_ptr_equal(nodes[1].next, &nodes[2]);

    List_Remove(&list, &nodes[1]);
    assert_ptr_equal(list.root, &nodes[0]);
    assert_ptr_equal(nodes[0].next, &nodes[2]);

    List_Remove(&list, &nodes[0]);
    assert_ptr_equal(list.root, &nodes[2]);

    List_Remove(&list, &nodes[2]);
    assert_null(list.root);
}

void test_List_GetLength(void **state)
{
    const size_t nr_nodes = 4;
    struct list_node_t nodes[nr_nodes];
    struct list_t list = List_New(1);

    for (size_t i = 0; i < nr_nodes; ++i)
    {
        assert_int_equal(List_GetLength(&list), i);
        List_Append(&list, &nodes[i]);
    }

    for (size_t i = 0; i < nr_nodes; ++i)
    {
        assert_int_equal(List_GetLength(&list), nr_nodes - i);
        List_Remove(&list, &nodes[i]);
    }
}

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test(test_List_New),
        cmocka_unit_test(test_List_Append_NULL),
        cmocka_unit_test(test_List_Append),
        cmocka_unit_test(test_List_Remove_NULL),
        cmocka_unit_test(test_List_Remove),
        cmocka_unit_test(test_List_GetLength)
    };

    if (argc >= 2)
    {
        cmocka_set_test_filter(argv[1]);
    }

    return cmocka_run_group_tests(tests, NULL, NULL);
}
