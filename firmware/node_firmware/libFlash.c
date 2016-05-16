/**
 * @file   libFlash.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2016-05-01 (Last edit)
 * @brief  Implementation of low level flash interface.
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
#include "libSPI.h"
#include "libFlash.h"

#include "Flash_HAL.h"

#include "Util/delay.h"

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

static void PreCallback(void);
static void PostCallback(void);

//////////////////////////////////////////////////////////////////////////
//INTERUPT SERVICE ROUTINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

///
/// @brief Init the flash hardware.
///
/// Enable the flash hardware by releasing CS and pulling WP.
///
/// @param  None
/// @return None
///
void libFlash_InitHW(void)
{
    InitCS();
    ReleaseCS();

    InitWP();
    PullWP();

    return;
}

void libFlash_Init(void)
{
    libFlash_Test();
}

void libFlash_Test(void)
{
    ifcflash_id_type id;
    libFlash_GetId(&id);



    uint8_t reg1;
    uint8_t reg2;
    uint8_t reg3;

    libSPI_WriteByte(CMD_READ_SREG1, PostCallback, NULL);
    libSPI_ReadByte(&reg1, NULL, PostCallback);

    libSPI_WriteByte(CMD_READ_SREG2, PostCallback, NULL);
    libSPI_ReadByte(&reg2, NULL, PostCallback);

    libSPI_WriteByte(CMD_READ_SREG3, PostCallback, NULL);
    libSPI_ReadByte(&reg3, NULL, PostCallback);

    DEBUG("SREG1: 0x%02x\r\n", reg1);
    DEBUG("SREG2: 0x%02x\r\n", reg2);
    DEBUG("SREG3: 0x%02x\r\n", reg3);

    return;
}

void libFlash_GetId(ifcflash_id_type *id)
{
    libSPI_WriteByte(CMD_JEDEC_ID, PreCallback, NULL);
    libSPI_Read(id, sizeof(ifcflash_id_type), NULL, PostCallback);

    DEBUG("Manufacturer: 0x%02X\r\n", id->manufacturer);
    DEBUG("Memory type: 0x%02X\r\n", id->memory_type);
    DEBUG("Capacity: 0x%02X\r\n", id->capacity);

    return;
}



bool libFlash_EraseSector(uint32_t sector_index)
{
    //TODO: Validate sector address here!

    libSPI_WriteByte(CMD_WRITE_ENABLE, PreCallback, PostCallback);

    libSPI_WriteByte(CMD_SECTOR_ERASE, PreCallback, NULL);

    uint32_t sector_address;

    uint8_t *address_ptr = &sector_address;
    libSPI_WriteByte(0, NULL, NULL);
    libSPI_WriteByte(0, NULL, NULL);
    libSPI_WriteByte(0, NULL, PostCallback);

    //NOTE: Write protection is automatically reset after completion of
    //      a sector erase.
    return false;
}

///
/// @brief Restore the device to its initial power up state.
///
/// If a software reset is initiated during a erase, program or writing of a
/// register operation the data in that sector, page or register is not stable
/// and the operation that was interrupted needs to be initiated again.
/// When the device is in deep power-down mode it is protected from a software
/// reset, the software reset commands are ignored and have no effect. To reset
/// the device release it from power down first.
///
/// @param  None
/// @return None
///
void libFlash_Reset(void)
{
    uint8_t cmd_sequence[] = {CMD_SW_RESET_ENABLE, CMD_SW_RESET};

    libSPI_Write(cmd_sequence, sizeof(cmd_sequence), PreCallback, PostCallback);
    return;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

///
/// @brief Set correct SPI-mode and select device.
///
/// @param  None
/// @return None
///
static void PreCallback(void)
{
    libSPI_SetMode(SPIMODE);
    PullCS();
    return;
}

///
/// @brief Deselect device.
///
/// Pulls SS high so the device is deselected. The SPI-mode is left as it is.
///
/// @param  None
/// @return None
///
static void PostCallback(void)
{
    ReleaseCS();
    return;
}