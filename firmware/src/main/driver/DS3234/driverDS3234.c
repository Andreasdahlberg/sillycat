/**
 * @file   driverDS3234.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2021-04-19 (Last edit)
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

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

#include "common.h"
#include "libDebug.h"
#include "Bit.h"
#include "driverDS3234.h"
#include "driverDS3234Registers.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define SRAM_SIZE 256

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

struct module_t
{
    libSPI_callback_type PreSPI;
    libSPI_callback_type PostSPI;
};

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static struct module_t module;

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

static inline bool IsRegisterAddressValid(uint8_t address);
static void WriteRegister(uint8_t address, uint8_t data);
static uint8_t ReadRegister(uint8_t address);
static void ReadRegisters(uint8_t address, void *dst_p, size_t length);
static void ClearBit(uint8_t register_address, uint8_t bit_index);
static void SetBit(uint8_t register_address, uint8_t bit_index);
static void GetDecimalRegisterValue(uint8_t address, uint8_t *value_p);
static void SetDecimalRegisterValue(uint8_t address, uint8_t value);
static inline bool IsSRAMParametersValid(uint8_t address, size_t length);
static inline void SetDefaultAlarmMask(void);
static inline bool IsNegative(int16_t value);

#ifdef DEBUG_ENABLE
static void DumpRegisterValues(void) __attribute__((unused));
#endif

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void driverDS3234_Init(libSPI_callback_type pre_p, libSPI_callback_type post_p)
{
    sc_assert(pre_p != NULL);
    sc_assert(post_p != NULL);

    module.PreSPI = pre_p;
    module.PostSPI = post_p;

    driverDS3234_SetAgingOffset(0);
    driverDS3234_EnableOscillator(true);

    INFO("DS3234 initialized");
}

void driverDS3234_EnableOscillator(bool enabled)
{
    if (enabled)
    {
        ClearBit(REG_CONTROL, CONREG_EOSC);
    }
    else
    {
        SetBit(REG_CONTROL, CONREG_EOSC);
    }
}

void driverDS3234_SetAgingOffset(int8_t offset)
{
    WriteRegister(REG_AGING_OFFSET, offset);
}

void driverDS3234_EnableSquareWaveOutput(bool enabled)
{
    if (enabled)
    {
        ClearBit(REG_CONTROL, CONREG_INTCN);
    }
    else
    {
        SetBit(REG_CONTROL, CONREG_INTCN);
    }
}

void driverDS3234_GetTime(struct driverRTC_time_t *time_p)
{
    sc_assert(time_p != NULL);

    /* Read all timekeeping registers in one operation to avoid rollover issues. */
    const uint8_t start_address = REG_SECONDS;
    const uint8_t end_address = REG_YEAR;
    const uint8_t number_of_timekeeping_registers = end_address - start_address + 1;
    uint8_t timekeeping_registers[number_of_timekeeping_registers];
    ReadRegisters(start_address, timekeeping_registers, number_of_timekeeping_registers);

    const uint8_t hour_mask = driverDS3234_Is24HourMode() ? HOUR_24_MASK : HOUR_12_MASK;
    time_p->second = BCDToDecimal(timekeeping_registers[REG_SECONDS]);
    time_p->minute = BCDToDecimal(timekeeping_registers[REG_MINUTES]);
    time_p->hour = BCDToDecimal(timekeeping_registers[REG_HOUR] & hour_mask);
    time_p->day = BCDToDecimal(timekeeping_registers[REG_DAY]);
    time_p->date = BCDToDecimal(timekeeping_registers[REG_DATE]);
    time_p->month = BCDToDecimal(timekeeping_registers[REG_MONTH_CENTURY] & MONTH_MASK);
    time_p->year = BCDToDecimal(timekeeping_registers[REG_YEAR]);
}

