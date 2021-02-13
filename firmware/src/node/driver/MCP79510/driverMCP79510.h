/**
 * @file   driverMCP79510.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2021-02-13 (Last edit)
 * @brief  Driver for the MCP79510 RTC.
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

#ifndef DRIVERMCP79510_H_
#define DRIVERMCP79510_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include <stddef.h>
#include "stdint.h"
#include "stdbool.h"
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
 * Initialize the MCP79510 RTC IC.
 *
 * @param pre_fp  Function for preparing a SPI-transfer.
 * @param post_fp Function for ending a SPI-transfer.
 */
void driverMCP79510_Init(libSPI_callback_type pre_fp, libSPI_callback_type post_fp);

/**
 * Get the current time.
 *
 * @param time_p Pointer to variable where the result will be stored.
 */
void driverMCP79510_GetTime(struct driverRTC_time_t *time_p);

/**
 * Set the current time.
 *
 * @param time_p Pointer to struct with time to set.
 *
 * @return True if new time was set successfully, otherwise false.
 */
bool driverMCP79510_SetTime(const struct driverRTC_time_t *time_p);

/**
 * Get hundredth of second.
 *
 * @param hsec_p Pointer to variable where the result will be stored.
 */
void driverMCP79510_GetHundredthSecond(uint8_t *hsec_p);

/**
 * Set hundredth of second.
 *
 * @param hsec Hundredth of second.
 *
 * @return True if the value was set, otherwise false.
 */
bool driverMCP79510_SetHundredthSecond(uint8_t hsec);

/**
 * Set alarm hundredth of second.
 *
 * Hundredth of second precision is only available for alarm 1.
 *
 * @param hsec Hundredth of second.
 *
 * @return True if the value was set, otherwise false.
 */
bool driverMCP79510_SetAlarmHundredthSecond(uint8_t hsec);

/**
 * Get second.
 *
 * @param sec_p Pointer to variable where the result will be stored.
 */
void driverMCP79510_GetSecond(uint8_t *sec_p);

/**
 * Set second.
 *
 * @param sec Second.
 *
 * @return True if the value was set, otherwise false.
 */
bool driverMCP79510_SetSecond(uint8_t sec);

/**
 * Set alarm second.
 *
 * @param sec         Alarm second.
 * @param alarm_index Alarm index, 0 or 1.
 *
 * @return True if the value was set, otherwise false.
 */
bool driverMCP79510_SetAlarmSeconds(uint8_t sec, uint8_t alarm_index);

/**
 * Get minute.
 *
 * @param minute_p Pointer to variable where the result will be stored.
 */
void driverMCP79510_GetMinute(uint8_t *minute_p);

/**
 * Set minute.
 *
 * @param minute Minute.
 *
 * @return True if the value was set, otherwise false.
 */
bool driverMCP79510_SetMinute(uint8_t minute);

/**
 * Set alarm minute.
 *
 * @param minute      Alarm minute.
 * @param alarm_index Alarm index, 0 or 1.
 *
 * @return True if the value was set, otherwise false.
 */
bool driverMCP79510_SetAlarmMinute(uint8_t minute, uint8_t alarm_index);

/**
 * Get hour.
 *
 * @param hour_p Pointer to variable where the result will be stored.
 */
void driverMCP79510_GetHour(uint8_t *hour_p);

/**
 * Set hour
 *
 * @param hour Hour.
 *
 * @return True if the value was set, otherwise false.
 */
bool driverMCP79510_SetHour(uint8_t hour);

/**
 * Set alarm hour.
 *
 * @param hour        Alarm hour.
 * @param alarm_index Alarm index, 0 or 1.
 *
 * @return True if the value was set, otherwise false.
 */
bool driverMCP79510_SetAlarmHour(uint8_t hour, uint8_t alarm_index);

/**
 * Get day.
 *
 * @param day_p Pointer to variable where the result will be stored.
 */
void driverMCP79510_GetDay(uint8_t *day_p);

/**
 * Set day
 *
 * @param day Day.
 *
 * @return True if the value was set, otherwise false.
 */
bool driverMCP79510_SetDay(uint8_t day);
/**
 * Get date.
 *
 * @param day_p Pointer to variable where the result will be stored.
 */
void driverMCP79510_GetDate(uint8_t *date_p);

/**
 * Set date
 *
 * @param date Date.
 *
 * @return True if the value was set, otherwise false.
 */
bool driverMCP79510_SetDate(uint8_t date);

/**
 * Set alarm date.
 *
 * @param date        Alarm date.
 * @param alarm_index Alarm index, 0 or 1.
 *
 * @return True if the value was set, otherwise false.
 */
