/**
 * @file   main_firmware.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2016-01-31 (Last edit)
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

#include "guiRTC.h"
#include "guiSensor.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

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
    //NOTE: The first init called will be the root view.
    guiRTC_Init();
    guiSensor_Init();
    Nodes_Init();

    libInput_SetCallbacks(Interface_NextView, Interface_PreviousView,
                          Interface_ActivateView);

    INFO("Start up done");

    while (1)
    {
        libADC_Update();
        libInput_Update();
        Sensor_Update();

        Transceiver_Update();
        Interface_Update();
    }

    CRITICAL("Main loop exit");
    SoftReset();
}