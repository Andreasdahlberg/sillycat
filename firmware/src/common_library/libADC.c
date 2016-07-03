/**
 * @file   libADC.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2016-07-06 (Last edit)
 * @brief  Implementation of ADC-library.
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

#include "libADC.h"
#include "libDebug.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define MAX_ADC_INPUTS 9    //Eight external inputs and one internal temperature sensor

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

typedef enum ADCState
{
    LIBADC_IDLE = 0,
    LIBADC_NEW_SAMPLE,
    LIBADC_SAMPLING
} ADCState;

typedef struct
{
    bool active;
    bool valid;
    uint8_t channel_index;
    uint16_t sample_value;
} adc_input_type;

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static ADCState adc_state;
static adc_input_type adc_inputs[MAX_ADC_INPUTS];

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

static void InitInputArray();
static void SelectInput(uint8_t adc_channel);

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

///
/// @brief Init libADC, all inputs are disabled after init.
///
/// @param  None
/// @return None
///
void libADC_Init(void)
{
    //Set the prescaler to 128(115 KHz) and enable interrupt
    ADCSRA |= ((1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0));

    //Set the reference voltage to AREF
    ADMUX = 0x00;

    InitInputArray();
    adc_state = LIBADC_IDLE;

    INFO("Init done");
}

///
/// @brief Update the internal library state, this selects the next active input and starts sampling.
///        Run as fast as possible.
///
/// @param  None
/// @return None
///
void libADC_Update(void)
{
    static uint8_t current_input = 0;

    if (current_input >= MAX_ADC_INPUTS)
    {
        current_input = 0;
    }

    switch (adc_state)
    {
        case LIBADC_IDLE:
            //Do nothing when idle
            break;

        case LIBADC_NEW_SAMPLE:
            while (current_input < MAX_ADC_INPUTS &&
                   adc_inputs[current_input].active == false)
            {
                ++current_input;
            }

            if (current_input < MAX_ADC_INPUTS &&
                adc_inputs[current_input].active == true)
            {
                SelectInput(current_input);
                //Start a new conversion
                ADCSRA |= (1 << ADSC);
                adc_state = LIBADC_SAMPLING;
            }
            break;

        case LIBADC_SAMPLING:
            //Check if ADC is done
            if (ADCSRA & (1 << ADIF))
            {
                adc_inputs[current_input].sample_value = ADCL;
                adc_inputs[current_input].sample_value |= (ADCH << 8);
                adc_inputs[current_input].valid = true;
                ADCSRA &= ~(1 << ADIF);
                adc_state = LIBADC_NEW_SAMPLE;
                ++current_input;
            }
            break;

        default:
            WARNING("Unknown State");
            break;
    }
}

///
/// @brief Enable ADC
///
/// @param  mode Select if enable/disable ADC
/// @return None
///
void libADC_Enable(bool mode)
{
    if (mode == true)
    {
        adc_state = LIBADC_NEW_SAMPLE;
        ADCSRA |= (1 << ADEN);
    }
    else
    {
        adc_state = LIBADC_IDLE;
        ADCSRA &= ~(1 << ADEN);
    }
}

///
/// @brief Enable ADC input. This will enable/disable the selected input and start sampling the input if in
///        free running mode.
///
/// @param  index Index of input to enable/disable
/// @param  mode Selects if to enable or disable input
/// @return None
///
void libADC_EnableInput(uint8_t index, bool mode)
{
    sc_assert(index < MAX_ADC_INPUTS);

    adc_inputs[index].active = mode;
    return;
}

///
/// @brief Get sample value from the selected input
///
/// @param  index Index of input to get sample from
/// @param  *sample_value Pointer to varibale where the sample will be stored
/// @return ERROR  If index is invalid or input is disabled
/// @return SUCCESS If index is valid and input is active
///
function_status libADC_GetSample(uint8_t index, uint16_t *sample_value)
{
    function_status status = ERROR;

    sc_assert(index < MAX_ADC_INPUTS);

    if (adc_inputs[index].active == true && adc_inputs[index].valid == true)
    {
        *sample_value = adc_inputs[index].sample_value;
        status = SUCCESS;
    }
    return status;
}

///
/// @brief Check if any ADC-conversion for the selected channel is done
///
/// @param  index Index of channel
/// @return bool true if valid, otherwise false
///
bool libADC_IsChannelValid(uint8_t index)
{
    sc_assert(index < MAX_ADC_INPUTS);
    return (adc_inputs[index].valid == true);
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static void SelectInput(uint8_t adc_channel)
{
    uint8_t new_admux;

    sc_assert(adc_channel < MAX_ADC_INPUTS);

    new_admux = ADMUX;
    //Clear MUX-bits
    new_admux &= 0xF0;

    //Set new channel
    new_admux |= adc_channel;
    ADMUX = new_admux;
}

static void InitInputArray()
{
    uint8_t index;

    for (index = 0; index < MAX_ADC_INPUTS; ++index)
    {
        adc_inputs[index].active = false;
        adc_inputs[index].valid = false;
        adc_inputs[index].channel_index = index;
        adc_inputs[index].sample_value = 0;
    }
}