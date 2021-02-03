/**
 * @file   mock_driverNHD223.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2021-02-03 (Last edit)
 * @brief  Mock functions for the NHD2.23 display driver.
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
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void __wrap_driverNHD223_Init(void)
{
    function_called();
}

void __wrap_driverNHD223_WriteCommand(uint8_t command)
{
    check_expected(command);
}

void __wrap_driverNHD223_WriteData(uint8_t data)
{
    check_expected(data);
}

void __wrap_driverNHD223_ResetDisplay(void)
{
    function_called();
}

void __wrap_driverNHD223_SetHorizontalAddressingMode(void)
{
    function_called();
}

void __wrap_driverNHD223_SetPageAddress(uint8_t address)
{
    check_expected(address);
}

void __wrap_driverNHD223_SetPageAddressRange(uint8_t start_address, uint8_t end_address)
{
    check_expected(start_address);
    check_expected(end_address);
}

void __wrap_driverNHD223_SetColumnAddress(uint8_t address)
{
    check_expected(address);
}

void __wrap_driverNHD223_SetColumnAddressRange(uint8_t start_address, uint8_t end_address)
{
    check_expected(start_address);
    check_expected(end_address);
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////
