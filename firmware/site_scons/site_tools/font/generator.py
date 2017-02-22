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

import os
import sys
import math
import string
import datetime
import argparse

from glyph import Glyph
from font import Font

__author__ = 'andreas.dahlberg90@gmail.com (Andreas Dahlberg)'
__version__ = '0.1.0'


def generate_file(data, template):
    with open(template, 'r') as template_file:
        template_data = template_file.read()

    return template_data.format(**data)


def to_string(lines):
    string = ''

    for line in lines:
        string = string + line

    return string


def byte_array_to_line(byte_array):
    line = ''
    for byte in byte_array:
        line = line + '0x{:02X}, '.format(byte)

    return line


def main(args):
    f = Font(args.font, args.size, args.monospace)
    f.add_chars(string.ascii_letters)
    f.add_chars(string.digits)
    f.add_chars(' -:%.')

    offset = 0
    index = 0

    now = datetime.datetime.now()

    data = {}
    data['author'] = 'Andreas Dahlberg'
    data['date'] = now.strftime("%Y-%m-%d %H:%M")
    data['brief'] = 'Font data for Ubuntu Mono.'
    data['font_name'] = 'Ubuntu Mono {}.'.format(args.size)
    data['font_advance'] = f.advance
    data['monospace_font'] = int(args.monospace)

    glyph_data_lines = []
    glyph_info_lines = []
    glyph_offset_lines = []

    for g in f:
        l = '    ' + byte_array_to_line(g.to_byte_array()) + '// \'{}\'\n'.format(g.char)
        glyph_data_lines.append(l)

        glyph_info_lines.append('{\n');
        glyph_info_lines.append('    .data = &glyph_data[{}],\n'.format(offset))
        glyph_info_lines.append('    .width = {},\n'.format(g.width))
        glyph_info_lines.append('    .height = {},\n'.format(g.height))
        glyph_info_lines.append('    .baseline_offset = {},\n'.format(g.offset))
        glyph_info_lines.append('    .horizontal_bearing = {},\n'.format(g.horizontal_bearing))

        if not f.monospace:
            glyph_info_lines.append('    .advance = {},\n'.format(g.advance))
        glyph_info_lines.append('},\n');
        glyph_offset_lines.append('        case \'{}\':\n'.format(g.char))
        glyph_offset_lines.append('            glyph_offset = {};\n'.format(index))
        glyph_offset_lines.append('            break;\n')
        req_bytes = math.ceil(g.height * g.width / 8)

        offset = offset + req_bytes
        index = index + 1

    data['glyph_data'] = to_string(glyph_data_lines)
    data['glyph_info'] = to_string(glyph_info_lines)
    data['glyph_offset'] = to_string(glyph_offset_lines)

    content = generate_file(data, args.template)
    with open(args.output, 'w') as f:
        f.write(content)

    return 0


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('font', help='Full path to a font.')
    parser.add_argument('size', type=int, help='Size of the generated font.')

    parser.add_argument('template', help='Output directory for the generated files.')
    parser.add_argument('output', help='Output directory for the generated files.')

    parser.add_argument('-m', '--monospace', help='Select if the font is monospace or not.',
        action='store_true')

    args = parser.parse_args()
    exit(main(args))
