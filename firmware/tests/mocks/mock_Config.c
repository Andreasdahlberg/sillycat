/**
 * @file   mock_Config.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2020-10-21 (Last edit)
 * @brief  Mock functions for the configuration module.
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
#include "mock_Config.h"

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

uint8_t __wrap_Config_GetBroadcastAddress(void)
{
    mock_type(uint8_t);
}

bool __wrap_Config_Load(void)
{
    mock_type(bool);
}

void __wrap_Config_SetNetworkId(const uint8_t *network_id_p)
{
}

void __wrap_Config_Save(void)
{
}

uint32_t __wrap_Config_GetReportInterval(void)
{
    mock_type(uint32_t);
}

uint16_t __wrap_Config_GetVersion(void)
{
    mock_type(uint16_t);
}

void __wrap_Config_SetReportInterval(uint32_t report_interval)
{
}

uint8_t __wrap_Config_GetAddress(void)
{
    mock_type(uint8_t);
}

char __wrap_Config_GetAESKey(void)
{
    mock_type(char);
}

uint8_t __wrap_Config_GetMasterAddress(void)
{
    mock_type(uint8_t);
}

uint8_t __wrap_Config_GetNetworkId(void)
{
    mock_type(uint8_t);
}

void __wrap_Config_SetAESKey(const char *aes_key_p)
{
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////
