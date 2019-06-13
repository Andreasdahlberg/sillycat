/**
 * @file   UART.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2019-06-13 (Last edit)
 * @brief  Implementation of UART module.
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

#ifndef UART_H_
#define UART_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include <stdint.h>

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

/**
 * Initialize the UART interface.
 */
void UART_Init(void);

/**
 * Enable/disable the UART interface.
 *
 * @param enable True to enable, otherwise false.
 */
void UART_Enable(bool enable);

/**
 * Write data to the UART interface.
 *
 * @param data_p Pointer to data.
 * @param length Number of bytes to write.
 *
 * @return Actual number of bytes written, a smaller number then the length
 *         means that the TX buffer is full.
 */
size_t UART_Write(const void *data_p, size_t length);

/**
 * Read data from the UART interface.
 *
 * @param data_p Pointer to buffer where data will be stored.
 * @param length Maximum number of bytes to read.
 *
 * @return Actual number of bytes read.
 */
size_t UART_Read(void *data_p, size_t length);

/**
 * Wait for TX to complete.
 *
 * @param timeout_ms Maximum time to wait, use 0 to disable the timeout.
 *
 * @return False if TX did not complete before the timeout, otherwise true.
 */
bool UART_WaitForTx(uint32_t timeout_ms);

#endif /* UART_H_ */
