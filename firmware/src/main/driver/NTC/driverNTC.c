/**
 * @file   driverNTC.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2021-04-12 (Last edit)
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
        -201, -199, -197, -195, -194, -192, -190, -188, -187, -185, -183, -181, -180, -178,
        -176, -175, -173, -172, -170, -168, -167, -165, -163, -162, -160, -159, -157, -156,
        -154, -152, -151, -149, -148, -146, -145, -143, -142, -140, -139, -137, -136, -134,
        -133, -131, -130, -129, -127, -126, -124, -123, -121, -120, -119, -117, -116, -114,
        -113, -112, -110, -109, -107, -106, -105, -103, -102, -101, -99, -98, -97, -95, -94,
        -93, -91, -90, -89, -87, -86, -85, -83, -82, -81, -79, -78, -77, -76, -74, -73, -72,
        -71, -69, -68, -67, -66, -64, -63, -62, -61, -59, -58, -57, -56, -54, -53, -52, -51,
        -50, -48, -47, -46, -45, -43, -42, -41, -40, -39, -38, -36, -35, -34, -33, -32, -30,
        -29, -28, -27, -26, -25, -23, -22, -21, -20, -19, -18, -17, -15, -14, -13, -12, -11,
        -10, -9, -7, -6, -5, -4, -3, -2, -1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 14, 15, 16,
        17, 18, 19, 20, 21, 22, 23, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 39, 40,
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 59, 60, 61, 62, 63,
        64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85,
        86, 87, 88, 89, 90, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106,
        107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123,
        124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140,
        141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157,
        158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174,
        175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
        192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208,
        209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225,
        226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242,
        243, 244, 245, 246, 247, 248, 249, 251, 252, 253, 254, 255, 256, 257, 258, 259, 260,
        261, 262, 263, 264, 265, 266, 267, 268, 269, 270, 271, 272, 273, 274, 275, 276, 278,
        279, 280, 281, 282, 283, 284, 285, 286, 287, 288, 289, 290, 291, 292, 293, 294, 296,
        297, 298, 299, 300, 301, 302, 303, 304, 305, 306, 307, 308, 310, 311, 312, 313, 314,
        315, 316, 317, 318, 319, 320, 321, 323, 324, 325, 326, 327, 328, 329, 330, 331, 333,
        334, 335, 336, 337, 338, 339, 340, 341, 343, 344, 345, 346, 347, 348, 349, 351, 352,
        353, 354, 355, 356, 357, 359, 360, 361, 362, 363, 364, 365, 367, 368, 369, 370, 371,
        372, 374, 375, 376, 377, 378, 380, 381, 382, 383, 384, 385, 387, 388, 389, 390, 391,
        393, 394, 395, 396, 397, 399, 400, 401, 402, 404, 405, 406, 407, 409, 410, 411, 412,
        413, 415, 416, 417, 418, 420, 421, 422, 424, 425, 426, 427, 429, 430, 431, 432, 434,
        435, 436, 438, 439, 440, 442, 443, 444, 445, 447, 448, 449, 451, 452, 453, 455, 456,
        457, 459, 460, 461, 463, 464, 466, 467, 468, 470, 471, 472, 474, 475, 477, 478, 479,
        481, 482, 484, 485, 486, 488, 489, 491, 492, 494, 495, 496, 498, 499, 501, 502, 504,
        505, 507, 508, 510, 511, 513, 514, 516, 517, 519, 520, 522, 523, 525, 526, 528, 529,
        531, 532, 534, 536, 537, 539, 540, 542, 543, 545, 547, 548, 550, 551, 553, 555, 556,
        558, 560, 561, 563, 565, 566, 568, 570, 571, 573, 575, 576, 578, 580, 581, 583, 585,
        587, 588, 590, 592, 594, 595, 597, 599, 601, 603, 604, 606, 608, 610, 612, 614, 615,
        617, 619, 621, 623, 625, 627, 629, 631, 632, 634, 636, 638, 640, 642, 644, 646, 648,
        650, 652, 654, 656, 658, 660, 662, 665, 667, 669, 671, 673, 675, 677, 679, 682, 684,
        686, 688, 690, 693, 695, 697, 699, 702, 704, 706, 708, 711, 713, 715, 718, 720, 722,
        725, 727, 730, 732, 735, 737, 740, 742, 745, 747, 750, 752, 755, 757, 760, 763, 765,
        768, 771, 773, 776, 779, 782, 784, 787, 790, 793, 796, 798, 801, 804, 807, 810, 813,
        816, 819, 822, 825, 828, 831, 835, 838, 841, 844, 847, 851, 854, 857, 861, 864, 867,
        871, 874, 878, 881, 885, 888, 892, 896, 899, 903, 907, 911, 915, 918, 922, 926, 930,
        934, 938, 942, 947, 951, 955, 959, 964, 968, 973, 977, 982, 986, 991, 995, 1000, 1005,
        1010, 1015, 1020, 1025, 1030, 1035, 1040, 1046, 1051
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
        Filter_Init(&self->filter, temperature, FILTER_ALPHA(0.1));
    }

    return Filter_Output(&self->filter);
}
