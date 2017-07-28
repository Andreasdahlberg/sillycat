/**
 * @file   ADC.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2017-07-28 (Last edit)
 * @brief  Implementation of the ADC interface.
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

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include "common.h"

#include "libDebug.h"
#include "ADC.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

///
/// @brief Init the ADC interface.
///
/// @param  None
/// @return None
///
void ADC_Init(void)
{
    driverADC_Init();
    INFO("ADC module initialized");
    return;
}

///
/// @brief Initialize an ADC channel.
///
/// @param  channel Pointer to channel struct.
/// @param  index Index of the ADC channel.
///
/// @return None
///
void ADC_InitChannel(struct adc_channel_t *channel, uint8_t index)
{
    driverADC_InitChannel(channel, index, NULL);
    return;
}

///
/// @brief Perform an ADC conversion. Blocks until the conversion is done.
///
/// @param  channel Pointer to channel struct.
/// @param  samples Pointer to array where the samples will be stored.
/// @param  length Number of sample to convert.
///
/// @return None
///
void ADC_Convert(struct adc_channel_t *channel, uint16_t *samples, size_t length)
{
    driverADC_Convert(channel, samples, length);
    driverADC_Wait();
    return;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////
