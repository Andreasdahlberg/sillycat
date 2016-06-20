/**
 * @file   UART.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2016-06-20 (Last edit)
 * @brief  Implementation of UART module.
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

#include <avr/interrupt.h>

#include "libUART.h"
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

#define AddToTXBuffer(data) FIFO_Push(&uart_tx_fifo, data)
#define GetFromRXBuffer(data) FIFO_Pop(&uart_rx_fifo, data)

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

bool AddToRXBuffer(uint8_t *data);
bool GetFromTXBuffer(uint8_t *data);

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

///
/// @brief Init the UART interface.
///
/// NOTE: UART is not active until enabled.
///
/// @param  None
/// @return None
///
void UART_Init(void)
{
    InitTXBuffer();
    InitRXBuffer();

    libUART_Init();
    libUART_SetBaudRate(BAUDRATE);
    libUART_SetCallbacks(AddToRXBuffer, GetFromTXBuffer);
    return;
}

///
/// @brief Enable/disable the UART interface.
///
/// @param  enable True to enable, otherwise false.
/// @return None
///
void UART_Enable(bool enable)
{
    libUART_Enable(enable);
}

///
/// @brief Write data to UART.
///
///
/// @param  *data Pointer to data.
/// @param  length Number of bytes to write.
/// @return Actual number of bytes written, a smaller number then length means
///         that the tx buffer is full.
///
size_t UART_Write(const void *data, size_t length)
{
    size_t cnt = 0;
    uint8_t *data_ptr = (uint8_t *)data;

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

    libUART_StartTx();
    return cnt;
}

///
/// @brief Read data from UART.
///
///
/// @param  *data Pointer to buffer where data will be stored.
/// @param  length Max number of bytes to read.
/// @return Actual number of bytes read.
///
size_t UART_Read(void *data, size_t length)
{
    size_t cnt = 0;
    uint8_t *data_ptr = (uint8_t *)data;

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

///
/// @brief Wait for Tx to complete.
///
///
/// @param  *timeout_ms Maximum time to wait, use 0 to disable the timeout.
/// @return False if Tx did not complete before the timeout, otherwise true.
///
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

bool AddToRXBuffer(uint8_t *data)
{
    bool status = true;

    if(FIFO_Push(&uart_rx_fifo, data) == false)
    {
        //TODO: place warning here
        status = false;
    }
    return status;
}

bool GetFromTXBuffer(uint8_t *data)
{
    bool status = true;

    if(FIFO_Pop(&uart_tx_fifo, data) == false)
    {
        //TODO: place warning here
        status = false;
    }
    return status;
}