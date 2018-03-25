/**
 * @file   libMCP79510.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-03-25 (Last edit)
 * @brief  Implementation of a driver for the MCP79510 RTC.
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

#ifndef LIBMCP79510_H_
#define LIBMCP79510_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

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
 * Initialize the MCP79510 RTC IC.
 */
void libMCP79510_Init(void);

/**
 * Set the SS pin as output and pull high. This function should be called as
 * early as possible in a system with several SPI-devices.
 */
void libMCP79510_HWInit(void);

/**
 * Get hundredth of second.
 *
 * @param hsec_p Pointer to variable where the result will be stored.
 */
void libMCP79510_GetHundredthSecond(uint8_t *hsec_p);

/**
 * Set hundredth of second.
 *
 * @param hsec Hundredth of second.
 *
 * @return True if the value was set, otherwise false.
 */
bool libMCP79510_SetHundredthSecond(uint8_t hsec);

/**
 * Set alarm hundredth of second.
 *
 * Hundredth of second precision is only available for alarm 1.
 *
 * @param hsec Hundredth of second.
 *
 * @return True if the value was set, otherwise false.
 */
bool libMCP79510_SetAlarmHundredthSecond(uint8_t hsec);

/**
 * Get second.
 *
 * @param sec_p Pointer to variable where the result will be stored.
 */
void libMCP79510_GetSecond(uint8_t *sec_p);

/**
 * Set second.
 *
 * @param sec Second.
 *
 * @return True if the value was set, otherwise false.
 */
bool libMCP79510_SetSecond(uint8_t sec);

/**
 * Set alarm second.
 *
 * @param sec         Alarm second.
 * @param alarm_index Alarm index, 0 or 1.
 *
 * @return True if the value was set, otherwise false.
 */
bool libMCP79510_SetAlarmSeconds(uint8_t sec, uint8_t alarm_index);

/**
 * Get minute.
 *
 * @param minute_p Pointer to variable where the result will be stored.
 */
void libMCP79510_GetMinute(uint8_t *minute_p);

/**
 * Set minute.
 *
 * @param minute Minute.
 *
 * @return True if the value was set, otherwise false.
 */
bool libMCP79510_SetMinute(uint8_t minute);

/**
 * Set alarm minute.
 *
 * @param minute      Alarm minute.
 * @param alarm_index Alarm index, 0 or 1.
 *
 * @return True if the value was set, otherwise false.
 */
bool libMCP79510_SetAlarmMinute(uint8_t minute, uint8_t alarm_index);

/**
 * Get hour.
 *
 * @param hour_p Pointer to variable where the result will be stored.
 */
void libMCP79510_GetHour(uint8_t *hour_p);

/**
 * Set hour
 *
 * @param hour Hour.
 *
 * @return True if the value was set, otherwise false.
 */
bool libMCP79510_SetHour(uint8_t hour);

/**
 * Set alarm hour.
 *
 * @param hour        Alarm hour.
 * @param alarm_index Alarm index, 0 or 1.
 *
 * @return True if the value was set, otherwise false.
 */
bool libMCP79510_SetAlarmHour(uint8_t hour, uint8_t alarm_index);

/**
 * Get day.
 *
 * @param day_p Pointer to variable where the result will be stored.
 */
void libMCP79510_GetDay(uint8_t *day_p);

/**
 * Set day
 *
 * @param day Day.
 *
 * @return True if the value was set, otherwise false.
 */
bool libMCP79510_SetDay(uint8_t day);
/**
 * Get date.
 *
 * @param day_p Pointer to variable where the result will be stored.
 */
void libMCP79510_GetDate(uint8_t *date_p);

/**
 * Set date
 *
 * @param date Date.
 *
 * @return True if the value was set, otherwise false.
 */
bool libMCP79510_SetDate(uint8_t date);

/**
 * Set alarm date.
 *
 * @param date        Alarm date.
 * @param alarm_index Alarm index, 0 or 1.
 *
 * @return True if the value was set, otherwise false.
 */
bool libMCP79510_SetAlarmDate(uint8_t date, uint8_t alarm_index);
/**
 * Get month.
 *
 * @param day_p Pointer to variable where the result will be stored.
 */
void libMCP79510_GetMonth(uint8_t *month_p);
/**
 * Set hour
 *
 * @param hour Hour.
 *
 * @return True if the value was set, otherwise false.
 */
bool libMCP79510_SetMonth(uint8_t month);
/**
 * Get year.
 *
 * @param day_p Pointer to variable where the result will be stored.
 */
void libMCP79510_GetYear(uint8_t *year_p);

/**
 * Set year.
 *
 * @param year Years since 2000 (0-99).
 *
 * @return True if the value was set, otherwise false.
 */
bool libMCP79510_SetYear(uint8_t year);

/**
 * Enable/disable 24-hour mode.
 *
 Enable/disable the MCP79510 on-board oscillator.
 */
void libMCP79510_Enable24HourMode(bool enabled);

/**
 * Check if 24-hour mode is enabled.
 *
 * @return True if enabled, otherwise false.
 */
bool libMCP79510_Is24HourMode(void);

/**
 * Enable/disable the MCP79510 on-board oscillator.
 *
 * @param enabled Enable/disable the MCP79510 on-board oscillator.
 */
void libMCP79510_EnableOscillator(bool enabled);

/**
 * Check if the current year is a leap year.
 *
 * @return True if leap year, otherwise false.
 */
bool libMCP79510_IsLeapYear(void);

/**
 * Enabled an alarm.
 *
 * @param enable True for enable, otherwise false.
 * @param alarm  Index of alarm, 0 or 1.
 */
void libMCP79510_EnableAlarm(bool enable, uint8_t alarm);

/**
 * Enable the square wave output.
 *
 * Enables the divided output from the crystal oscillator.
 *
 * @param enable True for enabled, otherwise false.
 */
void libMCP79510_EnableSquareWave(bool enable);

/**
 * Check if the oscillator is running.
 *
 * This flag is set and cleared by hardware. If this flag is set, the
 * oscillator is running. If clear, the oscillator is not running. This flag
 * does not indicate that the oscillator is running at the correct frequency.
 * The flag will wait 32 oscillator cycles before the bit is set.
 *
 * @return True if running, otherwise false.
 */
bool libMCP79510_IsOscillatorRunning(void);

/**
 * Clear the external battery switched flag.
 *
 * This flag is set by hardware when the VCC fails and the VBAT is used to
 * power the oscillator and the RTCC registers. This flag is cleared by
 * software.
 */
void libMCP79510_ClearBatterySwitchFlag(void);

/**
 * Clear the alarm flag.
 *
 * @param alarm_index Index of alarm, 0 or 1.
 */
void libMCP79510_ClearAlarmFlag(uint8_t alarm_index);

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
bool libMCP79510_WriteToSRAM(uint8_t address,
                             const uint8_t *data,
                             uint8_t length);

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
bool libMCP79510_ReadFromSRAM(uint8_t address, uint8_t *data_p, uint8_t length);

/**
 * Clear SRAM.
 *
 * Clear SRAM by reseting all locations to 0x00.
 */
void libMCP79510_ClearSRAM(void);

/**
 * Get unique device ID.
 *
 * Get the unique EUI-48/68 id. ID could be preprogrammed from the factory
 * or defined by the user application.
 *
 * @param eui_p  Pointer to buffer where the ID will be stored.
 * @param length  Length of ID.
 */
void libMCP79510_GetEUI(uint8_t *eui_p, size_t length);

#endif
