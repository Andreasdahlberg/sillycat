/**
 * @file   libMCP79510.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2016-02-27 (Last edit)
 * @brief  Header of libMCP79510
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

#ifndef LIBMCP79510_H_
#define LIBMCP79510_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

void libMCP79510_Init(void);
void libMCP79510_HWInit(void);
void libMCP79510_GetHundredthSecond(uint8_t *hsec);
void libMCP79510_GetSecond(uint8_t *sec);
void libMCP79510_GetMinute(uint8_t *minute);
void libMCP79510_GetHour(uint8_t *hour);
void libMCP79510_GetDay(uint8_t *day);
void libMCP79510_GetDate(uint8_t *date);
void libMCP79510_GetYear(uint8_t *year);
void libMCP79510_Enable24HourMode(bool enabled);
bool libMCP79510_Is24HourMode(void);
void libMCP79510_EnableOscillator(bool enabled);
bool libMCP79510_WriteToSRAM(uint8_t address, const uint8_t *data,
                             uint8_t length);
bool libMCP79510_ReadFromSRAM(uint8_t address, uint8_t *data, uint8_t length);

#endif /* LIBMCP79510_H_ */