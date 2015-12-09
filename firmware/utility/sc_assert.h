/**
 * @file   sc_assert.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2015-12-9 (Last edit)
 * @brief  Implementation of sc_assert
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

#ifndef SCASSERT_H_
#define SCASSERT_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include "ErrorHandler.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#if defined(NDEBUG)
#   define sc_assert_fail() ((void)0)
#   define sc_assert(e) ((void)0)
#else
#   define sc_assert_fail() (ErrorHandler_AssertFail(__func__, __FILE__, __LINE__, "sc_assert_fail"))
#   define sc_assert(e) ((e) ? (void)0 : ErrorHandler_AssertFail(__func__, __FILE__, __LINE__, #e))
#endif

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

#endif /* SCASSERT_H_ */