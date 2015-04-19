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


#ifndef LIBLED_H_
#define LIBLED_H_

#define STATUS_LED PC5

typedef enum LEDStatus
{
	LED_OFF = 0,
	LED_ON
}LEDStatus;

void libLED_Init();
void libLED_Update();
void libLED_Toggle();



#endif /* LIBLED_H_ */