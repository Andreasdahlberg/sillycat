/**
 * @file   guiInterface.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2015-11-10 (Last edit)
 * @brief  Implementation of guiInterface
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

#include "libDebug.h"
#include "libUI.h"
#include "Interface.h"
#include "guiInterface.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 32

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

void guiInterface_DrawViewIndicator(indicator_position_type position)
{
    switch (position)
    {
        case INDICATOR_POS_RIGHT:
            libUI_DrawLine(DISPLAY_WIDTH - 1, 0, DISPLAY_WIDTH - 1,
                           DISPLAY_HEIGHT - 1);
            break;

        case INDICATOR_POS_LEFT:
            libUI_DrawLine(0, 0, 0, DISPLAY_HEIGHT - 1);
            break;

        case INDICATOR_POS_TOP:
        case INDICATOR_POS_BOTTOM:
        default:
            WARNING("Invalid indicator position: %u", (uint8_t)position);
            break;
    }
    return;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////
