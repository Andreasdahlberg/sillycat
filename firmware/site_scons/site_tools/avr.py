# -*- coding: utf-8 -*
#
# This file is part of SillyCat Development Tools.
#
# SillyCat Development Tools is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# SillyCat Development Tools is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with SillyCat Development Tools.  If not, see <http://www.gnu.org/licenses/>.

__author__ = 'andreas.dahlberg90@gmail.com (Andreas Dahlberg)'

import SCons.Util
import SCons.Tool.cc as cc

COMPILER = 'avr-gcc'
OBJCOPY  = 'avr-objcopy'


def generate(env):
    """Add Builders and construction variables for avr-gcc to an Environment."""
    cc.generate(env)

    env['CC'] = env.Detect(COMPILER)
    env['OBJCOPY'] = env.Detect(OBJCOPY)
    env['SHCCFLAGS'] = SCons.Util.CLVar('$CCFLAGS')
        
    env.Append(BUILDERS = {
        'Elf': _get_elf_builder(),
        'Hex': _get_hex_builder(),
        'EEPROM': _get_eeprom_builder()
    })


def exists(env):
    return True


def _get_elf_builder():
    return SCons.Builder.Builder(
        action=SCons.Action.Action('${CC} -mmcu=${MCU} -O${OPTIMIZATION} -Wl,--gc-sections -o ${TARGET} ${SOURCES}', '${AVRDUDE_ELF_COMSTR}')
    )


def _get_hex_builder():
    return SCons.Builder.Builder(
        action=SCons.Action.Action("${OBJCOPY} -O ihex -R .eeprom ${SOURCES} ${TARGET}", '${AVRDUDE_HEX_COMSTR}')
    )

def _get_eeprom_builder():
    return SCons.Builder.Builder(
        action=SCons.Action.Action('${OBJCOPY} -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 -O ihex ${SOURCES} ${TARGET}', '${AVRDUDE_EEP_COMSTR}')
    )