bool driverDS3234_SetTime(const struct driverRTC_time_t *time_p)
{
    sc_assert(time_p != NULL);

    /**
     * Write seconds first to reset the countdown chain and avoid rollover issues.
     */
    return driverDS3234_SetSecond(time_p->second) &&
           driverDS3234_SetMinute(time_p->minute) &&
           driverDS3234_SetHour(time_p->hour) &&
           driverDS3234_SetDate(time_p->date) &&
           driverDS3234_SetMonth(time_p->month) &&
           driverDS3234_SetYear(time_p->year);
}

void driverDS3234_GetTemperature(int16_t *temperature_p)
{
    const int16_t scaling_factor = 100;
    const int8_t msb = (int8_t)ReadRegister(REG_TEMP_MSB);
    *temperature_p = (int16_t)msb * scaling_factor;

    const uint8_t fractional_resolution = 25;
    const uint8_t lsb = ReadRegister(REG_TEMP_LSB);
    const int8_t fractional_part = (int8_t)((lsb >> 6) * fractional_resolution);
    *temperature_p += IsNegative(*temperature_p) ? fractional_part * -1 : fractional_part;
}

void driverDS3234_GetYear(uint8_t *year_p)
{
    GetDecimalRegisterValue(REG_YEAR, year_p);
}

bool driverDS3234_SetYear(uint8_t year)
{
    const uint8_t max_year = 99;
    bool status = false;

    if (year <= max_year)
    {
        SetDecimalRegisterValue(REG_YEAR, year);
        status = true;
    }

    return status;
}

void driverDS3234_GetMonth(uint8_t *month_p)
{
    const uint8_t register_data = ReadRegister(REG_MONTH_CENTURY);
    *month_p = BCDToDecimal(register_data & MONTH_MASK);
}

bool driverDS3234_SetMonth(uint8_t month)
{
    const uint8_t max_month = 13;
    bool status = false;

    if (month > 0 && month < max_month)
    {
        uint8_t register_data = ReadRegister(REG_MONTH_CENTURY);
        register_data = ((register_data & 0x80) | DecimalToBCD(month));

        WriteRegister(REG_MONTH_CENTURY, register_data);
        status = true;
    }
    return status;
}

void driverDS3234_GetDate(uint8_t *date_p)
{
    GetDecimalRegisterValue(REG_DATE, date_p);
}

bool driverDS3234_SetDate(uint8_t date)
{
    const uint8_t max_date = 32;
    bool status = false;

    if (date > 0 && date < max_date)
    {
        SetDecimalRegisterValue(REG_DATE, date);
        status = true;
    }
    return status;
}

void driverDS3234_GetDay(uint8_t *day_p)
{
    GetDecimalRegisterValue(REG_DAY, day_p);
}

bool driverDS3234_SetDay(uint8_t day)
{
    const uint8_t max_day = 8;
    bool status = false;

    if (day > 0 && day < max_day)
    {
        SetDecimalRegisterValue(REG_DAY, day);
        status = true;
    }
    return status;
}

void driverDS3234_GetHour(uint8_t *hour_p)
{
    const uint8_t mask = driverDS3234_Is24HourMode() ? HOUR_24_MASK : HOUR_12_MASK;
    const uint8_t register_data = ReadRegister(REG_HOUR);
    *hour_p = BCDToDecimal(register_data & mask);
}

bool driverDS3234_SetHour(uint8_t hour)
{
    const bool is_24_hour_mode = driverDS3234_Is24HourMode();
    const uint8_t min_hour = is_24_hour_mode ? 0 : 1;
    const uint8_t max_hour = is_24_hour_mode ? 23 : 12;

    bool status = false;
    if (hour >= min_hour && hour <= max_hour)
    {
        const uint8_t mask = is_24_hour_mode ? ~HOUR_24_MASK : ~HOUR_12_MASK;
        uint8_t register_data = (ReadRegister(REG_HOUR) & mask) | DecimalToBCD(hour);
        WriteRegister(REG_HOUR, register_data);

        status = true;
    }

    return  status;
}

void driverDS3234_GetMinute(uint8_t *minute_p)
{
    GetDecimalRegisterValue(REG_MINUTES, minute_p);
}

