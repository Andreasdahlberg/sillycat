/**
 * @file   Flash.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2016-03-15 (Last edit)
 * @brief  Header of flash module.
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


#ifndef FLASH_H_
#define FLASH_H_

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

typedef void (*flash_callback_type)(uint8_t context);

typedef struct
{
    uint32_t density;
    uint32_t sector_size;
} flash_sfdp_info_type;

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

void Flash_Init(void);
void Flash_Service(void);
bool Flash_Write(uint32_t address, void *data, size_t length,
                 flash_callback_type callback);
bool Flash_Read(uint32_t address, void *data, size_t length,
                flash_callback_type callback);
void Flash_EraseAll(void);
void Flash_EraseSector(void);
void Flash_Reset(void);
void Flash_DeepPowerDown(void);
bool Flash_IsIdle(void);

#endif /* FLASH_H_ */