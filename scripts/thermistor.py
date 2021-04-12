#!/usr/bin/env python3
# -*- coding: utf-8 -*
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import math

# All temperatures are in kelvin if nothing else is specified.
B = 3380.0
R0 = 10000.0
T0 = 298.15
R2 = 10000.0
VIN = 3.3


def kelvin_to_celsius(temperature):
    assert temperature >= -273.15

    return temperature - 273.15


def adc_value_to_voltage(adc_value):
    assert adc_value >= 0 and adc_value <= 1023

    return VIN / 1023.0 * adc_value


def create_lookup_table(start, stop, step):
    rinf = R0 * math.pow(math.e, -B / T0)

    lut = []

    for v in range(start, stop, step):
        divider_voltage = adc_value_to_voltage(v)
        print('Voltage:     {:.3f} V'.format(divider_voltage))

        if divider_voltage > 0:
            ri = VIN * R2 / divider_voltage - R2
            t = B / (math.log(ri/rinf))

            print('Temperature: {:.1f} °C'.format(kelvin_to_celsius(t)))
            print('Resistance:  {:.2f} Ω'.format(ri))

            lut.append(round(kelvin_to_celsius(t) * 10))
        else:
            lut.append(-1000)

            print('Temperature: N/A °C')
            print('Resistance: ∞ Ω')

    return lut


def print_lut_stats(lut):
    print('-----------------------')
    print('Lookup table statistics')
    print('Length: {}'.format(len(lut)))
    print('Size: {:.1f} KiB'.format(len(lut) * 2 / 1024))
    print('Range: {:.1f} °C to {:.1f} °C'.format(min(lut) / 10, max(lut) / 10))


def _main():
    lut = create_lookup_table(120, 939, 1)
    print_lut_stats(lut)


if __name__ == '__main__':
    _main()
