/**
 * @file   driverPEC11.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-09-13 (Last edit)
 * @brief  Driver for Bourns PEC11 series rotary encoder with push button.
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

#ifndef DRIVERPEC11_H_
#define DRIVERPEC11_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include <stdbool.h>
#include <stdint.h>

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

/**
 * Initialize the driver and the required inputs.
 */
void driverPEC11_Init(void);

/**
 * Check if any left rotations has been recorded and
 * remove one rotation if possible.
 *
 * @return True if a rotation was present and removed.
 */
bool driverPEC11_PopLeftRotation(void);

/**
 * Check if any right rotations has been recorded and
 * remove one rotation if possible.
 *
 * @return True if a rotation was present and removed.
 */
bool driverPEC11_PopRightRotation(void);

/**
 * Check if any brief pushes has been recorded and
 * remove one push if possible.
 *
 * @return True if a brief push was present and removed.
 */
bool driverPEC11_PopBriefPush(void);

/**
 * Check if any extended pushes has been recorded and
 * remove one push if possible.
 *
 * @return True if a extended push was present and removed.
 */
bool driverPEC11_PopExtendedPush(void);

#endif