bool driverMCP79510_SetAlarmDate(uint8_t date, uint8_t alarm_index);

/**
 * Get month.
 *
 * @param day_p Pointer to variable where the result will be stored.
 */
void driverMCP79510_GetMonth(uint8_t *month_p);

/**
 * Set month
 *
 * @param month Month.
 *
 * @return True if the value was set, otherwise false.
 */
bool driverMCP79510_SetMonth(uint8_t month);

/**
 * Get year.
 *
 * @param year_p Pointer to variable where the result will be stored.
 */
void driverMCP79510_GetYear(uint8_t *year_p);

/**
 * Set year.
 *
 * @param year Years since 2000 (0-99).
 *
 * @return True if the value was set, otherwise false.
 */
bool driverMCP79510_SetYear(uint8_t year);

/**
 * Enable/disable 24-hour mode.
 *
 * @param enabled True for enable, otherwise false.
 */
void driverMCP79510_Enable24HourMode(bool enabled);

/**
 * Check if 24-hour mode is enabled.
 *
 * @return True if enabled, otherwise false.
 */
bool driverMCP79510_Is24HourMode(void);

/**
 * Enable/disable the MCP79510 on-board oscillator.
 *
 * @param enabled Enable/disable the MCP79510 on-board oscillator.
 */
void driverMCP79510_EnableOscillator(bool enabled);

/**
 * Check if the current year is a leap year.
 *
 * @return True if leap year, otherwise false.
 */
bool driverMCP79510_IsLeapYear(void);

/**
 * Enabled an alarm.
 *
 * @param enable True for enable, otherwise false.
 * @param alarm  Index of alarm, 0 or 1.
 */
void driverMCP79510_EnableAlarm(bool enable, uint8_t alarm);

/**
 * Enable the square wave output.
 *
 * Enables the divided output from the crystal oscillator.
 *
 * @param enable True for enabled, otherwise false.
 */
void driverMCP79510_EnableSquareWave(bool enable);

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
bool driverMCP79510_IsOscillatorRunning(void);

/**
 * Set oscillator trimming value.
 *
 * Digital trimming can be used to correct for inaccuracies of the external
 * crystal or clock source, up to roughly ±259 ppm.
 *
 * @param trim_value Each step in the trim value equates to adding or
 *                   subtracting two clock pulses from the 32.768 kHz clock
 *                   signal. Allowed range is -255 to 255, outside of this
 *                   interval the min/max is set instead.
 */
void driverMCP79510_SetOscillatorTrimming(int16_t trim_value);

/**
 * Enable external battery switchover.
 *
 * If enabled, the internal circuitry is connected to the VBAT pin. If not
 * enabled then the VBAT pin is disconnected and the only current drain on the
 * external battery is the VBAT pin leakage.
 *
 * @param enable True for enabled, otherwise false.
 */
void driverMCP79510_EnableExternalBattery(bool enable);

/**
 * Check if the external battery switchover is enable.
 *
 * See 'driverMCP79510_EnableExternalBattery' for more details.
 */
bool driverMCP79510_IsExternalBatteryEnabled(void);

/**
 * Clear the external battery switched flag.
 *
 * This flag is set by hardware when the VCC fails and the VBAT is used to
 * power the oscillator and the RTCC registers. This flag is cleared by
 * software.
 */
void driverMCP79510_ClearBatterySwitchFlag(void);

/**
 * Get the external battery switched flag.
 *
 * See 'driverMCP79510_ClearBatterySwitchFlag' for more details.
 */
bool driverMCP79510_GetBatterySwitchFlag(void);

/**
 * Clear the alarm flag.
 *
 * @param alarm_index Index of alarm, 0 or 1.
 */
void driverMCP79510_ClearAlarmFlag(uint8_t alarm_index);

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
bool driverMCP79510_WriteToSRAM(uint8_t address,
                                const void *data_p,
                                uint8_t length
                               );

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
bool driverMCP79510_ReadFromSRAM(uint8_t address, void *data_p, uint8_t length);

/**
 * Clear SRAM.
 *
 * Clear the entire SRAM by reseting all locations to 0x00.
 */
void driverMCP79510_ClearSRAM(void);

/**
 * Get unique device ID.
 *
 * Get the unique EUI-48/68 id. ID could be preprogrammed from the factory
 * or defined by the user application.
 *
 * @param eui_p  Pointer to buffer where the ID will be stored.
 * @param length  Length of ID.
 */
void driverMCP79510_GetEUI(uint8_t *eui_p, size_t length);

#endif
