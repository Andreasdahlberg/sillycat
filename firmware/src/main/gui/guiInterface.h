/**
 * @file   guiInterface.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2017-02-13 (Last edit)
 * @brief  Header of guiInterface
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

#ifndef GUIINTERFACE_H_
#define GUIINTERFACE_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

typedef enum
{
    INDICATOR_POS_RIGHT = 0,
    INDICATOR_POS_LEFT,
    INDICATOR_POS_TOP,
    INDICATOR_POS_BOTTOM,

} indicator_position_type;

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

void guiInterface_DrawViewIndicator(indicator_position_type position);

#endif
