/**
 * @file   sc_assert.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2017-03-30 (Last edit)
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


//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#ifndef UNIT_TESTING

#if defined(NDEBUG)
#define sc_assert_fail() ((void)0)
#define sc_assert(e) ((void)0)
#else
#include <avr/pgmspace.h>
#include "ErrorHandler.h"
#define sc_assert_fail() (ErrorHandler_AssertFail(PSTR(__FILE__), __LINE__, \
    PSTR("sc_assert_fail")))
#define sc_assert(e) ((e) ? (void)0 : ErrorHandler_AssertFail(PSTR(__FILE__), \
    __LINE__, PSTR(#e)))
#endif
#else
extern void mock_assert(const int result, const char* const expression,
                        const char * const file, const int line);
#define sc_assert_fail() mock_assert(0, "sc_assert_fail", __FILE__, __LINE__);
#define sc_assert(e) mock_assert((int)(e), #e, __FILE__, __LINE__);
#endif

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

#endif /* SCASSERT_H_ */
