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


#ifndef LIBMCP79510_H_
#define LIBMCP79510_H_

#include "common.h"

//Instructions
#define EEREAD	0x03	//Read data from EE memory array beginning at selected address
#define EEWRITE	0x02	//Write data to EE memory array beginning at selected address
#define EEWRDI	0x04	//Reset the write enable latch (disable write operations)
#define EEWREN	0x06	//Set the write enable latch (enable write operations)
#define SRREAD	0x05	//Read STATUS register
#define SRWRITE	0x01	//Write STATUS register
#define READ	0x13	//Read RTCC/SRAM array beginning at selected address
#define WRITE	0x12	//Write RTCC/SRAM data to memory array beginning at selected address
#define UNLOCK	0x14	//Unlock ID Locations
#define IDWRITE 0x32	//Write to the ID Locations
#define IDREAD	0x33	//Read the ID Locations
#define CLRRAM	0x54	//Clear RAM Location to ‘0’

//Time and configuration(TC) registers
#define TC_SEC_CENT		0x00	//Hundredths of seconds
#define TC_SEC			0x01	//Seconds
#define TC_MIN			0x02	//Minutes
#define TC_HOUR			0x03	//Hours
#define TC_DAY			0x04	//Day
#define TC_DATE			0x05	//Date
#define TC_MONTH		0x06	//Month
#define TC_YEAR			0x07	//Year
#define TC_CONTROL		0x08	//Control register
#define TC_CALIBRATION	0x09	//Calibration

//Alarm 0 Registers
#define ALARM0_SEC		0x0C
#define ALARM0_MIN		0x0D
#define ALARM0_HOUR		0x0E
#define ALARM0_DAY		0x0F
#define ALARM0_DATE		0x10
#define ALARM0_MONTH	0x11

//Alarm 1 Registers
#define ALARM1_SEC_CENT	0x12
#define ALARM1_SEC		0x13
#define ALARM1_MIN		0x14
#define ALARM1_HOUR		0x15
#define ALARM1_DAY		0x16
#define ALARM1_DATE		0x17

//Power-Down Time-Stamp Registers
#define PD_MIN		0x18
#define PD_HOUR		0x19
#define PD_DATE		0x1A
#define PD_MONTH	0x1B

//Power-Up Time-Stamp Registers
#define PU_MIN		0x1C
#define PU_HOUR		0x1D
#define PU_DATE		0x1E
#define PU_MONTH	0x1F

#endif /* LIBMCP79510_H_ */