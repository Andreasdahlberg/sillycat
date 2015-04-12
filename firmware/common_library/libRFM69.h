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

typedef enum
{
	RFM_SLEEP = 0,
	RFM_STANDBY,
	RFM_SYNTHESIZER,
	RFM_TRANSMITTER,
	RFM_RECEIVER
}libRFM69_mode_type;

void libRFM69_Init();
void libRFM69_Update();
void libRFM69_Test();
bool libRFM69_SetMode(libRFM69_mode_type mode);
void libRFM69_EnableEncryption(bool enable);
bool libRFM69_IsModeReady();
void libRFM69_WaitForModeReady();
void libRFM69_SetCarrierFrequency(uint32_t frequency);
void libRFM69_EnableSequencer(bool enable);

#endif /* LIBRFM69_H_ */