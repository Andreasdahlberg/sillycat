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

#ifndef LIBSPI_H_
#define LIBSPI_H_

#include <avr/io.h>
#include "common.h"

typedef enum SPI_status
{
	IDLE = 0,
	READ,
	READ_DONE,
	WRITE,
	WRITE_DONE,
	
}SPI_status;


void libSPI_Init(uint8_t spi_mode);
void libSPI_Update(void);
void libSPI_SetMaster(bool master_value);
SPI_status libSPI_GetStatus(void);
bool libSPI_SetMode(uint8_t mode);
bool SetActiveSlave(bool state);
bool libSPI_Write(const uint8_t* data_bytes, uint8_t length);
bool libSPI_Read(const uint8_t* data_bytes, uint8_t length);

void libSPI_WriteByte(uint8_t data_byte);
void libSPI_ReadByte(uint8_t *data_byte);


#endif /* LIBSPI_H_ */