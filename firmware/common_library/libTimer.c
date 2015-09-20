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

#include <avr/interrupt.h>
#include <util/atomic.h>

#define LIBNAME "libTimer"
#define TIMERMAX 4294967295

volatile uint32_t system_timer;

ISR(TIMER1_COMPA_vect)
{
	++system_timer;
}


///
/// @brief Init the Timer library
///
/// @param  None
/// @return None
///
void libTimer_Init()
{
	system_timer = 0;
	
	TCCR1B |= (1 << WGM12);
	TIMSK1 |= (1 << OCIE1A);
		
	sei();
		
	OCR1A = 1000;
	TCCR1B |= (1 << CS11); // Fosc/8
}


///
/// @brief Resets the system timer
///
/// @param  None
/// @return None
///
void libTimer_Reset()
{
	system_timer = 0;
}


///
/// @brief Get the current time(ms)
///
/// @param  None
/// @return uint32_t The current system time in milliseconds
///
uint32_t libTimer_GetMilliseconds()
{
    uint32_t current_timer;
    
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        current_timer = system_timer;
    }
    
	return current_timer;
}

///
/// @brief Get the current time(s)
///
/// @param  None
/// @return uint32_t The current system time in seconds
///
uint32_t libTimer_GetSeconds()
{
	return (libTimer_GetMilliseconds() / 1000);
}


///
/// @brief Get the difference between the system timer and the supplied timestamp. 
///
/// @param  time_ms Timestamp in ms to compare with system timer
/// @return uint32_t The difference in ms beetween timestamp and system timer
///
uint32_t libTimer_TimeDifference(uint32_t time_ms)
{
	uint32_t time_difference;
	uint32_t current_time = libTimer_GetMilliseconds();
	
	//Check for timer overflow	
	if (time_ms > current_time)
	{
		time_difference = (TIMERMAX - time_ms) + current_time;
	}
	else
	{
		time_difference = current_time - time_ms;
	}
	return time_difference;
}