bool driverDS3234_SetMinute(uint8_t minute)
{
    const uint8_t max_minute = 59;

    bool status = false;
    if (minute <= max_minute)
    {
        SetDecimalRegisterValue(REG_MINUTES, minute);
        status = true;
    }

    return status;
}

void driverDS3234_GetSecond(uint8_t *second_p)
{
    return GetDecimalRegisterValue(REG_SECONDS, second_p);
}

bool driverDS3234_SetSecond(uint8_t second)
{
    const uint8_t max_second = 59;

    bool status = false;
    if (second <= max_second)
    {
        SetDecimalRegisterValue(REG_SECONDS, second);
        status = true;
    }

    return status;
}

bool driverDS3234_Is24HourMode(void)
{
    const uint8_t register_data = ReadRegister(REG_HOUR);
    return !Bit_Get(HOUR_MODE_BIT, &register_data);
}

void driverDS3234_Enable24HourMode(bool enabled)
{
    if (enabled)
    {
        ClearBit(REG_HOUR, HOUR_MODE_BIT);
    }
    else
    {
        SetBit(REG_HOUR, HOUR_MODE_BIT);
    }
}

void driverDS3234_EnableAlarm(bool enable, uint8_t alarm_index)
{
    sc_assert(alarm_index < 2);

    if (enable == true)
    {
        SetDefaultAlarmMask();
        SetBit(REG_CONTROL, alarm_index);
    }
    else
    {
        ClearBit(REG_CONTROL, alarm_index);
    }
}

void driverDS3234_ClearAlarmFlag(uint8_t alarm_index)
{
    sc_assert(alarm_index < 2);

    ClearBit(REG_CONTROL_STATUS, alarm_index);
}

bool driverDS3234_SetAlarmDate(uint8_t date, uint8_t alarm_index)
{
    sc_assert(alarm_index < 2);

    const uint8_t register_address = (alarm_index == 0) ? REG_ALARM_1_DAY_DATE : REG_ALARM_2_DAY_DATE;
    const uint8_t max_date = 31;
    bool status = false;

    if (date > 0 && date <= max_date)
    {
        SetDecimalRegisterValue(register_address, date);
        status = true;
    }
    return status;
}

bool driverDS3234_SetAlarmHour(uint8_t hour, uint8_t alarm_index)
{
    sc_assert(alarm_index < 2);

    const uint8_t register_address = (alarm_index == 0) ? REG_ALARM_1_HOURS : REG_ALARM_2_HOURS;
    const bool is_24_hour_mode = driverDS3234_Is24HourMode();
    const uint8_t min_hour = is_24_hour_mode ? 0 : 1;
    const uint8_t max_hour = is_24_hour_mode ? 23 : 12;
    bool status = false;

    if (hour >= min_hour && hour <= max_hour)
    {
        SetDecimalRegisterValue(register_address, hour);
        status = true;
    }
    return status;
}

bool driverDS3234_SetAlarmMinute(uint8_t minute, uint8_t alarm_index)
{
    sc_assert(alarm_index < 2);

    const uint8_t register_address = (alarm_index == 0) ? REG_ALARM_1_MINUTES : REG_ALARM_2_MINUTES;
    const uint8_t max_minute = 59;
    bool status = false;

    if (minute <= max_minute)
    {
        SetDecimalRegisterValue(register_address, minute);
        status = true;
    }
    return status;
}

bool driverDS3234_SetAlarmSecond(uint8_t second)
{
    const uint8_t max_second = 59;
    bool status = false;

    if (second <= max_second)
    {
        SetDecimalRegisterValue(REG_ALARM_1_SECONDS, second);
        status = true;
    }
    return status;
}

bool driverDS3234_WriteToSRAM(uint8_t address, const void *data_p, size_t length)
{
    sc_assert(data_p != NULL);

    bool status = false;

    if (IsSRAMParametersValid(address, length))
    {
        WriteRegister(REG_SRAM_ADDRESS, address);
        for (size_t i = 0; i < length; ++i)
        {
            //SRAM address is auto incremented after each write
            WriteRegister(REG_SRAM_DATA, ((const uint8_t *)data_p)[i]);
        }
        status = true;
    }
    return status;
}

