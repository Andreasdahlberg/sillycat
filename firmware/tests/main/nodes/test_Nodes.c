/**
 * @file   test_Nodes.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2020-10-21 (Last edit)
 * @brief  Test suite for the nodes module.
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

#include "Nodes.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define MAX_NUMBER_OF_NODES 3

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

static int Setup(void **state)
{
    Nodes_Init();

    return 0;
}

//////////////////////////////////////////////////////////////////////////
//TESTS
//////////////////////////////////////////////////////////////////////////

static void test_Nodes_Add_NULL(void **state)
{
    expect_assert_failure(Nodes_Add(NULL));
}

static void test_Nodes_Add_Full(void **state)
{
    struct node_t dummy_node;

    /* Fill the node collection. */
    for (size_t i = 0; i < MAX_NUMBER_OF_NODES; ++i)
    {
        Nodes_Add(&dummy_node);
    }

    /* The node collection is full so we expect an assert failure. */
    expect_assert_failure(Nodes_Add(&dummy_node));
}

static void test_Nodes_GetNodeFromID_Empty(void **state)
{
    assert_null(Nodes_GetNodeFromID(0));
}

static void test_Nodes_GetNodeFromID_NoMatchingID(void **state)
{
    struct node_t dummy_node;

    for (size_t i = 0; i < MAX_NUMBER_OF_NODES; ++i)
    {
        Nodes_Add(&dummy_node);
    }

    will_return_always(__wrap_Node_GetID, 1);
    assert_null(Nodes_GetNodeFromID(2));
}

static void test_Nodes_GetNodeFromID(void **state)
{
    struct node_t dummy_nodes[MAX_NUMBER_OF_NODES];

    for (size_t i = 0; i < MAX_NUMBER_OF_NODES; ++i)
    {
        Nodes_Add(&dummy_nodes[i]);
        will_return(__wrap_Node_GetID, i);
    }

    const uint8_t node_id = MAX_NUMBER_OF_NODES - 1;
    assert_ptr_equal(Nodes_GetNodeFromID(node_id), &dummy_nodes[node_id]);
}

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    const struct CMUnitTest tests[] =
    {
        cmocka_unit_test_setup(test_Nodes_Add_NULL, Setup),
        cmocka_unit_test_setup(test_Nodes_Add_Full, Setup),
        cmocka_unit_test_setup(test_Nodes_GetNodeFromID_Empty, Setup),
        cmocka_unit_test_setup(test_Nodes_GetNodeFromID_NoMatchingID, Setup),
        cmocka_unit_test_setup(test_Nodes_GetNodeFromID, Setup)
    };

    if (argc >= 2)
    {
        cmocka_set_test_filter(argv[1]);
    }

    return cmocka_run_group_tests(tests, NULL, NULL);
}
