/**
 * @file   libDHT22.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2015-11-14 (Last edit)
 * @brief  Header of libDht22
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


#ifndef LIBDHT22_H_
#define LIBDHT22_H_

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

typedef struct
{
    float humidity;
    float temperature;
    bool status;
} dht22_data_type;

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

void libDHT22_Init(void);
void libDHT22_Update(void);
dht22_data_type libDHT22_GetSensorReading(void);
bool libDHT22_IsIdle(void);
void libDHT22_StartReading(void);
bool libDHT22_IsReadingValid(void);

#endif /* LIBDHT22_H_ */