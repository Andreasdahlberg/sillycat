/**
 * @file   driverDHT22.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2019-04-30 (Last edit)
 * @brief  DHT22 RHT sensor driver.
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
#include <avr/io.h>
#include "libDebug.h"
#include "Timer.h"
#include "driverInputCapture.h"
#include "driverDHT22.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define DATAPIN DDB0
#define POWERUP_DELAY_MS 1000
#define LOW_HIGH_LIMIT 100
#define REQUEST_READING_TIME_MS 2
#define READING_TIMEOUT_MS 6
#define TIMINGS_INDEX_OFFSET 3
#define EXPECTED_NR_PULSES 43

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

typedef enum
{
    DHT_UNINITIALIZED = 0,
    DHT_IDLE,
    DHT_POWERUP,
    DHT_READING,
    DHT_DECODING,
} dht_state_t;

typedef enum
{
    DHT_READING_REQUEST = 0,
    DHT_READING_REQUEST_WAIT,
    DHT_READING_CAPTURE_DATA
} dht_reading_state_t;

struct module_t
{
    dht_state_t state;
    struct
    {
        int16_t temperature;
        int16_t humidity;
        bool valid;
    } measurement;
    struct
    {
        dht_reading_state_t state;
        uint32_t timer;
    } reading;
    uint32_t initialization_time;
    volatile uint32_t pulse_counter;
    volatile uint16_t pulse_timings[EXPECTED_NR_PULSES];
};

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

struct module_t module;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

static void InitHardware(void);
static void DecodeTimings(void);
static uint16_t ConvertToScaledInteger(uint8_t integral, uint8_t fractional);
static bool IsDataValid(uint8_t *data);
static dht_state_t ReadingStateMachine(void);
static void InterruptHandler(uint16_t value);

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void driverDHT22_Init(void)
{
    InitHardware();
    driverInputCapture_Init();
    driverInputCapture_DisableNoiseCanceling();
    driverInputCapture_RegisterInterruptHandler(InterruptHandler);

    module = (__typeof__(module))
    {
        .state = DHT_POWERUP,
        .initialization_time = Timer_GetMilliseconds(),
    };

    INFO("DHT22 RHT sensor initialized");
}

void driverDHT22_Update(void)
{
    switch (module.state)
    {
        case DHT_POWERUP:
            if (Timer_TimeDifference(module.initialization_time) > POWERUP_DELAY_MS)
            {
                module.state = DHT_IDLE;
            }
            break;

        case DHT_IDLE:
            break;

        case DHT_READING:
            module.state = ReadingStateMachine();
            break;

        case DHT_DECODING:
            DecodeTimings();
            module.initialization_time = Timer_GetMilliseconds();

            //TODO: Power up or new state? save last reading time?
            module.state = DHT_IDLE;
            break;

        case DHT_UNINITIALIZED:
        default:
            sc_assert_fail();
    }
}

int16_t driverDHT22_GetTemperature(void)
{
    return module.measurement.temperature;
}

int16_t driverDHT22_GetHumidity(void)
{
    return module.measurement.humidity;
}

bool driverDHT22_IsMeasurmentValid(void)
{
    return module.measurement.valid;
}

void driverDHT22_ClearValidFlag(void)
{
    module.measurement.valid = false;
}

bool driverDHT22_IsIdle(void)
{
    return (module.state == DHT_IDLE);
}

void driverDHT22_StartMeasurement(void)
{
    if (driverDHT22_IsIdle() == true)
    {
        module.state = DHT_READING;
    }
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static void InitHardware(void)
{
    //Set data pin as output and high
    DDRB |= (1 << DATAPIN);
    PORTB |= (1 << DATAPIN);
}

static dht_state_t ReadingStateMachine(void)
{
    dht_state_t next_dht_state = DHT_READING;

    switch (module.reading.state)
    {
        case DHT_READING_REQUEST:
            module.reading.timer = Timer_GetMilliseconds();

            //Pull data pin low
            DDRB |= (1 << DATAPIN);
            PORTB &= ~(1 << DATAPIN);

            module.reading.state = DHT_READING_REQUEST_WAIT;
            break;

        case DHT_READING_REQUEST_WAIT:
            //NOTE: A delay of minimum 1 ms is required
            if (Timer_TimeDifference(module.reading.timer) >= REQUEST_READING_TIME_MS)
            {
                //Pull data pin high and then reconfigure as input
                PORTB |= (1 << DATAPIN);
                DDRB &= ~(1 << DATAPIN);

                module.reading.timer = Timer_GetMilliseconds();
                module.pulse_counter = 0;

                driverInputCapture_Enable();
                module.reading.state = DHT_READING_CAPTURE_DATA;
            }
            break;

        case DHT_READING_CAPTURE_DATA:
            if (module.pulse_counter == EXPECTED_NR_PULSES)
            {
                driverInputCapture_Disable();
                module.reading.state = DHT_READING_REQUEST;
                next_dht_state =  DHT_DECODING;
            }
            //NOTE: According to the DHT22-datasheet a measurement takes 5 mS,
            //      with FOSC 8 MHz and 8 as prescaler the timer overflows in 64 mS.
            //      A timeout between 5 mS and 64 mS is therefore needed.
            else if (Timer_TimeDifference(module.reading.timer) > READING_TIMEOUT_MS)
            {
                ERROR("Timeout while capturing data");
                INFO("Pulse counter: %lu", module.pulse_counter);
                driverInputCapture_Disable();
                module.reading.state = DHT_READING_REQUEST;
                module.initialization_time = Timer_GetMilliseconds();
                next_dht_state = DHT_POWERUP;
            }
            break;

        default:
            sc_assert_fail();
            break;
    }
    return next_dht_state;
}

static uint16_t ConvertToScaledInteger(uint8_t integral, uint8_t fractional)
{
    return (integral << 8) + (fractional);
}

static void DecodeTimings(void)
{
    uint8_t bit_index;
    uint8_t byte_index;
    uint8_t index;
    uint8_t data[5] = {0, 0, 0, 0, 0};
    uint16_t prev = module.pulse_timings[TIMINGS_INDEX_OFFSET - 1];

    for (byte_index = 0; byte_index < 5; ++byte_index)
    {
        for (bit_index = 0; bit_index < 8; ++bit_index)
        {
            index = (byte_index << 3) + bit_index + TIMINGS_INDEX_OFFSET;
            if (module.pulse_timings[index] - prev > LOW_HIGH_LIMIT)
            {
                data[byte_index] |= (1 << (7 - bit_index));
            }
            prev = module.pulse_timings[index];
        }
    }

    if (IsDataValid(data) == true)
    {
        module.measurement.humidity = ConvertToScaledInteger(data[0], data[1]);
        module.measurement.temperature = ConvertToScaledInteger(data[2], data[3]);
        module.measurement.valid = true;
    }
    else
    {
        ERROR("Invalid data");
        module.measurement.valid = false;
    }
}

static bool IsDataValid(uint8_t *data)
{
    return (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF));
}

static void InterruptHandler(uint16_t value)
{
    if (module.pulse_counter < EXPECTED_NR_PULSES)
    {
        module.pulse_timings[module.pulse_counter] = value;
        ++module.pulse_counter;
    }
}
