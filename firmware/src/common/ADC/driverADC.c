/**
 * @file   driverADC.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2020-10-05 (Last edit)
 * @brief  Implementation of low a level ADC driver.
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

//NOTE: Include before all other headers
#include "common.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include "driverADC.h"
#include "libDebug.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define MAX_ADC_CHANNEL_INDEX 8

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

struct driver_adc_t
{
    struct adc_channel_t *channel;
    uint16_t *buffer;
    size_t length;
    size_t index;
};

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static struct driver_adc_t module;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

ISR(ADC_vect)
{
    module.buffer[module.index] = ADCL | (ADCH << 8);
    ++module.index;

    if (module.length == module.index)
    {
        // Disable the ADC.
        ADCSRA &= ~(1 << ADEN);

        if (module.channel->callback != NULL)
        {
            module.channel->callback((uint8_t)module.index);
        }
    }
}

//////////////////////////////////////////////////////////////////////////
//INTERUPT SERVICE ROUTINES
//////////////////////////////////////////////////////////////////////////

static inline void EnableADC(void);
static inline void StartConversion(void);
static inline void SelectChannel(uint8_t index);

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

///
/// @brief Init the ADC hardware and the driver state.
///
/// @param  None
/// @return None
///
void driverADC_Init(void)
{
    module.channel = NULL;
    module.buffer = NULL;
    module.length = 0;
    module.index = 0;

    // Enable auto trigger, enable interrupts and set the prescaler to 128(115 KHz).
    ADCSRA = ((1 << ADATE) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0));

    // Set the reference voltage to AREF.
    ADMUX = 0x00;
}

///
/// @brief Initialize an ADC channel.
///
/// @param  channel Pointer to channel struct.
/// @param  index Index of the ADC channel.
/// @param  callback This function will be called after an conversion has completed.
///                  The callback is run from an ISR.
///
/// @return None
///
void driverADC_InitChannel(struct adc_channel_t *channel,
                           uint8_t index,
                           driver_adc_callback_t callback)
{
    sc_assert(channel != NULL);
    sc_assert(index <= MAX_ADC_CHANNEL_INDEX);

    channel->index = index;
    channel->callback = callback;
}

///
/// @brief Start an ADC conversion.
///
/// @param  channel Pointer to channel struct.
/// @param  samples Pointer to array where the samples will be stored.
/// @param  length Number of sample to convert.
///
/// @return None
///
void driverADC_Convert(struct adc_channel_t *channel, uint16_t *samples, size_t length)
{
    sc_assert(channel != NULL);
    sc_assert(samples != NULL);
    sc_assert(length > 0);

    module.channel = channel;
    module.buffer = samples;
    module.length = length;
    module.index = 0;

    SelectChannel(channel->index);
    EnableADC();
    StartConversion();
}

///
/// @brief Wait for an ADC conversion to finish.
///
/// @param  None
///
/// @return None
///
void driverADC_Wait(void)
{
    while (ADCSRA & (1 << ADEN))
    {
        /* Wait for conversion to finish. */
    }
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static inline void EnableADC(void)
{
    ADCSRA |= (1 << ADEN);
}

static inline void StartConversion(void)
{
    ADCSRA |= (1 << ADSC);
}

static inline void SelectChannel(uint8_t index)
{
    uint8_t new_admux;

    new_admux = ADMUX;
    //Clear MUX-bits
    new_admux &= 0xF0;

    //Set new channel
    new_admux |= index;
    ADMUX = new_admux;
}
