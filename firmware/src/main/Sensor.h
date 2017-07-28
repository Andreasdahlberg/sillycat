/**
 * @file   Sensor.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2017-07-28 (Last edit)
 * @brief  Header of Sensor module
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

#ifndef SENSOR_H_
#define SENSOR_H_

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define SENSOR_EXTERNAL_TEMPERATURE 0x07
#define SENSOR_INTERNAL_TEMPERATURE 0x08

#define AVERAGE_WINDOW_S    300    //5 min
#define SAMPLE_FREQUENCY_MS 5000   //5 sec

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//Using packed attribute since the struct will be stored to SRAM
typedef struct __attribute__((packed))
{
    uint16_t header;
    uint16_t max;
    uint16_t min;
    uint16_t value;
    uint16_t average;
}
sensor_sample_type;

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

void Sensor_Init(void);
void Sensor_Update(void);
void Sensor_GetSensorValue(uint8_t sensor, uint16_t *sensor_value);
bool Sensor_GetReading(uint8_t sensor, sensor_sample_type *reading);
bool Sensor_SaveReading(uint8_t sensor, sensor_sample_type *reading);

#endif
