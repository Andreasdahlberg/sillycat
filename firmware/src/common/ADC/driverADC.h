/**
 * @file   driverADC.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2017-07-28 (Last edit)
 * @brief  Header of ADC-library.
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

#ifndef DRIVERADC_H_
#define DRIVERADC_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

typedef void (*driver_adc_callback_t)(uint8_t index);

struct adc_channel_t
{
    uint8_t index;
    driver_adc_callback_t callback;
};

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

void driverADC_Init(void);
void driverADC_InitChannel(struct adc_channel_t *channel,
                           uint8_t index,
                           driver_adc_callback_t callback);
void driverADC_Convert(struct adc_channel_t *channel, uint16_t *samples, size_t length);
void driverADC_Wait();

#endif
