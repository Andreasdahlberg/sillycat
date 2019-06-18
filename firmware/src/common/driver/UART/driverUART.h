/**
 * @file   driverUART.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2019-06-18 (Last edit)
 * @brief  Implementation of ATmega328 UART driver.
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

#ifndef driverUART_H_
#define driverUART_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stdbool.h>

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

typedef bool (*driverUART_isr_callback)(uint8_t *data);

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

/**
 * Initialize the UART peripheral.
 */
void driverUART_Init(void);

/**
 * Set handler functions for Rx and Tx.
 *
 * @param rx_callback Pointer to rx handler.
 * @param tx_callback Pointer to tx handler.
 */
void driverUART_SetCallbacks(driverUART_isr_callback rx_callback,
                             driverUART_isr_callback tx_callback);

/**
 * Enable/disable the UART peripheral.
 *
 * @param enable True to enable, otherwise false.
 */
void driverUART_Enable(bool enable);

/**
 * Start a Tx transfer.
 *
 * When a Tx transfer is started the Tx handler is called to get data.
 */
void driverUART_StartTx(void);

/**
 * Set UART baud rate.
 *
 * Remember to check the error rate with the selected baud rate and
 * the crystal frequency.
 *
 * @param  baud Baud rate to set.
 *
 * @return True if baud rate is valid, otherwise false.
 */
bool driverUART_SetBaudRate(uint32_t baud);

#endif
