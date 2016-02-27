/**
 * @file   MCP79510Registers.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2016-02-27 (Last edit)
 * @brief  Registers and bit defines for MCP79510.
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

#ifndef MCP79510REGISTERS_H_
#define MCP79510REGISTERS_H_

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//Instructions
#define INST_EEREAD  0x03    //Read data from EE memory array beginning at selected address
#define INST_EEWRITE 0x02    //Write data to EE memory array beginning at selected address
#define INST_EEWRDI  0x04    //Reset the write enable latch (disable write operations)
#define INST_EEWREN  0x06    //Set the write enable latch (enable write operations)
#define INST_SRREAD  0x05    //Read STATUS register
#define INST_SRWRITE 0x01    //Write STATUS register
#define INST_READ    0x13    //Read RTCC/SRAM array beginning at selected address
#define INST_WRITE   0x12    //Write RTCC/SRAM data to memory array beginning at selected address
#define INST_UNLOCK  0x14    //Unlock ID Locations
#define INST_IDWRITE 0x32    //Write to the ID Locations
#define INST_IDREAD  0x33    //Read the ID Locations
#define INST_CLRRAM  0x54    //Clear RAM Location to ‘0’

//Time and configuration(TC) registers
#define REG_TC_SEC_CENT     0x00    //Hundredths of seconds
#define REG_TC_SEC          0x01    //Seconds
#define REG_TC_MIN          0x02    //Minutes
#define REG_TC_HOUR         0x03    //Hours
#define REG_TC_DAY          0x04    //Day
#define REG_TC_DATE         0x05    //Date
#define REG_TC_MONTH        0x06    //Month
#define REG_TC_YEAR         0x07    //Year
#define REG_TC_CONTROL      0x08    //Control register
#define REG_TC_CALIBRATION  0x09    //Calibration

//Alarm 0 Registers
#define REG_ALARM0_SEC      0x0C
#define REG_ALARM0_MIN      0x0D
#define REG_ALARM0_HOUR     0x0E
#define REG_ALARM0_DAY      0x0F
#define REG_ALARM0_DATE     0x10
#define REG_ALARM0_MONTH    0x11

//Alarm 1 Registers
#define REG_ALARM1_SEC_CENT 0x12
#define REG_ALARM1_SEC      0x13
#define REG_ALARM1_MIN      0x14
#define REG_ALARM1_HOUR     0x15
#define REG_ALARM1_DAY      0x16
#define REG_ALARM1_DATE     0x17

//Power-Down Time-Stamp Registers
#define REG_PD_MIN      0x18
#define REG_PD_HOUR     0x19
#define REG_PD_DATE     0x1A
#define REG_PD_MONTH    0x1B

//Power-Up Time-Stamp Registers
#define REG_PU_MIN      0x1C
#define REG_PU_HOUR     0x1D
#define REG_PU_DATE     0x1E
#define REG_PU_MONTH    0x1F

#define REG_TC_SEC_OSC_BIT 0x07
#define REG_TC_HOUR_MODE_BIT 0x06

#endif /* MCP79510REGISTERS_H_ */