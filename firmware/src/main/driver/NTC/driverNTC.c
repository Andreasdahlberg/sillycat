/**
 * @file   driverNTC.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-10-31 (Last edit)
 * @brief  NTC sensor driver
 *
 * Driver for NTC sensors.
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

#include <avr/pgmspace.h>
#include "ADC.h"
#include "Sensor.h"
#include "Timer.h"
#include "Filter.h"
#include "libDebug.h"
#include "driverNTC.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define SAMPLE_PERIOD_MS 100

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

struct ntc_sensor_t
{
    struct sensor_t base;
    struct
    {
        struct adc_channel_t channel;
        uint8_t index;
    } adc;
    struct filter_t filter;
    uint32_t timer;
};

struct lookup_table_t
{
    uint16_t offset;
    uint16_t temperatures[819];
};

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

static void Update(struct sensor_t *super);
static bool GetTemperature(struct ntc_sensor_t *self, int16_t *temperature);
static bool ADCValueToTemperature(uint16_t adc_value, int16_t *temperature);
static bool IsValid(uint16_t adc_value);
static int16_t FilterTemperature(struct ntc_sensor_t *self, int16_t temperature);

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

// TODO: Use generated lookup table.
static const struct lookup_table_t ntc_lut PROGMEM =
{
    .offset = 120,
    .temperatures = {
        -200, -198, -197, -195, -193, -191, -190, -188, -186, -184, -183, -181, -179, -178,
        -176, -174, -173, -171, -169, -168, -166, -165, -163, -161, -160, -158, -157, -155,
        -154, -152, -150, -149, -147, -146, -144, -143, -141, -140, -138, -137, -135, -134,
        -132, -131, -129, -128, -127, -125, -124, -122, -121, -119, -118, -117, -115, -114,
        -112, -111, -110, -108, -107, -106, -104, -103, -101, -100, -99, -97, -96, -95, -93,
        -92, -91, -89, -88, -87, -85, -84, -83, -82, -80, -79, -78, -76, -75, -74, -73, -71,
        -70, -69, -68, -66, -65, -64, -63, -61, -60, -59, -58, -56, -55, -54, -53, -51, -50,
        -49, -48, -47, -45, -44, -43, -42, -41, -39, -38, -37, -36, -35, -33, -32, -31, -30,
        -29, -28, -26, -25, -24, -23, -22, -21, -19, -18, -17, -16, -15, -14, -13, -11, -10,
        -9, -8, -7, -6, -5, -4, -2, -1, 0, 0, 1, 2, 3, 4, 5, 7, 8, 9, 10, 11, 12, 13, 14, 15,
        16, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 31, 32, 33, 34, 35, 36, 37, 38, 39,
        40, 41, 42, 43, 44, 45, 46, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62,
        63, 64, 65, 66, 67, 68, 69, 70, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85,
        86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105,
        106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122,
        123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139,
        140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156,
        157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173,
        174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190,
        191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
        208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224,
        226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242,
        243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 256, 257, 258, 259,
        260, 261, 262, 263, 264, 266, 267, 268, 269, 270, 271, 272, 273, 274, 275, 276, 277,
        278, 279, 280, 281, 282, 283, 284, 285, 287, 288, 289, 290, 291, 292, 293, 294, 295,
        296, 297, 298, 299, 300, 301, 303, 304, 305, 306, 307, 308, 309, 310, 311, 312, 313,
        314, 316, 317, 318, 319, 320, 321, 322, 323, 324, 325, 326, 328, 329, 330, 331, 332,
        333, 334, 335, 336, 338, 339, 340, 341, 342, 343, 344, 345, 347, 348, 349, 350, 351,
        352, 353, 355, 356, 357, 358, 359, 360, 361, 363, 364, 365, 366, 367, 368, 370, 371,
        372, 373, 374, 375, 377, 378, 379, 380, 381, 383, 384, 385, 386, 387, 389, 390, 391,
        392, 393, 395, 396, 397, 398, 399, 401, 402, 403, 404, 406, 407, 408, 409, 410, 412,
        413, 414, 415, 417, 418, 419, 421, 422, 423, 424, 426, 427, 428, 429, 431, 432, 433,
        435, 436, 437, 438, 440, 441, 442, 444, 445, 446, 448, 449, 450, 452, 453, 454, 456,
        457, 458, 460, 461, 462, 464, 465, 466, 468, 469, 471, 472, 473, 475, 476, 477, 479,
        480, 482, 483, 485, 486, 487, 489, 490, 492, 493, 495, 496, 497, 499, 500, 502, 503,
        505, 506, 508, 509, 511, 512, 514, 515, 517, 518, 520, 521, 523, 524, 526, 527, 529,
        530, 532, 533, 535, 537, 538, 540, 541, 543, 544, 546, 548, 549, 551, 553, 554, 556,
        557, 559, 561, 562, 564, 566, 567, 569, 571, 572, 574, 576, 577, 579, 581, 583, 584,
        586, 588, 590, 591, 593, 595, 597, 598, 600, 602, 604, 606, 608, 609, 611, 613, 615,
        617, 619, 621, 622, 624, 626, 628, 630, 632, 634, 636, 638, 640, 642, 644, 646, 648,
        650, 652, 654, 656, 658, 660, 662, 664, 666, 668, 670, 672, 675, 677, 679, 681, 683,
        685, 688, 690, 692, 694, 696, 699, 701, 703, 706, 708, 710, 713, 715, 717, 720, 722,
        724, 727, 729, 732, 734, 737, 739, 742, 744, 747, 749, 752, 754, 757, 759, 762, 765,
        767, 770, 773, 776, 778, 781, 784, 787, 789, 792, 795, 798, 801, 804, 807, 810, 813,
        816, 819, 822, 825, 828, 831, 834, 837, 840, 844, 847, 850, 853, 857, 860, 863, 867,
        870, 874, 877, 881, 884, 888, 892, 895, 899, 903, 906, 910, 914, 918, 922, 926, 930,
        934, 938, 942, 946, 950, 955, 959, 963, 968, 972, 977, 981, 986, 990, 995, 1000, 1005,
        1009, 1014, 1019, 1024, 1030, 1035, 1040, 1045, 1051
    }
};

static struct ntc_sensor_t ntc_sensors[] =
{
    {
        .base =
        {
            .Update = Update
        },
        .adc = {
            .index = 0x06
        }
    },
    {
        .base =
        {
            .Update = Update
        },
        .adc = {
            .index = 0x07
        }
    }
};

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void driverNTC_Init(void)
{
    for (size_t i = 0; i < ElementsIn(ntc_sensors); ++i)
    {
        ADC_InitChannel(&ntc_sensors[i].adc.channel, ntc_sensors[i].adc.index);
    }

    INFO("NTC sensors initialized");
}

struct sensor_t *driverNTC_GetSensor(size_t id)
{
    sc_assert(id < ElementsIn(ntc_sensors));

    return (struct sensor_t *)&ntc_sensors[id];
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static void Update(struct sensor_t *super)
{
    struct ntc_sensor_t *self = (struct ntc_sensor_t *)super;

    if (Timer_TimeDifference(self->timer) > SAMPLE_PERIOD_MS)
    {
        int16_t temperature;

        if (GetTemperature(self, &temperature))
        {
            self->base.value = FilterTemperature(self, temperature);
            self->base.valid = true;
        }
        else
        {
            self->base.valid = false;
        }

        self->timer = Timer_GetMilliseconds();
    }
}

static bool GetTemperature(struct ntc_sensor_t *self, int16_t *temperature)
{
    uint16_t adc_value;

    ADC_Convert(&self->adc.channel, &adc_value, 1);
    return ADCValueToTemperature(adc_value, temperature);
}

static bool ADCValueToTemperature(uint16_t adc_value, int16_t *temperature)
{
    if (IsValid(adc_value))
    {
        const uint16_t offset = pgm_read_word(&ntc_lut.offset);

        *temperature = pgm_read_word(&ntc_lut.temperatures[adc_value - offset]);
        return true;
    }
    else
    {
        return false;
    }
}

static bool IsValid(uint16_t adc_value)
{
    const uint16_t offset = pgm_read_word(&ntc_lut.offset);

    return (adc_value >= offset) &&
           (adc_value - offset < ElementsIn(ntc_lut.temperatures));
}

static int16_t FilterTemperature(struct ntc_sensor_t *self, int16_t temperature)
{
    if (Filter_IsInitialized(&self->filter))
    {
        Filter_Process(&self->filter, temperature);
    }
    else
    {
        /**
         * Assuming a sample frequency of 10 Hz, α = 0.2 gives a cutoff of 4 Hz.
         */
        Filter_Init(&self->filter, temperature, FILTER_ALPHA(0.2));
    }

    return Filter_Output(&self->filter);
}
