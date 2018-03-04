/**
 * @file   Sensor.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-03-04 (Last edit)
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
#include "Timer.h"
#include "Sensor.h"
#include "CRC.h"
#include "libDebug.h"
#include "libDS3234.h"
#include "driverNTC.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define MAX_NUMBER_OF_SENSORS 8

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

struct module_t
{
    struct sensor_t *sensors[MAX_NUMBER_OF_SENSORS];
    size_t number_of_sensors;
};

struct __attribute__((packed)) sensor_statistics_t
{
    int16_t max;
    int16_t min;
    uint16_t crc;
};

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static struct module_t module;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

static void SetSensorValues(struct sensor_t *self, int16_t value);
static void WriteValuesToSRAM(const struct sensor_t *self);
static void ReadValuesFromSRAM(struct sensor_t *self);
static void ResetValues(struct sensor_t *self);
static bool IsCRCValid(const struct sensor_statistics_t *statistics);
static void UpdateCRC(struct sensor_statistics_t *statistics);

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void Sensor_Init(void)
{
    module.number_of_sensors = 0;

    INFO("Sensor module initialized");
}

void Sensor_Update(void)
{
    for (size_t i = 0; i < module.number_of_sensors; ++i)
    {
        if (module.sensors[i]->Update != NULL)
        {
            module.sensors[i]->Update(module.sensors[i]);
        }

        if (Sensor_IsValid(module.sensors[i]))
        {
            SetSensorValues(module.sensors[i], module.sensors[i]->value);
            WriteValuesToSRAM(module.sensors[i]);
        }
    }
}

void Sensor_Register(struct sensor_t *self)
{
    sc_assert(self != NULL);
    sc_assert(module.number_of_sensors < ElementsIn(module.sensors));

    module.sensors[module.number_of_sensors] = self;

    // Assign a unique ID to the sensor so it's values can be located in
    // SRAM later on.
    self->id = module.number_of_sensors;

    ResetValues(module.sensors[module.number_of_sensors]);
    ReadValuesFromSRAM(module.sensors[module.number_of_sensors]);

    ++module.number_of_sensors;
}

bool Sensor_GetValue(struct sensor_t *self, int16_t *value)
{
    sc_assert(self != NULL);
    sc_assert(value != NULL);

    *value = self->value;
    return self->valid;
}

bool Sensor_GetMaxValue(struct sensor_t *self, int16_t *value)
{
    sc_assert(self != NULL);
    sc_assert(value != NULL);

    *value = self->max;
    return self->valid;
}

bool Sensor_GetMinValue(struct sensor_t *self, int16_t *value)
{
    sc_assert(self != NULL);
    sc_assert(value != NULL);

    *value = self->min;
    return self->valid;
}

bool Sensor_IsValid(struct sensor_t *self)
{
    sc_assert(self != NULL);

    return self->valid;
}

void Sensor_Reset(struct sensor_t *self)
{
    sc_assert(self != NULL);

    ResetValues(self);
    WriteValuesToSRAM(self);
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static void SetSensorValues(struct sensor_t *self, int16_t value)
{
    sc_assert(self != NULL);

    self->value = value;

    if (value > self->max)
    {
        self->max = value;
    }

    if (value < self->min)
    {
        self->min = value;
    }
}

static void WriteValuesToSRAM(const struct sensor_t *self)
{
    uint8_t address = sizeof(struct sensor_statistics_t) * self->id;

    struct sensor_statistics_t stats;
    stats.max = self->max;
    stats.min = self->min;

    UpdateCRC(&stats);
    libDS3234_WriteToSRAM(address, (uint8_t *)&stats, sizeof(stats));
}

static void ReadValuesFromSRAM(struct sensor_t *self)
{
    uint8_t address = sizeof(struct sensor_statistics_t) * self->id;
    struct sensor_statistics_t stats;

    libDS3234_ReadFromSRAM(address, (uint8_t *)&stats, sizeof(stats));

    if (IsCRCValid(&stats))
    {
        self->max = stats.max;
        self->min = stats.min;
    }
}

static void ResetValues(struct sensor_t *self)
{
    self->max = INT16_MIN;
    self->min = INT16_MAX;
    self->value = 0;
    self->valid = false;
}

static bool IsCRCValid(const struct sensor_statistics_t *statistics)
{
    size_t size;
    size = sizeof(*statistics) - offsetof(__typeof__(*statistics), crc);

    return CRC_16(statistics, size) == statistics->crc;
}

static void UpdateCRC(struct sensor_statistics_t *statistics)
{
    size_t size;
    size = sizeof(*statistics) - offsetof(__typeof__(*statistics), crc);

    statistics->crc = CRC_16(statistics, size);
}
