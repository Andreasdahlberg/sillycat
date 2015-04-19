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


#ifndef LIBDS3234_H_
#define LIBDS3234_H_

#include "common.h"

//Read registers
#define REG_SECONDS				0x00
#define REG_MINUTES				0x01
#define REG_HOUR				0x02
#define REG_DAY					0x03
#define REG_DATE				0x04
#define REG_MONTH_CENTURY		0x05
#define REG_YEAR				0x06
#define REG_ALARM_1_SECONDS		0x07
#define REG_ALARM_1_MINUTES		0x08
#define REG_ALARM_1_HOURS		0x09
#define REG_ALARM_1_DAY_DATE	0x0A
#define REG_ALARM_2_MINUTES		0x0B
#define REG_ALARM_2_HOURS		0x0C
#define REG_ALARM_2_DAY_DATE	0x0D
#define REG_CONTROL				0x0E
#define REG_CONTROL_STATUS		0x0F
#define REG_AGING_OFFSET		0x10
#define REG_TEMP_MSB			0x11
#define REG_TEMP_LSB			0x12
#define REG_DISABLE_TEMP		0x13
#define REG_SRAM_ADDRESS		0x18
#define REG_SRAM_DATA			0x19

#define READ_ADDRESS	0x00
#define WRITE_ADDRESS	0x80

#define MONTH_MASK		0x1F

void libDS3234_Init();
void libDS3234_Update();
void libDS3234_Test();

void libDS3234_GetTemperature(uint16_t *temperature);
bool libDS3234_GetDate(uint8_t *date);
bool libDS3234_GetMinutes(uint8_t *minutes);
bool libDS3234_GetSeconds(uint8_t *seconds);

#endif /* LIBDS3234_H_ */