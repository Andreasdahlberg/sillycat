/**
 * @file   mock_libUART.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2019-06-13 (Last edit)
 * @brief  Mock functions for the UART module.
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

#ifndef WRAP_LIBUART_H_
#define WRAP_LIBUART_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include <stdbool.h>
#include "libUART.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

void __wrap_libUART_Init(void) __attribute__((weak));
void __wrap_libUART_SetCallbacks(libUART_isr_callback rx_callback, libUART_isr_callback tx_callback) __attribute__((weak));
void __wrap_libUART_Enable(bool enable) __attribute__((weak));
void __wrap_libUART_StartTx(void) __attribute__((weak));
bool __wrap_libUART_SetBaudRate(uint32_t baud) __attribute__((weak));

#endif
