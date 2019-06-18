/**
 * @file   driverUART.c
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

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include "common.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "driverUART.h"

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

static driverUART_isr_callback rx_handler;
static driverUART_isr_callback tx_handler;

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

void driverUART_Init(void)
{
    /**
     * Set frame format: 1 stop bit, 8 data bits.
     */
    UCSR0C =  (3 << UCSZ00);
}

void driverUART_SetCallbacks(driverUART_isr_callback rx_callback,
                             driverUART_isr_callback tx_callback)
{
    rx_handler = rx_callback;
    tx_handler = tx_callback;
}

void driverUART_Enable(bool enable)
{
    if (enable == true)
    {
        UCSR0B |= ((1 << RXEN0) | (1 << TXEN0) | (1<<RXCIE0));
    }
    else
    {
        UCSR0B &= ~((1 << RXEN0) | (1 << TXEN0) | (1<<RXCIE0));
    }
}

void driverUART_StartTx(void)
{
    UCSR0B |= (1<<UDRIE0);
}

bool driverUART_SetBaudRate(uint32_t baud)
{
    bool status = false;
    uint32_t tmp = DIV_MUL * baud;

    /**
     * Check for overflow and invalid baud rate.
     */
    if (tmp <= F_CPU && tmp > baud)
    {
        uint16_t UBRRn = CalculateUBRRn(baud);
        UBRR0H = (uint8_t)(UBRRn >> 8);
        UBRR0L = (uint8_t)UBRRn;
        status = true;
    }

    return status;
}
