/**
 * @file   node_firmware.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2015-11-12 (Last edit)
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

#include "libDebug.h"
#include "libADC.h"
#include "libSPI.h"
#include "libRFM69.h"
#include "libDHT22.h"
#include "libPower.h"

#include "Timer.h"
#include "LED.h"
#include "Sensor.h"
#include "Transceiver.h"
#include "ErrorHandler.h"

#include "RTC.h"
#include "Power.h"

#include "sc_assert.h"

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

    libRFM69_HWInit();
    RTC_InitHW();

    libDebug_Init();
    INFO("Node unit started");
    INFO("Last reset: 0x%02X", mcu_status);

    libADC_Init();
    Timer_Init();
    libSPI_Init(1);
    libADC_Enable(TRUE);
    libDHT22_Init();
    libPower_Init();

    LED_Init();
    RTC_Init();
    Sensor_Init();
    Transceiver_Init();
    Power_Init();

    Power_AddListener(LED_EventHandler);

    INFO("Start up done");
    while (1)
    {
        libDHT22_Update();
        Sensor_Update();
        Transceiver_Update();
        LED_Update();
        Power_Update();
    }
    sc_assert_fail();
    SoftReset();
}