/**
 * @file   Flash.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2016-03-15 (Last edit)
 * @brief  Implementation of flash interface.
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

//NOTE: Include common.h before all other headers
#include "common.h"

#include "libDebug.h"
#include "Flash.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

typedef struct
{
    uint32_t address;
    void *data;
    size_t size;
    bool status;
    flash_callback_type callback;

} flash_operation_type;

typedef enum
{
    FLASH_IDLE = 0,
    FLASH_WRITE_READY,
    FLASH_WRITE_WAITING,
    FLASH_WRITE_DONE
} flash_status_type;

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static flash_operation_type flash_transfer;
static flash_status_type flash_status;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//INTERUPT SERVICE ROUTINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

///
/// @brief Init flash.
///
/// Init flash hardware and reset flash transfer status.
///
/// @param  None
/// @return None
///
void Flash_Init(void)
{
    //NOTE: Hardware should already be configured here but InitHW is
    //      called here to make sure proper state after a warm reset
    //      of the flash module.
    libFlash_InitHW();

    flash_transfer = (flash_operation_type) {0};
    flash_status = FLASH_IDLE;

    //TODO: Read JEDEC/SFDP data here.
    //TODO: Check flash status register here.
    return;
}

///
/// @brief Init flash hardware.
///
/// Enable flash hardware by releasing CS.
///
/// @param  None
/// @return None
///
void Flash_InitHW(void)
{
    //TODO: Implement!
    return;
}

///
/// @brief Update the internal state of the flash module.
///
///
/// @param  None
/// @return None
///
void Flash_Service(void)
{
    switch (flash_status)
    {
        case FLASH_IDLE:
            // do nothing here
            break;

        case FLASH_WRITE_READY:
            break;

        case FLASH_WRITE_WAITING:
            break;

        case FLASH_WRITE_DONE:
            break;

        default:
            sc_assert_fail();
            break;
    }
    return;
}

///
/// @brief Write data asynchronous to flash.
///
/// @param  address Address to write to.
/// @param  *data Pointer to data.
/// @param  length Length of data.
/// @param  callback Pointer to function to be called after the write is complete.
///                  Null can be used if no callback is needed.
/// @return bool True if write is started, otherwise false.
///
bool Flash_Write(uint32_t address, void *data, size_t length,
                 flash_callback_type callback)
{
    sc_assert(data != NULL);

    if (!Flash_IsIdle())
    {
        return false;
    }

    //Configure transfer
    flash_transfer.address = address;
    flash_transfer.data = data;
    flash_transfer.size = length;
    flash_transfer.callback = callback;
    flash_transfer.status = false;

    flash_status = FLASH_WRITE_READY;
    return true;
}

bool Flash_Read(uint32_t address, void *data, size_t length,
                flash_callback_type callback)
{
    sc_assert(data != NULL);

    return false;
}

void Flash_EraseAll(void)
{
    return;
}

void Flash_EraseSector(void)
{
    return;
}

void Flash_Reset(void)
{
    return;
}

void Flash_DeepPowerDown(void)
{
    return;
}

///
/// @brief Check if flash is idle.
///
///
/// @param  None
/// @return True if idle, otherwise false.
///
bool Flash_IsIdle(void)
{
    return flash_status == FLASH_IDLE;
}


//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////