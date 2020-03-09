/**
 * @file   UART.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2020-03-09 (Last edit)
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

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include "common.h"
#include <avr/interrupt.h>
#include "driverUART.h"
#include "FIFO.h"
#include "Timer.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define BAUDRATE 250000

#define TX_BUFFER_SIZE 128
#define RX_BUFFER_SIZE 8

#define InitTXBuffer() uart_tx_fifo = FIFO_New(tx_buffer)
#define InitRXBuffer() uart_rx_fifo = FIFO_New(rx_buffer)

#define AddToTXBuffer(data_p) FIFO_Push(&uart_tx_fifo, data_p)
#define GetFromRXBuffer(data_p) FIFO_Pop(&uart_rx_fifo, data_p)

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static uint8_t tx_buffer[TX_BUFFER_SIZE];
static uint8_t rx_buffer[RX_BUFFER_SIZE];

static fifo_type uart_tx_fifo;
static fifo_type uart_rx_fifo;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

static bool AddToRXBuffer(uint8_t *data_p);
static bool GetFromTXBuffer(uint8_t *data_p);

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void UART_Init(void)
{
    InitTXBuffer();
    InitRXBuffer();

    driverUART_Init();
    driverUART_SetBaudRate(BAUDRATE);
    driverUART_SetCallbacks(AddToRXBuffer, GetFromTXBuffer);
}

void UART_Enable(bool enable)
{
    driverUART_Enable(enable);
}

size_t UART_Write(const void *data_p, size_t length)
{
    size_t cnt = 0;
    const uint8_t *data_ptr = (const uint8_t *)data_p;

    while (cnt < length)
    {
        cli();
        if (AddToTXBuffer(data_ptr++) == false)
        {
            sei();
            break;
        }
        sei();

        ++cnt;
    }

    driverUART_StartTx();
    return cnt;
}

size_t UART_Read(void *data_p, size_t length)
{
    size_t cnt = 0;
    uint8_t *data_ptr = (uint8_t *)data_p;

    while (cnt < length)
    {
        cli();
        if (GetFromRXBuffer(data_ptr++) == false)
        {
            sei();
            break;
        }
        sei();

        ++cnt;
    }
    return cnt;
}

bool UART_WaitForTx(uint32_t timeout_ms)
{
    bool status = true;
    uint32_t start_time_ms = Timer_GetMilliseconds();

    while (!FIFO_IsEmpty(&uart_tx_fifo))
    {
        if (timeout_ms != 0 && Timer_TimeDifference(start_time_ms) > timeout_ms)
        {
            status = false;
            break;
        }
    }
    return status;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static bool AddToRXBuffer(uint8_t *data_p)
{
    bool status;

    status = FIFO_Push(&uart_rx_fifo, data_p);
    return status;
}

static bool GetFromTXBuffer(uint8_t *data_p)
{
    bool status;

    status = FIFO_Pop(&uart_tx_fifo, data_p);
    return status;
}
