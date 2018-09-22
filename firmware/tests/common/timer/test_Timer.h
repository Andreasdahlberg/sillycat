/**
 * @file   test_Timer.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-09-22 (Last edit)
 * @brief  Test suite for the Timer module.
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

#ifndef TEST_TIMER_H_
#define TEST_TIMER_H_

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

void test_Timer_Init(void **state);
void test_Timer_Reset(void **state);
void test_Timer_GetMilliseconds(void **state);
void test_Timer_TimeDifference_zero(void **state);
void test_Timer_TimeDifference_nonzero(void **state);
void test_Timer_TimeDifference_wraparound(void **state);
void test_Timer_GetSeconds_exact(void **state);
void test_Timer_GetSeconds_ceil(void **state);
void test_Timer_GetSeconds_floor(void **state);

#endif
