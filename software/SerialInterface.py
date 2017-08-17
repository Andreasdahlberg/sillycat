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

import serial
import glob
from PyQt4 import QtCore
from DevToolUtil import platform_is_windows


class SerialInterface(QtCore.QThread):
    """Class to listen for serial data"""
    def __init__(self):
        """Init interface"""
        QtCore.QThread.__init__(self)
        self._running = False


    def open_port(self, port, baudrate, timeout=0.1):
        """Connect the interface to a serial port"""
        try:
            self._ser = serial.Serial(port, baudrate, timeout=timeout)
            return True
        except serial.SerialException:
            print('Failed to open interface on port ' + port)
            return False


    def get_ports(self):
        """Get a list with all available COM-ports"""

        if platform_is_windows():
            ports = range(256)
        else:
            ports = glob.glob ('/dev/tty[A-Za-z]*')

        open_ports = []
        for port_name in ports:
            try:
                s = serial.Serial(port_name)
                open_ports.append(s.portstr)
                s.close()
            except serial.SerialException:
                pass
        return open_ports


    def run(self):
        """Listens for data and emits a signal for every new line"""
        self._running = True
        while self._running == True:
            rx_data = self._ser.readline()
            if rx_data:
                if rx_data[:6] == b'<VRAM>':
                    rx_data += (self._ser.read(520 - len(rx_data)))
                    self.emit(QtCore.SIGNAL('new_vram(QByteArray)'),
                                            rx_data[6:-2])
                elif rx_data[:5] == b'<PCK>':
                        self.emit(QtCore.SIGNAL('new_pck(QString)'),
                                                rx_data[5:].decode('utf-8').rstrip('\r\n'))
                else:
                    try:
                        self.emit(QtCore.SIGNAL('new_stream(QString)'),
                                                rx_data.decode('utf-8').rstrip('\r\n'))
                    except UnicodeDecodeError:
                        pass
        self.exit_cleanup()


    def stop(self):
        """Stop listen for new data and kill thread"""
        self._running = False


    def exit_cleanup(self):
        """Clean up before killing thread, closing serial port"""
        self._ser.close()
