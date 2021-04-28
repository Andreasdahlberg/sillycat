/**
 * @file   main_firmware.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2021-04-28 (Last edit)
 * @brief  Implementation of main
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
#include <avr/wdt.h>

#include "libDebug.h"
#include "libSPI.h"
#include "libRFM69.h"
#include "libUI.h"

#include "Board.h"
#include "RTC.h"
#include "ADC.h"
#include "Sensor.h"
#include "Interface.h"
#include "Timer.h"
#include "Transceiver.h"
#include "Com.h"
#include "Nodes.h"
#include "Config.h"
#include "ErrorHandler.h"
#include "PacketHandler.h"
#include "Encoder.h"

#include "driverNTC.h"
#include "driverPEC11.h"

#include "guiRTC.h"
#include "guiSensor.h"
#include "guiNodes.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define LOW_STACK_LIMIT 100 // ~5% left of total memory

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

struct module_t
{
    struct node_t nodes[3];
    uint32_t memory_check_timer;
    bool memory_low_flag;
};

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static struct module_t module;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

void CheckMemoryUsage(void);
void assert_fail_handler(const char *file_p, int line_number, const char *expression_p);
void handle_corrupt_config(void);

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

    Board_Init();
    libDebug_Init();
    INFO("Main unit started");
    INFO("Last reset: 0x%02X", mcu_status);
    ErrorHandler_Init();
    ErrorHandler_SetAssertFailHandler(assert_fail_handler);
    ADC_Init();
    Timer_Init();
    libSPI_Init(1);
    RTC_Init();
    Sensor_Init();
    driverNTC_Init();

    Interface_Init();
    if (!Config_Load())
    {
        handle_corrupt_config();
    }
    Transceiver_Init();
    Com_Init();
    Encoder_Init();
    //NOTE: The first gui init called will be the root view.
    guiRTC_Init();
    guiSensor_Init();
    guiNodes_Init();
    Nodes_Init();

    Sensor_Register(driverNTC_GetSensor(0));
    Sensor_Register(driverNTC_GetSensor(1));

    for (size_t i = 0; i < ElementsIn(module.nodes); ++i)
    {
        Node_Init(&module.nodes[i], i + 128);
        Nodes_Add(&module.nodes[i]);
    }

    Com_SetPacketHandler(PacketHandler_HandleReadingPacket, COM_PACKET_TYPE_READING);

    const struct encoder_callbacks_t encoder_callbacks =
    {
        .right = Interface_NextView,
        .left = Interface_PreviousView,
        .brief_push = Interface_ActivateView,
        .extended_push = Interface_Action
    };
    Encoder_SetCallbacks(&encoder_callbacks);

    INFO("Start up done");
    DEBUG("Device address: 0x%02X\r\n", Config_GetAddress());

    module.memory_check_timer = Timer_GetMilliseconds();

    while (1)
    {
        Encoder_Update();
        Sensor_Update();
        Transceiver_Update();
        Com_Update();
        Interface_Update();
        CheckMemoryUsage();
    }

    CRITICAL("Main loop exit");
    Board_SoftReset();
}

void CheckMemoryUsage(void)
{
    const uint32_t check_interval = 1000;

    if (Timer_TimeDifference(module.memory_check_timer) > check_interval)
    {
        const uint16_t unused_memory = Board_StackCount();
        DEBUG("Unused memory: %u", unused_memory);

        if (!module.memory_low_flag && unused_memory < LOW_STACK_LIMIT)
        {
            ErrorHandler_LogError(LOW_STACK, unused_memory);
            module.memory_low_flag = true;
            WARNING("Low memory: %u", unused_memory);
        }

        sc_assert(unused_memory > 0);
        module.memory_check_timer = Timer_GetMilliseconds();
    }
}

void assert_fail_handler(const char *file_p, int line_number, const char *expression_p)
{
    UNUSED(expression_p);
    const char *file_name = strrchr_P(file_p, '/');

    libUI_Print("Assert: %i", 2, UI_DOUBLE_ROW_FIRST, line_number);
    libUI_Print_P(file_name, 2, UI_DOUBLE_ROW_SECOND);
    libUI_Update();
}

void handle_corrupt_config(void)
{
    CRITICAL("Corrupt device configuration");
    ErrorHandler_LogError(CORRUPT_CONFIG, 0);

    libUI_Print("Corrupt config", 0, UI_DOUBLE_ROW_FIRST);
    libUI_Print("V:0x%02X, E:0x%02X", 0, UI_DOUBLE_ROW_SECOND, Config_GetVersion(), CORRUPT_CONFIG);
    libUI_Update();

    ErrorHandler_PointOfNoReturn();
}