bool driverDS3234_ReadFromSRAM(uint8_t address, void *data_p, size_t length)
{
    sc_assert(data_p != NULL);

    bool status = false;

    if (IsSRAMParametersValid(address, length))
    {
        WriteRegister(REG_SRAM_ADDRESS, address);
        for (size_t i = 0; i < length; ++i)
        {
            //SRAM address is auto incremented after each read
            ((uint8_t *)data_p)[i] = ReadRegister(REG_SRAM_DATA);
        }
        status = true;
    }
    return status;
}

void driverDS3234_ClearSRAM(void)
{
    WriteRegister(REG_SRAM_ADDRESS, 0x00);
    for (size_t i = 0; i < SRAM_SIZE; ++i)
    {
        //SRAM address is auto incremented after each write
        WriteRegister(REG_SRAM_DATA, 0x00);
    }
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static inline bool IsRegisterAddressValid(uint8_t address)
{
    return (address <= REG_SRAM_DATA);
}

static void WriteRegister(uint8_t address, uint8_t data)
{
    sc_assert(IsRegisterAddressValid(address));

    libSPI_WriteByte(address | WRITE_ADDRESS, module.PreSPI, NULL);
    libSPI_WriteByte(data, NULL, module.PostSPI);
}

static uint8_t ReadRegister(uint8_t address)
{
    sc_assert(IsRegisterAddressValid(address));

    uint8_t data;
    libSPI_WriteByte(address | READ_ADDRESS, module.PreSPI, NULL);
    libSPI_ReadByte(&data, NULL, module.PostSPI);

    return data;
}

static void ReadRegisters(uint8_t address, void *dst_p, size_t length)
{
    sc_assert(IsRegisterAddressValid(address));

    libSPI_WriteByte(address | READ_ADDRESS, module.PreSPI, NULL);
    libSPI_Read(dst_p, length, NULL, module.PostSPI);
}

static void ClearBit(uint8_t register_address, uint8_t bit_index)
{
    uint8_t register_data = ReadRegister(register_address);
    register_data &= ~(1 << bit_index);
    WriteRegister(register_address, register_data);
}

static void SetBit(uint8_t register_address, uint8_t bit_index)
{
    uint8_t register_data = ReadRegister(register_address);
    register_data |= (1 << bit_index);
    WriteRegister(register_address, register_data);
}

static void GetDecimalRegisterValue(uint8_t address, uint8_t *value_p)
{
    uint8_t register_data = ReadRegister(address);
    *value_p = BCDToDecimal(register_data);
}

static void SetDecimalRegisterValue(uint8_t address, uint8_t value)
{
    /* Only values less then 100 is accepted since a byte can only hold two-digits of BCD data. */
    sc_assert(value < 100);

    uint8_t register_data = DecimalToBCD(value);
    WriteRegister(address, register_data);
}

static inline bool IsSRAMParametersValid(uint8_t address, size_t length)
{
    return length <= SRAM_SIZE && (size_t)address + length <= SRAM_SIZE;
}

static inline void SetDefaultAlarmMask(void)
{
    /* Alarm when hours, minutes, and seconds match */
    WriteRegister(REG_ALARM_1_DAY_DATE, 0x80);
}

static inline bool IsNegative(int16_t value)
{
    return value < 0;
}

#ifdef DEBUG_ENABLE
static void DumpRegisterValues(void)
{

    DEBUG("**DS3234 registers**\r\n");
    for (uint8_t reg_addr = 0x00; reg_addr <= REG_DISABLE_TEMP; ++ reg_addr)
    {
        uint8_t reg_value = ReadRegister(reg_addr);
        DEBUG("Addr: 0x%02X, Value: 0x%02X\r\n", reg_addr, reg_value);

    }
    return;
}
#endif
