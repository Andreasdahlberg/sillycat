/**
 * @file   guiSensor.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-09-01 (Last edit)
 * @brief  Header of guiSensor
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

#ifndef GUISENSOR_H_
#define GUISENSOR_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

void guiSensor_Init(void);
void guiSensor_DrawDetailedTemperatureView(uint16_t context __attribute__ ((unused)));
void guiSensor_DrawTemperatureView(uint16_t context __attribute__ ((unused)));

#endif
