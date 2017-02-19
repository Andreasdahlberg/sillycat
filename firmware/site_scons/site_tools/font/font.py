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

import freetype
from glyph import Glyph

__author__ = 'andreas.dahlberg90@gmail.com (Andreas Dahlberg)'
__version__ = '0.1.0'


class Font(object):
    """An object representing a font containing glyphs for a selected set of chars."""
    def __init__(self, file, size, monospace=False):
        self._face = freetype.Face(file)
        self._face.set_pixel_sizes(0, size)
        self._monospace = monospace
        self._glyphs = []
        self._iter_index = 0

    def __iter__(self):
        self._iter_index = 0
        return self

    def __next__(self):
        if self._iter_index == len(self._glyphs):
            raise StopIteration
        else:
            self._iter_index = self._iter_index + 1
            return self._glyphs[self._iter_index - 1]

    @property
    def monospace(self):
        """Property indicating if the font is monospace or not."""
        return self._monospace

    @property
    def advance(self):
        """Property for the advance value of a monospace font. None if returned if the font
        is non-monospace."""
        if self.monospace and len(self._glyphs) > 0:
            return int(self._glyphs[0].advance)
        else:
            return None

    def add_chars(self, chars):
        """Create glyph objects for the added chars and add them to the font."""
        for char in chars:
            self._face.load_char(char, freetype.FT_LOAD_RENDER | freetype.FT_LOAD_TARGET_MONO)
            glyph = Glyph.from_glyph_slot(char, self._face.glyph)
            self._glyphs.append(glyph)

    def get_all_glyph_data(self):
        """Get a list with the bit data for all glyphs in the font."""
        glyph_data = []

        for glyph in self._glyphs:
            glyph_data.extend(glyph.to_byte_array())

        return glyph_data
