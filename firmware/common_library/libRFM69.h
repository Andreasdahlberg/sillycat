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


#ifndef LIBRFM69_H_
#define LIBRFM69_H_

#define READ_REG	0x7F
#define WRITE_REG	0x80

void libRFM69_Init();
void libRFM69_Update();
void libRFM69_Test();


typedef enum
{
	SLEEP = 0,
	STANDBY,
	SYNTHESIZER,
	TRANSMITTER,
	RECEIVER
}libRFM69_mode_type;



#endif /* LIBRFM69_H_ */