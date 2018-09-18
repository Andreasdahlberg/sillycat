/**
 * @file   test_Interface.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-09-18 (Last edit)
 * @brief  Test suite for the Interface module.
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

#ifndef TEST_INTERFACE_H_
#define TEST_INTERFACE_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

void test_GetRootView_None(void **state);
void test_GetRootView_OneView(void **state);
void test_GetRootView_TwoViews(void **state);
void test_GetRootView_OneViewWithOneChild(void **state);
void test_AddView_NullWithRootView(void **state);
void test_AddView_NullWithoutRootView(void **state);
void test_AddView_OneView(void **state);
void test_AddView_SeveralViews(void **state);
void test_RemoveView_Null(void **state);
void test_RemoveView_RootViewAlone(void **state);
void test_RemoveView_RootViewWithOneSibling(void **state);
void test_RemoveView_RootViewWithChild(void **state);
void test_RemoveView_ViewWithPrevSibling(void **state);
void test_RemoveView_ViewWithPrevAndNextSibling(void **state);
void test_RemoveView_ChildViewWithSibling(void **state);
void test_AddChild_NullChildWithNoExistingChild(void **state);
void test_AddChild_NullChildWithExistingChild(void **state);
void test_AddChild_NullParentWithNoExistingChild(void **state);
void test_AddChild_OneChild(void **state);
void test_AddChild_TwoChildren(void **state);
void test_NextView_NoRootView(void **state);
void test_NextView_OneView(void **state);
void test_NextView_TwoViews(void **state);
void test_PreviousView_NoRootView(void **state);
void test_PreviousView_OneView(void **state);
void test_PreviousView_TwoViews(void **state);
void test_ActivateView_NoRootView(void **state);
void test_ActivateView_ActivateOnceWithNoChild(void **state);
void test_ActivateView_ActivateOnceWithChild(void **state);
void test_ActivateView_ActivateTwiceWithChild(void **state);
void test_Update_NoRootView(void **state);
void test_Update_ViewWithNoDrawFunction(void **state);
void test_Update_ViewWithDrawFunction(void **state);
void test_Update_AutoRefresh(void **state);
void test_Update_ForcedRefresh(void **state);

#endif
