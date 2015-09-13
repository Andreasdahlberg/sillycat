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
import threading

class SerialInterface(threading.Thread):
    def __init__(self):
        super(SerialInterface, self).__init__()        
        self.filter = []
        self.data_sinks = []
        self._running = False


    def connect(self, port, baudrate, timeout=0.1):
        """Connect the interface to a serial port"""        
        try:
            self._ser = serial.Serial(port, baudrate, timeout=timeout)
            return True
        except serial.SerialException:
            print('Failed to open interface on port ' + port)
            return False


    def run(self):
        self._running = True
        while self._running == True:
            rx_data = self._ser.readline()
            if rx_data:
                for sink in self.data_sinks:
                    sink.push(rx_data)

        self.exit_cleanup()


    def stop(self):
        self._running = False


    def exit_cleanup(self):
        self._ser.close() 