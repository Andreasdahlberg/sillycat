/**
 * @file   driverInputCapture.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-11-12 (Last edit)
 * @brief  ATmega328 Input Capture driver.
 *
 * Driver for the ATmega328 timer input capture mode.
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

#ifndef DRIVERINPUTCAPTURE_H_
#define DRIVERINPUTCAPTURE_H_

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

typedef void (*driverInputCapture_isr_t)(uint16_t counter_value);

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

/**
 * Initialize the Input capture driver.
 *
 * Sets the hardware to a known default state(input capture disabled).
 */
void driverInputCapture_Init(void);

/**
 * Register a interrupt handler function.
 *
 * This function is called when the TIMER1_CAPT interrupt is triggered and
 * their is a new counter value to read.
 *
 * @param interrupt_handler Function to call when an interrupt is triggered.
 */
void driverInputCapture_RegisterInterruptHandler(driverInputCapture_isr_t interrupt_handler);

/**
 * Enable the input capture mode.
 */
void driverInputCapture_Enable(void);

/**
 * Disable the input capture mode.
 */
void driverInputCapture_Disable(void);

/**
 * Enable the input capture noise canceling.
 *
 * When the noise canceler is activated, the input from the Input Capture
 * pin (ICP1) is filtered. The filter function requires four successive equal
 * valued samples of the ICP1 pin for changing its output. The Input Capture
 * is therefore delayed by four Oscillator cycles when the noise canceler is
 * enabled.
 */
void driverInputCapture_EnableNoiseCanceling(void);

/**
 * Disable the input capture noise canceling.
 *
 * Refer to 'driverInputCapture_EnableNoiseCanceling()' for more information.
 */
void driverInputCapture_DisableNoiseCanceling(void);

#endif
