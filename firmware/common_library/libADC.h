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


#ifndef LIBADC_H_
#define LIBADC_H_

void libADC_Init(void);
void libADC_Update(void);
void libADC_Enable(bool mode);
uint8_t libADC_GetCurrentInput();
function_status libADC_EnableInput(uint8_t index, bool mode);
function_status libADC_GetSample(uint8_t index, uint16_t *sample_value);

#endif /* LIBADC_H_ */