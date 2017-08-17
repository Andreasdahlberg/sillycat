/**
 * @file   libS25FL1K_reg.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2017-08-17 (Last edit)
 * @brief  Header with S25FL1-k registers and commands.
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

#ifndef LIBS25FL1K_REG_H_
#define LIBS25FL1K_REG_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define CMD_READ_SREG1                  0x05
#define CMD_READ_SREG2                  0x35
#define CMD_READ_SREG3                  0x33
#define CMD_WRITE_ENABLE                0x06
#define CMD_WRITE_ENABLE_VSREG          0x50
#define CMD_WRITE_DISABLE               0x04
#define CMD_WRITE_SREGS                 0x01
#define CMD_SET_BURST                   0x77
#define CMD_SET_BLOCK_PTR_PROTECTION    0x39
#define CMD_PAGE_PROGRAM                0x02
#define CMD_SECTOR_ERASE                0x20
#define CMD_BLOCK_ERASE                 0xD8
#define CMD_CHIP_ERASE                  0xC7 //TODO: Check why both 0xC7 and 0x60 is listed in datasheet
#define CMD_ERASE_PROGRAM_SUSPEND       0x75
#define CMD_ERASE_PROGRAM_RESUME        0x75
#define CMD_READ_DATA                   0x03
#define CMD_FAST_READ                   0x0B
#define CMD_FAST_READ_DUAL_OUTPUT       0x3B
#define CMD_FAST_READ_QUAD_OUTPUT       0x6B
#define CMD_FAST_READ_DUAL_IO           0xBB
#define CMD_FAST_READ_QUAD_IO           0xEB
#define CMD_CONT_READ_MODE_RESET        0xFF
#define CMD_DEEP_POWER_DOWN             0xB9
#define CMD_RELEASE_POWER_DOWN          0xAB //TODO: id until CS is released, check in datasheet!
#define CMD_MANUFACTURER_DEVICE_ID      0x90
#define CMD_JEDEC_ID                    0x9F
#define CMD_READ_SFDP_REG               0x5A //TODO: Also unique id number
#define CMD_READ_SECURITY_REGISTER      0x48
#define CMD_ERASE_SECURITY_REGISTER     0x44
#define CMD_PROGRAM_SECURITY_REGISTERS  0x42
#define CMD_SW_RESET_ENABLE             0x66
#define CMD_SW_RESET                    0x99

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

#endif
