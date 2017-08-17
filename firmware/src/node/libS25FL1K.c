/**
 * @file   libS25FL1K.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2017-08-17 (Last edit)
 * @brief  Implementation of the S25FL1-K low level driver.
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

//NOTE: Include common.h before all other headers
#include "common.h"

#include <util/delay.h>

#include "libSPI.h"
#include "libS25FL1K.h"
#include "libS25FL1K_reg.h"
#include "libS25FL1K_HAL.h"

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

static void PreCallback(void);
static void PostCallback(void);

//////////////////////////////////////////////////////////////////////////
//INTERUPT SERVICE ROUTINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

///
/// @brief Init the hardware required by the flash memory.
///
/// @param  None
/// @return None
///
void libS25FL1K_InitHW(void)
{
    InitCS();
    ReleaseCS();

    InitWP();
    PullWP();

    return;
}

///
/// @brief Restore the device to its initial power up state.
///
/// If a software reset is initiated during a erase, program or writing of a
/// register operation the data in that sector, page or register is not stable
/// and the operation that was interrupted needs to be initiated again.
/// When the device is in deep power-down mode it is protected from a software
/// reset, the software reset commands are ignored and have no effect. To reset
/// the device release it from power down first.
///
/// @param  None
/// @return None
///
void libS25FL1K_Reset(void)
{
    uint8_t cmd_sequence[] = {CMD_SW_RESET_ENABLE, CMD_SW_RESET};
    libSPI_Write(cmd_sequence, sizeof(cmd_sequence), PreCallback, PostCallback);

    return;
}

///
/// @brief Put the device into deep power down state.
///
/// After CS is driven high, the power down state will entered within 3 µs.
/// While in the power down state only the Release from Deep-Power-Down command,
/// which restores the device to normal operation, will be recognized. All other
/// commands are ignored. This includes the Read Status Register command,
/// which is always available during normal operation. Ignoring all but one
/// command also makes the Power Down state a useful condition for securing
/// maximum write protection.
///
/// @param  None
/// @return None
///
void libS25FL1K_EnterDeepPowerDown(void)
{
    libSPI_WriteByte(CMD_DEEP_POWER_DOWN, PreCallback, PostCallback);

    return;
}

///
/// @brief Release the device from deep power down state.
///
/// @param  None
/// @return None
///
void libS25FL1K_ExitDeepPowerDown(void)
{
    libSPI_WriteByte(CMD_RELEASE_POWER_DOWN, PreCallback, PostCallback);

    // After the release command has been sent, CS must be kept high until
    // standby mode is ready. According to the S25FL1-K_00_03 datasheet, table 5.8,
    // this can take up to 3 µs.
    _delay_us(3);

    return;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

///
/// @brief Prepare for SPI read/write.
///

/// Sets the correct SPI mode and pulls SS low so the device is selected.
///
/// @param  None
/// @return None
///
static void PreCallback(void)
{
    libSPI_SetMode(SPIMODE);
    PullCS();

    return;
}

///
/// @brief Cleanup after SPI read/write.
///
/// Pulls SS high so the device is deselected. The SPI-mode is left as it is.
///
/// @param  None
/// @return None
///
static void PostCallback(void)
{
    ReleaseCS();

    return;
}
