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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "libDebug.h"

#define LIBNAME "libDebug"

void PrintName(char* lib_name);

///
/// @brief Init debug library
///
/// @param  None
/// @return None
///
void libDebug_Init()
{
	#ifdef DEBUG_ENABLE	
	libUART_Init();
	#endif
	DEBUG_STR(LIBNAME, "Init done");
}

void libDebug_Print(const char* text, ...)
{
	va_list args;
	char buffer[64];
	
	va_start(args, text);
	vsprintf(buffer, text, args);
	libUART_SendArray((uint8_t*)buffer, strlen(buffer));
	va_end(args);

	return;
}

///
/// @brief Print debug string
///
/// @param  lib_name Pointer to array with the library name
/// @param  message Pointer to array with the debug message
/// @return None
///
void libDebug_PrintString(char* lib_name, char* message)
{
	PrintName(lib_name);
	
	libUART_SendArray((uint8_t*)message, strlen(message));
	//TODO: Add timestamp
	libUART_SendArray((uint8_t*)"\r\n", 2); 
	return;
}

void libDebug_PrintHex(char*lib_name, uint8_t value)
{
	char data_buffer[12];
	sprintf(data_buffer, "HEX: 0x%02x\r\n", value);
	
	PrintName(lib_name);
	libUART_SendArray((uint8_t*)data_buffer, strlen(data_buffer));
	return;
}

void libDebug_PrintInteger(char* lib_name, int value)
{
	char data_buffer[12];
	itoa(value, data_buffer, 10);
	
	PrintName(lib_name);
	
	libUART_SendArray((uint8_t*)data_buffer, strlen(data_buffer));
	//TODO: Add timestamp
	libUART_SendArray((uint8_t*)"\r\n", 2);
}

void libDebug_PrintUnsignedInteger(char* lib_name, uint32_t value)
{
	char data_buffer[12];
	ultoa(value, data_buffer, 10);
	
	PrintName(lib_name);
	
	libUART_SendArray((uint8_t*)data_buffer, strlen(data_buffer));
	//TODO: Add timestamp
	libUART_SendArray((uint8_t*)"\r\n", 2);
}

void libDebug_PrintBinary(char* lib_name, uint8_t value)
{
	char data_buffer[12];
	uint8_t index;
	
	for (index = 0; index < 8; ++index)
	{
		data_buffer[7-index] = ((value >> index) & 0x01) + 0x30;
	}
	PrintName(lib_name);
	libUART_SendArray((uint8_t*)data_buffer, 8);
	libUART_SendArray((uint8_t*)"\r\n", 2);	
	return;
}


void libDebug_PrintBoolean(char* lib_name, bool value)
{	
	PrintName(lib_name);
	
	if (value == TRUE)
	{
		libUART_SendArray((uint8_t*)"True", 4);
	}
	else
	{	
		libUART_SendArray((uint8_t*)"False", 5);
	}
	//TODO: Add timestamp
}


//**********************Local functions**********************//

void PrintName(char* lib_name)
{
	libUART_SendByte('<');
	libUART_SendArray((uint8_t*)lib_name, strlen(lib_name));
	libUART_SendByte('>');
	libUART_SendByte(' ');	
}