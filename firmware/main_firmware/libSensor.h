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


#ifndef LIBSENSOR_H_
#define LIBSENSOR_H_

#define LIBSENSOR_EXTERNAL_LIGHT		0x06
#define LIBSENSOR_EXTERNAL_TEMPERATURE	0x07
#define LIBSENSOR_INTERNAL_TEMPERATURE	0x08

void libSensor_Init();
function_status libSensor_GetSensorValue(uint8_t sensor, uint16_t *sensor_value);

#endif /* LIBSENSOR_H_ */