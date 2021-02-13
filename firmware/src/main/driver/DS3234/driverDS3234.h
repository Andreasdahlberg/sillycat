/**
 * @file   driverDS3234.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2021-02-13 (Last edit)
 * @brief  Driver for the DS3234 RTC
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

#ifndef DRIVERDS3234_H_
#define DRIVERDS3234_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "libSPI.h"
#include "driverRTC.h"

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
 * Initialize the DS3234 RTC IC.
 *
 * @param pre_p  Function for preparing a SPI-transfer.
 * @param post_p Function for ending a SPI-transfer.
 */
void driverDS3234_Init(libSPI_callback_type pre_p, libSPI_callback_type post_p);

/**
 * Get the current time.
 *
 * @param time_p Pointer to variable where the result will be stored.
 */
void driverDS3234_GetTime(struct driverRTC_time_t *time_p);

/**
 * Get the chip-temperature.
 *
 * This function does not start a new temperature conversion so the temperature is
 * only updated once every 64 seconds(default TCXO sample rate).
 *
 * @param temperature_p Pointer to variable where the temperature will be stored. The
 *                      temperature is scaled by 100, e.g. 2550 is equal to 25.5 degrees
 *                      Celsius.
 */
void driverDS3234_GetTemperature(int16_t *temperature_p);

/**
 * Get the current year.
 *
 * @param year_p Pointer to variable where the result will be stored(0-99).
 */
void driverDS3234_GetYear(uint8_t *year_p);

/**
 * Set the year.
 *
 * @param year Years since 2000 (0-99).
 *
 * @return True if the value was set, otherwise false.
 */
bool driverDS3234_SetYear(uint8_t year);

/**
 * Get the current month.
 *
 * @param month_p Pointer to variable where the result will be stored(1-12).
 */
void driverDS3234_GetMonth(uint8_t *month_p);

/**
 * Set the month.
 *
 * @param month Month(1-12).
 *
 * @return True if the value was set, otherwise false.
 */
bool driverDS3234_SetMonth(uint8_t month);

/**
 * Get the current date.
 *
 * @param date_p Pointer to variable where the result will be stored(1-31).
 */
void driverDS3234_GetDate(uint8_t *date_p);

/**
 * Set the date.
 *
 * @param date Date(1-31).
 *
 * @return True if the value was set, otherwise false.
 */
bool driverDS3234_SetDate(uint8_t date);

/**
 * Get the current day of the week.
 *
 * @param date_p Pointer to variable where the result will be stored(1-7).
 */
void driverDS3234_GetDay(uint8_t *day_p);

/**
 * Set the day of the week.
 *
 * @param day Day of the week(1-7).
 */
bool driverDS3234_SetDay(uint8_t day);

/**
 * Get the current hour.
 *
 * The range of the returned value depends on the set hour mode, 12h or 24h.
 *
 * @param hour_p Pointer to variable where the result will be stored(0-23).
 */
void driverDS3234_GetHour(uint8_t *hour_p);

/**
 * Set the hour.
 *
 * @param hour Hour(0-23).
 *
 * @return True if the value was set, otherwise false.
 */
bool driverDS3234_SetHour(uint8_t hour);

/**
 * Get the current minute.
 *
 * @param minute_p Pointer to variable where the result will be stored(0-59).
 */
void driverDS3234_GetMinute(uint8_t *minute_p);

/**
 * Set the minute.
 *
 * @param minute Minute(0-59).
 *
 * @return True if the value was set, otherwise false.
 */
bool driverDS3234_SetMinute(uint8_t minute);

/**
 * Get the current second.
 *
 * @param second_p Pointer to variable where the result will be stored(0-59).
 */
void driverDS3234_GetSecond(uint8_t *second_p);

/**
 * Set the second.
 *
 * @param second Second(0-59).
 *
 * @return True if the value was set, otherwise false.
 */
bool driverDS3234_SetSecond(uint8_t second);

/**
 * Check if 24-hour mode is enabled.
 *
 * @return True if enabled, otherwise false.
 */
bool driverDS3234_Is24HourMode(void);

/**
 * Enable/disable 24-hour mode.
 *
 * @param enabled True for enable, otherwise false.
 */
void driverDS3234_Enable24HourMode(bool enabled);

/**
 * Enable an alarm.
 *
 * @param enable True for enable, otherwise false.
 * @param alarm_index Index of alarm, 0 or 1.
 */
void driverDS3234_EnableAlarm(bool enable, uint8_t alarm_index);

/**
 * Clear the alarm flag.
 *
 * @param alarm_index Index of alarm, 0 or 1.
 */
void driverDS3234_ClearAlarmFlag(uint8_t alarm_index);

/**
 * Set the alarm date.
 *
 * @param date Alarm date(1-31).
 * @param alarm_index Alarm index, 0 or 1.
 *
 * @return True if the value was set, otherwise false.
 */
bool driverDS3234_SetAlarmDate(uint8_t date, uint8_t alarm_index);

/**
 * Set the alarm hour.
 *
 * @param hour Alarm hour(0-23).
 * @param alarm_index Alarm index, 0 or 1.
 *
 * @return True if the value was set, otherwise false.
 */
bool driverDS3234_SetAlarmHour(uint8_t hour, uint8_t alarm_index);

/**
 * Set the alarm minute.
 *
 * @param minute Alarm minute(0-59).
 * @param alarm_index Alarm index, 0 or 1.
 *
 * @return True if the value was set, otherwise false.
 */
bool driverDS3234_SetAlarmMinute(uint8_t minute, uint8_t alarm_index);

/**
 * Set the alarm second.
 *
 * This is only available for Alarm 1(index 0).
 *
 * @param second Alarm second(0-59).
 *
 * @return True if the value was set, otherwise false.
 */
bool driverDS3234_SetAlarmSecond(uint8_t second);

/**
 * Write data to SRAM.
 *
 * Write data to SRAM. The SRAM is powered by the RTC battery(if present) so
 * data is persistent between power cycles.
 *
 * @param address Address in SRAM to write. Virtual address starting at zero.
 * @param data_p  Pointer to data to write.
 * @param length  Number of bytes to write.
 *
 * @return True if address and length is valid, otherwise false.
 */
bool driverDS3234_WriteToSRAM(uint8_t address, const void *data_p, size_t length);

/**
 * Read data from SRAM.
 *
 * Read data from SRAM. The SRAM is powered by the RTC battery(if present) so
 * data is persistent between power cycles.
 *
 * @param address Address in SRAM to read. Virtual address starting at zero.
 * @param data_p  Pointer to buffer where data will be stored.
 * @param length  Number of bytes to read.
 *
 * @return True if address and length is valid, otherwise false.
 */
bool driverDS3234_ReadFromSRAM(uint8_t address, void *data_p, size_t length);

/**
 * Clear SRAM.
 *
 * Clear the entire SRAM by reseting all locations to 0x00.
 */
void driverDS3234_ClearSRAM(void);

#endif
