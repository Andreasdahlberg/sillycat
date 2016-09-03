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
import time
from PyQt4 import QtGui, QtCore


class PacketView(QtGui.QTableWidget):
    def __init__(self, path):
        super(PacketView, self).__init__()

        self.setColumnCount(8)
        self.setRowCount(0);

        self.setHorizontalHeaderLabels(['Target', 'Source', 'Ack', 'RSSI', 'Packet size', 'Timestamp', 'Type', 'Data size'])
        self.horizontalHeader().setResizeMode(QtGui.QHeaderView.Stretch)

        self._path = path


    def update_packet_view(self, text):

        row_index = self.rowCount()
        self.insertRow(row_index)

        col_index = 0
        for value in text.split(','):
            self.setItem(row_index, col_index, QtGui.QTableWidgetItem(value))
            col_index = col_index + 1
        self.export_to_csv('test')            


    def export_to_csv(self, name, separator=',', end_of_line='\n'):
        if not os.path.isdir(self._path):
            os.makedirs(self._path)

        timestamp = time.strftime("%Y%m%d%H%M%S")
        filename = '{}_{}.csv'.format(name, timestamp)
        
        output_file = open(os.path.join(self._path, filename), 'w')

        for row_index in range(0, self.rowCount()):
            for column_index in range(0, self.columnCount()):
                column_data = self.item(row_index, column_index).text()
                output_file.write(column_data + separator)

            output_file.write(end_of_line)
        output_file.close()