/**
 * @file   libDHT22.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2015-11-14 (Last edit)
 * @brief  Implementation of libDHT22
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

#include "libdht22.h"
#include "libDebug.h"
#include "Timer.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define DATAPIN DDB0
#define MEASURE_INTERVAL_MS 2000 //Time between sensor readings, must be at least 2000 ms
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
} dht_state_type;

typedef enum
{
    DHT_READING_REQUEST = 0,
    DHT_READING_REQUEST_WAIT,
    DHT_READING_CAPTURE_DATA
} dht_reading_state_type;

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static dht_state_type dht22_state = DHT_UNINITIALIZED;
static dht22_data_type sensor_reading;
static uint32_t init_time;
static volatile uint32_t pulse_counter;
static volatile uint16_t pulse_timings[EXPECTED_NR_PULSES];

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

static void DecodeTimings(void);
static float ConvertToFloat(uint8_t integral, uint8_t fractional);
static bool IsDataValid(uint8_t *data);
static dht_state_type ReadingStateMachine(void);

//////////////////////////////////////////////////////////////////////////
//INTERUPT SERVICE ROUTINES
//////////////////////////////////////////////////////////////////////////

ISR(TIMER1_CAPT_vect)
{
    if (pulse_counter < EXPECTED_NR_PULSES)
    {
        pulse_timings[pulse_counter] = ICR1;
        ++pulse_counter;
    }
}

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void libDHT22_Init(void)
{
    //Set data pin as output and high
    DDRB |= (1 << DATAPIN);
    PORTB |= (1 << DATAPIN);

    dht22_state = DHT_IDLE;
    init_time = Timer_GetMilliseconds();
    sensor_reading.status = FALSE;

    INFO("Init done");
    return;
}

void libDHT22_Update(void)
{
    switch (dht22_state)
    {
        case DHT_IDLE:
            break;

        case DHT_POWERUP:
            if (Timer_TimeDifference(init_time) > MEASURE_INTERVAL_MS)
            {
                dht22_state = DHT_READING;
            }
            break;

        case DHT_READING:
            dht22_state = ReadingStateMachine();
            break;

        case DHT_DECODING:
            DecodeTimings();
            init_time = Timer_GetMilliseconds();
            dht22_state = DHT_IDLE;
            break;

        case DHT_UNINITIALIZED:
        default:
            ERROR("Uninitialized/unknown state, reseting module");
            libDHT22_Init();
            break;
    }
    return;
}

///
/// @brief Get the latest sensor reading
///
/// @param  None
/// @return dht22_data_type Struct with the sensor readings and its status
///
dht22_data_type libDHT22_GetSensorReading(void)
{
    dht22_data_type return_data = sensor_reading;
    sensor_reading.status = FALSE;
    return return_data;
}

bool libDHT22_IsReadingValid(void)
{
    return sensor_reading.status;
}

bool libDHT22_IsIdle(void)
{
    return (dht22_state == DHT_IDLE);
}

void libDHT22_StartReading(void)
{
    if (libDHT22_IsIdle() == TRUE)
    {
        dht22_state = DHT_POWERUP;
    }
    return;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static void EnableInputCapture(bool enabled)
{
    if (enabled == TRUE)
    {
        pulse_counter = 0;

        //Set clock source to CLK
        TCCR1B |= (1 << CS11);

        //Disable noise canceling
        TCCR1B &= ~(1 << ICNC1);

        //Enabled input capture interrupts
        TIMSK1 |= (1 << ICIE1);
    }
    else
    {
        //Disabled input capture interrupts
        TIMSK1 &= ~(1 << ICIE1);

        //Stop timer by removing the clock source
        TCCR1B &= ~(1 << CS12 | 1 << CS11 | 1 << CS10);

        //Reset the timer register
        TCNT1 = 0x00;
    }
    return;
}

static dht_state_type ReadingStateMachine(void)
{
    static dht_reading_state_type state = DHT_READING_REQUEST;
    static uint32_t reading_timer;
    dht_state_type next_dht_state = DHT_READING;

    switch (state)
    {
        case DHT_READING_REQUEST:
            reading_timer = Timer_GetMilliseconds();

            //Pull data pin low
            DDRB |= (1 << DATAPIN);
            PORTB &= ~(1 << DATAPIN);

            state = DHT_READING_REQUEST_WAIT;
            break;

        case DHT_READING_REQUEST_WAIT:
            //NOTE: A delay of minimum 1 ms is required
            if (Timer_TimeDifference(reading_timer) >= REQUEST_READING_TIME_MS)
            {
                //Pull data pin high and then reconfigure as input
                PORTB |= (1 << DATAPIN);
                DDRB &= ~(1 << DATAPIN);

                reading_timer = Timer_GetMilliseconds();

                EnableInputCapture(TRUE);
                state = DHT_READING_CAPTURE_DATA;
            }
            break;

        case DHT_READING_CAPTURE_DATA:
            //NOTE: According to the DHT22-datasheet a measurement takes 5 mS,
            //      with FOSC 8 MHz and 8 as prescaler the timer overflows in 64 mS.
            //      A timeout between 5 mS and 64 mS is therefore needed.
            if (Timer_TimeDifference(reading_timer) > READING_TIMEOUT_MS)
            {
                ERROR("Timeout while capturing data");
                state = DHT_READING_REQUEST;
                init_time = Timer_GetMilliseconds();
                return DHT_POWERUP;
            }

            if (pulse_counter == EXPECTED_NR_PULSES)
            {
                EnableInputCapture(FALSE);
                state = DHT_READING_REQUEST;
                next_dht_state =  DHT_DECODING;
            }
            break;

        default:
            ERROR("Unknown state");
            state = DHT_READING_REQUEST;
            libDHT22_Init();
            break;
    }
    return next_dht_state;
}

///
/// @brief Convert integral and fractional part into float
///
/// @param  integral The integral part of the float
/// @param  fractional The fractional part of the float
/// @return float The resulting float
///
static float ConvertToFloat(uint8_t integral, uint8_t fractional)
{
    float data = (integral << 8);
    data += fractional;
    data /= 10;
    return data;
}

///
/// @brief  Decode pulse timings and convert them into usable data
///
/// @param  None
/// @return None
///
static void DecodeTimings(void)
{
    uint8_t bit_index;
    uint8_t byte_index;
    uint8_t index;
    uint8_t data[5] = {0, 0, 0, 0, 0};
    uint16_t prev = pulse_timings[TIMINGS_INDEX_OFFSET - 1];

    for (byte_index = 0; byte_index < 5; ++byte_index)
    {
        for (bit_index = 0; bit_index < 8; ++bit_index)
        {
            index = (byte_index << 3) + bit_index + TIMINGS_INDEX_OFFSET;
            if (pulse_timings[index] - prev > LOW_HIGH_LIMIT)
            {
                data[byte_index] |= (1 << (7 - bit_index));
            }
            prev = pulse_timings[index];
        }
    }

    if (IsDataValid(data) == TRUE)
    {
        sensor_reading.humidity = ConvertToFloat(data[0], data[1]);
        sensor_reading.temperature = ConvertToFloat(data[2], data[3]);
        sensor_reading.status = TRUE;
    }
    else
    {
        ERROR("Invalid data");
        sensor_reading.status = FALSE;
    }
    return;
}

///
/// @brief Check if the sensor data is valid
///
/// @param  *data Pointer to array with data
/// @return bool Result
///
static bool IsDataValid(uint8_t *data)
{
    return (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF));
}