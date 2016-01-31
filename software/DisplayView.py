#!/usr/bin/python
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
__version__ = '0.1.0'

import os
import math, struct

from PyQt4 import QtGui, QtCore

VRAM_PAGES = 4
VRAM_COLUMNS = 128
VRAM_SIZE = (VRAM_PAGES * VRAM_COLUMNS)

def create_bitmap_image(rows, w):
    mult4 = lambda n: int(math.ceil(n/4))*4
    mult8 = lambda n: int(math.ceil(n/8))*8
    lh = lambda n: struct.pack("<h", n)
    li = lambda n: struct.pack("<i", n)

    h, wB = len(rows), int(mult8(w)/8)
    s, pad = li(mult4(wB)*h+0x20), [0]*(mult4(wB)-wB)
    s = li(mult4(w)*h+0x20)
    return (b"BM" + s + b"\x00\x00\x00\x00\x20\x00\x00\x00\x0C\x00\x00\x00" +
        lh(w) + lh(h) + b"\x01\x00\x01\x00\xff\xff\xff\x00\x00\x00" +
        b"".join([bytes(row+pad) for row in reversed(rows)]))



class DisplayPixmap(QtGui.QLabel):
    def __init__(self):
        super(DisplayPixmap, self).__init__()

class DisplayView(QtGui.QLabel):
    def __init__(self):
        super(DisplayView, self).__init__()
        self._pixmap = QtGui.QPixmap()
        self.setPixmap(self._pixmap)

        self.setMouseTracking(True)


    def mouseMoveEvent(self, event):
        mouse_position = event.pos(); 

        x_pos = math.floor(mouse_position.x() / 3);
        if x_pos > 127:
            x_pos = 127

        y_pos = math.floor(mouse_position.y() / 3);
        if y_pos > 30:
            y_pos = 30            



        scaled_point = QtCore.QPoint(x_pos, y_pos)


        self.emit(QtCore.SIGNAL('new_pixel(QPoint)'), scaled_point)


    def add_color(self, pixmap):
        mask = pixmap.createMaskFromColor(QtGui.QColor(0, 0, 0), QtCore.Qt.MaskOutColor)
        
        p = QtGui.QPainter(pixmap)
        p.setPen(QtGui.QColor(255, 201, 14))

        p.drawPixmap(pixmap.rect(), mask, mask.rect())
        p.end() 

        mask = pixmap.createMaskFromColor(QtGui.QColor(255, 255, 255), QtCore.Qt.MaskOutColor)

        p = QtGui.QPainter(pixmap)
        p.setPen(QtGui.QColor(0, 0, 0))
        p.drawPixmap(pixmap.rect(), mask, mask.rect())
        p.end()

        return pixmap   


    def save_view_to_file(self, filename="display.png"):
        self._pixmap.save(filename, 'PNG')


    def update_display_view(self, image_data):
        self._pixmap.loadFromData(self.format_image_data(bytes(image_data)))
        self.setPixmap(self.add_color(self._pixmap).scaled(384, 93))

        if (self.isHidden()):
            self.show()


    def format_image_data(self, image_data):
        formated_data = []

        data = 0
        bit_cnt = 0;
        for y in range(0, 32):
            page_idx = math.floor(y / 8)
            page_offset = y % 8
            byte_offset = VRAM_COLUMNS * page_idx
            for x in range(0, 128):
                if (bit_cnt == 8):
                    formated_data.append(data)
                    data = 0
                    bit_cnt = 0

                bit = (image_data[byte_offset + x] >> page_offset) & 0x01
                data = (data | (bit << (7 - bit_cnt)))
                bit_cnt = bit_cnt + 1

        cnt = 0

        img_data = []
        row_data = []
        for byte in formated_data:

            if(cnt == 16):
                img_data.append(row_data)
                row_data = []
                cnt = 0

            row_data.append(byte)
            cnt = cnt + 1
        return bytes(create_bitmap_image(img_data, 128))

