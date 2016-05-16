/**
 * @file   libUART.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2016-05-16 (Last edit)
 * @brief  Implementation of libUART functions
 *
 * Detailed description of file.
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

//NOTE: Include before all other headers
#include "common.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include "libUART.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define DIV_MUL 16

#define CalculateUBRRn(baud) (((F_CPU * 10 / (DIV_MUL * baud) - 10) + 5) / 10)
#define CalculateBaudRate(ubrrn) (F_CPU / (DIV_MUL * (ubrrn + 1)))

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static libUART_isr_callback rx_handler;
static libUART_isr_callback tx_handler;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//INTERUPT SERVICE ROUTINES
//////////////////////////////////////////////////////////////////////////

ISR(USART_RX_vect)
{
    uint8_t data = UDR0;
    if (rx_handler != NULL)
    {
        rx_handler(&data);
    }
}

ISR(USART_UDRE_vect)
{
    uint8_t data;

    if (tx_handler != NULL && tx_handler(&data))
    {
        UDR0 = data;
    }
    else
    {
        UCSR0B &= ~(1<<UDRIE0);
    }
}

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

///
/// @brief Init UART
///
/// @param  None
/// @return None
///
void libUART_Init(void)
{
    //Set frame format: 1 stop bit 8 data
    UCSR0C =  (3 << UCSZ00);
    return;
}

///
/// @brief Set handler functions for Rx and Tx.
///
/// NULL can be used to remove a handler.
///
/// @param  rx_callback Pointer to rx handler.
/// @param  tx_callback Pointer to tx handler.
/// @return None
///
void libUART_SetCallbacks(libUART_isr_callback rx_callback,
                          libUART_isr_callback tx_callback)
{
    rx_handler = rx_callback;
    tx_handler = tx_callback;
    return;
}

///
/// @brief Enable/disable the UART interface.
///
/// @param  enable True to enable, otherwise false.
/// @return None
///
void libUART_Enable(bool enable)
{
    if (enable == true)
    {
        //PRR &= ~(1 << PRUSART0);
        UCSR0B |= ((1 << RXEN0) | (1 << TXEN0) | (1<<RXCIE0));
    }
    else
    {
        UCSR0B &= ~((1 << RXEN0) | (1 << TXEN0) | (1<<RXCIE0));
        //PRR |= (1 << PRUSART0);
    }
    return;
}

///
/// @brief Start a Tx transfer.
///
/// When a Tx transfer is started the Tx handler is called to get data.
///
/// @param  None
/// @return None
///
void libUART_StartTx(void)
{
    UCSR0B |= (1<<UDRIE0);
    return;
}

///
/// @brief Set baud rate.
///
/// NOTE: Remember to check the error rate with the selected baud rate
///       and the crystal frequency.
///
/// @param  baud Baud rate to set.
/// @return bool True if baud rate is valid, otherwise false.
///
bool libUART_SetBaudRate(uint32_t baud)
{
    bool status = false;
    uint32_t tmp = DIV_MUL * baud;

    //Check for overflow and invalid baud rate
    if (tmp <= F_CPU && tmp > baud)
    {
        uint16_t UBRRn = CalculateUBRRn(baud);
        UBRR0H = (uint8_t)(UBRRn >> 8);
        UBRR0L = (uint8_t)UBRRn;
        status = true;
    }

    return status;
}
