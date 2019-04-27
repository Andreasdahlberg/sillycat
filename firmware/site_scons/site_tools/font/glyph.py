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

__author__ = 'andreas.dahlberg90@gmail.com (Andreas Dahlberg)'
__version__ = '0.1.0'


def _bits(byte):
    data = []
    for _ in range(8):
        data.insert(0, int((byte & 1) == 1))
        byte = byte >> 1
    return data


class Glyph(object):
    """An object representing a glyph."""
    def __init__(self, char, data, width, height, advance, offset, horizontal_bearing):
        self._char = char
        self._data = data
        self._width = width
        self._height = height
        self._advance = advance
        self._offset = offset
        self._horizontal_bearing = horizontal_bearing

    def __str__(self):
        out = '"{}", Width: {}, Height: {}, Offset: {}, Advance: {}, Horizontal Bearing: {}\n'.format(
            self._char, self._width, self._height, self._offset, self._advance, self._horizontal_bearing)
        return out + self._get_ascii_art()

    def _get_ascii_art(self):
        out = ''
        data = self.to_byte_array()

        for y in range(0, self.height):
            for x in range(0, self.width):
                bit_count = int((y * self.width) + x)
                byte_idx = int(bit_count / 8)
                bit_idx = int(bit_count % 8)

                if data[byte_idx] & (1 << (7 - bit_idx)):
                    out = out + '*'
                else:
                    out = out + ' '
            out = out + '\n'
        return out

    @property
    def char(self):
        """Get the char value for the glyph."""
        return self._char

    @property
    def width(self):
        """Get the width of the glyph."""
        return self._width

    @property
    def height(self):
        """Get the height of the glyph."""
        return self._height

    @property
    def data(self):
        return self._data

    @property
    def advance(self):
        """Get the advance value for the glyph."""
        return self._advance

    @property
    def offset(self):
        """Get the glyph offset from the baseline."""
        return self._offset

    @property
    def horizontal_bearing(self):
        """Get the horizontal distance from the current pen position to the glyph's left bbox edge"""
        return self._horizontal_bearing

    @staticmethod
    def unpack_bitmap(bitmap):
        """Unpacke the glyhp bitmap into an array with all the pixel values."""
        data = []
        for y_pixel in range(bitmap.rows):
            row = []
            for x_pixel in range(bitmap.pitch):
                row.extend(_bits(bitmap.buffer[y_pixel*bitmap.pitch+x_pixel]))
            data.extend(row[:bitmap.width])
        return data

    @staticmethod
    def from_glyph_slot(char, glyph_slot):
        """Create a Glyph object from an Freetype Glyph slot."""

        data = Glyph.unpack_bitmap(glyph_slot.bitmap)
        width = glyph_slot.bitmap.width
        height = glyph_slot.bitmap.rows
        advance = glyph_slot.advance.x / 64
        offset = glyph_slot.bitmap_top - height
        horizontal_bearing = glyph_slot.bitmap_left

        return Glyph(char, data, width, height, advance, offset, horizontal_bearing)

    def to_byte_array(self):
        """Convert the glyph bitmap into an packed byte array."""

        byte_counter = 0
        bit_index = 0
        data = 0

        array = []
        for y_pixel in range(self.height):
            for x_pixel in range(self.width):
                val = self.data[y_pixel * self.width + x_pixel]

                data |= (val << 7 - bit_index)
                bit_index += 1

                if bit_index == 8:
                    byte_counter += 1
                    bit_index = 0
                    array.append(data)
                    data = 0

        req_bytes = math.ceil(self.height * self.width / 8)

        #Skip empty glyphs, ex. space. Also check if an byte with the left bits should be added.
        if self.height > 0 and self.width > 0 and byte_counter < req_bytes:
            byte_counter += 1
            bit_index = 0
            array.append(data)

        return array
