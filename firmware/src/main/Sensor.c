/**
 * @file   Sensor.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2017-08-26 (Last edit)
 * @brief  Implementation of Sensor module
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
#include <avr/pgmspace.h>
#include <string.h>

#include "libDebug.h"
#include "libDS3234.h"

#include "ADC.h"
#include "Sensor.h"
#include "Sensor_Calibration.h"
#include "Timer.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define TABLE_OFFSET    -30

#define READING_HEADER          0xAA0F
#define READING_START_ADDRESS   0x00
#define ADDRESS_OFFSET          0

#define SUPPLY_VOLTAGE_mV 3300

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

struct module_t
{
    struct adc_channel_t temperature_ext_channel;
    struct adc_channel_t temperature_int_channel;
};

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static struct module_t module;
static uint16_t current_alpha;
static sensor_sample_type sensor_reading;

//TODO: Generate table with better precision
static const uint16_t mf52_table[] PROGMEM =
{
    54, //-30
    72, //-25
    95, //-20
    122, //-15
    156, //-10
    194, //-5
    244, //0
    288, //5
    342, //10
    398, //15
    455, //20
    512, //25
    568, //30
    620, //35
    669, //40
    714, //45
    754, //50
    790, //55
    821, //60
    849, //65
    873, //70
    893, //75
    911, //80
    926, //85
    939, //90
    950, //95
    960, //100
    968, //105
    975, //110
};

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

static int16_t RawValueToTemperature(uint16_t raw_value);

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void Sensor_Init(void)
{
    current_alpha = (uint16_t)CALCULATE_ALPHA(SAMPLE_FREQUENCY_MS / 1000,
                    AVERAGE_WINDOW_S);
    DEBUG("Smoothing alpha: %u\r\n", current_alpha);

    ADC_InitChannel(&module.temperature_ext_channel, SENSOR_EXTERNAL_TEMPERATURE);
    ADC_InitChannel(&module.temperature_int_channel, SENSOR_INTERNAL_TEMPERATURE);

    INFO("Sensor module initialized");
}

void Sensor_Update(void)
{
    static uint32_t sample_timer = 0;

    if (Timer_TimeDifference(sample_timer) > SAMPLE_FREQUENCY_MS)
    {

        Sensor_GetReading(SENSOR_EXTERNAL_TEMPERATURE, &sensor_reading);
        Sensor_GetSensorValue(SENSOR_EXTERNAL_TEMPERATURE, &sensor_reading.value);

        if (sensor_reading.header == READING_HEADER)
        {
            sensor_reading.average = GetExponentialMovingAverage(sensor_reading.value,
                                     sensor_reading.average,
                                     current_alpha);

            if (sensor_reading.value > sensor_reading.max)
            {
                sensor_reading.max = sensor_reading.value;
            }
            else if (sensor_reading.value < sensor_reading.min)
            {
                sensor_reading.min = sensor_reading.value;
            }
        }
        else
        {
            INFO("No sample found, Address: 0x%02X, Header: 0x%04X\r\n", 0x00,
                 sensor_reading.header);

            sensor_reading.header = READING_HEADER;
            sensor_reading.max = sensor_reading.value;
            sensor_reading.min = sensor_reading.value;
            sensor_reading.average =
                sensor_reading.value; //NOTE: Is this bad? extreme initial values can mess up the average.
        }

        Sensor_SaveReading(SENSOR_EXTERNAL_TEMPERATURE, &sensor_reading);
        sample_timer = Timer_GetMilliseconds();
    }
}

bool Sensor_SaveReading(uint8_t sensor, sensor_sample_type *reading)
{
    uint8_t address;

    address = READING_START_ADDRESS + (ADDRESS_OFFSET + sensor) *
              sizeof(sensor_sample_type);
    return libDS3234_WriteToSRAM(address, (uint8_t *)reading,
                                 sizeof(sensor_sample_type));
}

bool Sensor_GetReading(uint8_t sensor, sensor_sample_type *reading)
{
    bool status = false;
    uint8_t address;

    address = READING_START_ADDRESS + (ADDRESS_OFFSET + sensor) *
              sizeof(sensor_sample_type);

    if (libDS3234_ReadFromSRAM(address, (uint8_t *)reading,
                               sizeof(sensor_sample_type)))
    {
        status = (bool)(reading->header == READING_HEADER);
    }

    return status;
}

void Sensor_GetSensorValue(uint8_t sensor, int16_t *sensor_value)
{
    int32_t tmp_value;
    uint16_t adc_value;

    switch (sensor)
    {
        case SENSOR_EXTERNAL_TEMPERATURE:
            ADC_Convert(&module.temperature_ext_channel, &adc_value, 1);
            *sensor_value = RawValueToTemperature(adc_value);
            break;

        case SENSOR_INTERNAL_TEMPERATURE:
            ADC_Convert(&module.temperature_int_channel, &adc_value, 1);
            tmp_value = (int32_t)adc_value;
            tmp_value *= SUPPLY_VOLTAGE_mV;
            tmp_value = tmp_value >> 10;
            tmp_value -= 289; // Offset between mV and temp from Table 23-2 in datasheet.

#ifdef INTERNAL_TEMP_SENSOR_CALIBRATED
            tmp_value = tmp_value * INTERNAL_TEMP_CALIBRATION_K +
                        INTERNAL_TEMP_CALIBRATION_M;
#endif
            *sensor_value = (int16_t)(tmp_value);
            break;

        default:
            sc_assert_fail();
            break;
    }
    return;
}

///
/// @brief Clear all saved sensor readings.
///
/// NOTE: The entire DS3234 SRAM will be cleared!
///
/// @param  None
/// @return None
///
void Sensor_ClearReadings(void)
{
    libDS3234_ClearSRAM();
    return;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static int16_t RawValueToTemperature(uint16_t raw_value)
{
    // Truncate the raw value if it's outside of the LUT.
    if (raw_value < mf52_table[0])
    {
        raw_value = mf52_table[0];
    }
    else if (raw_value > mf52_table[ElementsIn(mf52_table) - 1])
    {
        raw_value = mf52_table[ElementsIn(mf52_table) - 1];
    }

    uint8_t index = 0;
    while (index < ElementsIn(mf52_table) &&
            pgm_read_word(&mf52_table[index]) < raw_value)
    {
        ++index;
    }

    int32_t tmp;
    tmp = 100 * (int32_t)raw_value;
    tmp /= pgm_read_word(&mf52_table[index]);
    tmp *= (TABLE_OFFSET + (index * 5));
    tmp /= 10;
    return  (int16_t)tmp;
}
