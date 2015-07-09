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


#ifndef LIBDEBUG_H_
#define LIBDEBUG_H_

#ifdef DEBUG_ENABLE
	#define DEBUG(...)					libDebug_Print(__VA_ARGS__)
	#define DEBUG_STR(name, message)	libDebug_PrintString(name, message)
	#define DEBUG_INT(name, num)		libDebug_PrintInteger(name, num)
	#define DEBUG_HEX(name ,num)		libDebug_PrintHex(name, num)
	#define DEBUG_BOOL(name, value)		libDebug_PrintBoolean(name, value)
	#define DEBUG_BINARY(name, num)		libDebug_PrintBinary(name, num)
#else
	#define DEBUG(...)
	#define DEBUG_STR(name, message)
	#define DEBUG_INT(name, num)
	#define DEBUG_HEX(name ,num)
	#define DEBUG_BOOL(name, value)		
	#define DEBUG_BINARY(name, num)
#endif

#include "common.h"
#include "libUART.h"

void libDebug_Init();
void libDebug_PrintString(char* lib_name, char* message);
void libDebug_PrintInteger(char* lib_name, int value);
void libDebug_PrintUnsignedInteger(char* lib_name, uint32_t value);
void libDebug_PrintBoolean(char* lib_name, bool value);
void libDebug_PrintHex(char*lib_name, uint8_t value);
void libDebug_PrintBinary(char* lib_name, uint8_t value);

void libDebug_Print(const char* text, ...);

#endif /* LIBDEBUG_H_ */