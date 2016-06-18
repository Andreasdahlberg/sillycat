/**
 * @file   main_firmware.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2016-05-19 (Last edit)
 * @brief  Implementation of main
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
#include <avr/wdt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "libDebug.h"
#include "libADC.h"
#include "libSPI.h"
#include "libDS3234.h"
#include "libRFM69.h"
#include "libInput.h"

#include "Sensor.h"
#include "Interface.h"
#include "Timer.h"
#include "Transceiver.h"
#include "Nodes.h"
#include "Config.h"
#include "ErrorHandler.h"

#include "guiRTC.h"
#include "guiSensor.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define LOW_STACK_LIMIT 100 // ~5% left of total memory
#define HICH_MCU_TEMP_LIMIT 75 // ~10% below max operating temperature

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

void CheckHealth(void);

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

int main(void)
{
#ifdef DEBUG_ENABLE
    uint8_t mcu_status = MCUSR;
    MCUSR = 0;
#endif

    wdt_disable();

    libDS3234_InitHW();
    libRFM69_InitHW();

    libDebug_Init();
    INFO("Main unit started");
    INFO("Last reset: 0x%02X", mcu_status);
    ErrorHandler_Init();
    libADC_Init();
    Timer_Init();
    libSPI_Init(1);
    libDS3234_Init();
    libADC_Enable(true);
    Sensor_Init();
    libInput_Init();
    Config_Load();
    Transceiver_Init();
    Interface_Init();
    //NOTE: The first gui init called will be the root view.
    guiRTC_Init();
    guiSensor_Init();
    Nodes_Init();

    libInput_SetCallbacks(Interface_NextView, Interface_PreviousView,
                          Interface_ActivateView);

    INFO("Start up done");
    DEBUG("Node ID: 0x%02X\r\n", Config_GetNodeId());
    DEBUG("Node role: 0x%02X\r\n", Config_GetNodeRole());

    ErrorHandler_PrintLog();

    uint32_t check_timer = Timer_GetMilliseconds();

    while (1)
    {
        libADC_Update();
        libInput_Update();
        Sensor_Update();

        Transceiver_Update();
        Interface_Update();

        if (Timer_TimeDifference(check_timer) > 1000)
        {
            CheckHealth();
            check_timer = Timer_GetMilliseconds();
        }

    }

    CRITICAL("Main loop exit");
    SoftReset();
}

void CheckHealth(void)
{
    static bool memory_low_flag = false;
    static bool high_mcu_temp_flag = false;

    uint16_t unused_memory = StackCount();
    if (!memory_low_flag && unused_memory < LOW_STACK_LIMIT)
    {
        ErrorHandler_LogError(LOW_STACK, unused_memory);
        memory_low_flag = true;
        WARNING("Low memory: %u", unused_memory);
    }

    uint16_t mcu_temperature;
    Sensor_GetSensorValue(SENSOR_INTERNAL_TEMPERATURE, &mcu_temperature);
    if (!high_mcu_temp_flag && mcu_temperature > HICH_MCU_TEMP_LIMIT)
    {
        ErrorHandler_LogError(HICH_MCU_TEMP_LIMIT, (uint8_t)mcu_temperature);
        high_mcu_temp_flag = true;
        WARNING("High MCU temperature: %u", mcu_temperature);
    }

    sc_assert(unused_memory > 0);
}