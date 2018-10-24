/**
 * @file   driverMCUTemperature.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-10-24 (Last edit)
 * @brief  MCU temperature driver
 *
 * Driver for the internal MCU temperature sensor.
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

#include "ADC.h"
#include "Sensor.h"
#include "Timer.h"
#include "libDebug.h"
#include "driverMCUTemperature_Calibration.h"
#include "driverMCUTemperature.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define SAMPLE_PERIOD_MS 100

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

struct mcu_sensor_t
{
    struct sensor_t base;
    struct
    {
        struct adc_channel_t channel;
        uint8_t index;
    } adc;
    uint32_t timer;
};

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

static void Update(struct sensor_t *super);
static bool GetTemperature(struct mcu_sensor_t *self, int16_t *temperature);
static int16_t ADCValueToTemperature(uint16_t adc_value);

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static struct mcu_sensor_t mcu_sensor =
{
    .base =
    {
        .Update = Update
    },
    .adc = {
        .index = 0x08
    }
};

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void driverMCUTemperature_Init(void)
{
    ADC_InitChannel(&mcu_sensor.adc.channel, mcu_sensor.adc.index);
    mcu_sensor.timer = 0;

    INFO("MCU temperature sensor initialized");
}

struct sensor_t *driverMCUTemperature_GetSensor(void)
{
    return (struct sensor_t *)&mcu_sensor;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static void Update(struct sensor_t *super)
{
    struct mcu_sensor_t *self = (struct mcu_sensor_t *)super;

    if (Timer_TimeDifference(self->timer) > SAMPLE_PERIOD_MS)
    {
        GetTemperature(self, &self->base.value);
        self->base.valid = true;
        self->timer = Timer_GetMilliseconds();
    }
}

static bool GetTemperature(struct mcu_sensor_t *self, int16_t *temperature)
{
    uint16_t adc_value;

    ADC_Convert(&self->adc.channel, &adc_value, 1);
    *temperature = ADCValueToTemperature(adc_value);

    return true;
}

static int16_t ADCValueToTemperature(uint16_t adc_value)
{
    int32_t tmp_value;
    tmp_value = (int32_t)adc_value;
    tmp_value *= SUPPLY_VOLTAGE_mV;
    tmp_value = tmp_value >> 10;
    tmp_value -= 289; // Offset between mV and temp from Table 23-2 in datasheet.
    tmp_value = tmp_value * MCU_TEMP_SENSOR_CALIBRATION_K +
                MCU_TEMP_SENSOR_CALIBRATION_M;

    return (int16_t)tmp_value;
}
