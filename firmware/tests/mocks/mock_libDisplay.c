/**
 * @file   mock_libDisplay.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-09-22 (Last edit)
 * @brief  Mock functions for libDisplay module.
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
#include "mock_libDisplay.h"

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
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void __wrap_libDisplay_Init(void)
{
    function_called();
}

void __wrap_libDisplay_On(void)
{
    function_called();
}

void __wrap_libDisplay_Off(void)
{
    function_called();
}

void __wrap_libDisplay_Clear(void)
{
    function_called();
}

void __wrap_libDisplay_ClearVRAM(void)
{
    function_called();
}

void __wrap_libDisplay_Reset(void)
{
    function_called();
}

void __wrap_libDisplay_Flush(void)
{
    function_called();
}

void __wrap_libDisplay_SetPixel(uint8_t x, uint8_t y)
{
    check_expected(x);
    check_expected(y);
}

void __wrap_libDisplay_SetBrightness(uint8_t brightness)
{
    check_expected(brightness);
}

void __wrap_libDisplay_Rotate(bool state)
{
    check_expected(state);
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////
