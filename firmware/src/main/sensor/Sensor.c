/**
 * @file   Sensor.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2021-01-14 (Last edit)
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

#include "common.h"
#include "Sensor.h"
#include "CRC.h"
#include "libDebug.h"
#include "driverNVM.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define MAX_NUMBER_OF_SENSORS 9

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
    bool valid;
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
static void WriteValuesToNVM(const struct sensor_t *self);
static void ReadValuesFromNVM(struct sensor_t *self);
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
            WriteValuesToNVM(module.sensors[i]);
        }
    }
}

void Sensor_Register(struct sensor_t *self)
{
    sc_assert(self != NULL);
    sc_assert(module.number_of_sensors < ElementsIn(module.sensors));

    module.sensors[module.number_of_sensors] = self;

    // Assign a unique ID to the sensor so it's values can be located in
    // NVM later on.
    self->id = module.number_of_sensors;

    ResetValues(module.sensors[module.number_of_sensors]);
    ReadValuesFromNVM(module.sensors[module.number_of_sensors]);

    ++module.number_of_sensors;
}

bool Sensor_GetValue(const struct sensor_t *self, int16_t *value)
{
    sc_assert(self != NULL);
    sc_assert(value != NULL);

    *value = self->value;
    return self->valid;
}

bool Sensor_GetMaxValue(const struct sensor_t *self, int16_t *value)
{
    sc_assert(self != NULL);
    sc_assert(value != NULL);

    *value = self->statistics.max;
    return self->statistics.valid;
}

bool Sensor_GetMinValue(const struct sensor_t *self, int16_t *value)
{
    sc_assert(self != NULL);
    sc_assert(value != NULL);

    *value = self->statistics.min;
    return self->statistics.valid;
}

bool Sensor_IsValid(const struct sensor_t *self)
{
    sc_assert(self != NULL);

    return self->valid;
}

bool Sensor_IsStatisticsValid(const struct sensor_t *self)
{
    sc_assert(self != NULL);

    return self->statistics.valid;
}

void Sensor_Reset(struct sensor_t *self)
{
    sc_assert(self != NULL);

    ResetValues(self);
    WriteValuesToNVM(self);
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static void SetSensorValues(struct sensor_t *self, int16_t value)
{
    sc_assert(self != NULL);

    self->value = value;

    if (value > self->statistics.max)
    {
        self->statistics.max = value;
    }

    if (value < self->statistics.min)
    {
        self->statistics.min = value;
    }

    self->statistics.valid = true;
}

static void WriteValuesToNVM(const struct sensor_t *self)
{
    uint8_t address = sizeof(struct sensor_statistics_t) * self->id;

    struct sensor_statistics_t stats;
    stats.max = self->statistics.max;
    stats.min = self->statistics.min;
    stats.valid = self->statistics.valid;

    UpdateCRC(&stats);
    driverNVM_Write(address, &stats, sizeof(stats));
}

static void ReadValuesFromNVM(struct sensor_t *self)
{
    uint8_t address = sizeof(struct sensor_statistics_t) * self->id;
    struct sensor_statistics_t stats;

    driverNVM_Read(address, &stats, sizeof(stats));

    if (IsCRCValid(&stats))
    {
        self->statistics.max = stats.max;
        self->statistics.min = stats.min;
        self->statistics.valid = stats.valid;
    }
    else
    {
        self->statistics.valid = false;
    }
}

static void ResetValues(struct sensor_t *self)
{
    self->statistics.max = INT16_MIN;
    self->statistics.min = INT16_MAX;
    self->statistics.valid = false;
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
