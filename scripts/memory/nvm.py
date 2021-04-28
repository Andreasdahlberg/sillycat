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
import subprocess
import struct
from datetime import datetime
from datetime import timezone
from datetime import timedelta
from intelhex import IntelHex
import crcmod

__author__ = 'andreas.dahlberg90@gmail.com (Andreas Dahlberg)'
__version__ = '0.1.0'


class ConfigCrcException(Exception):
    """Exception for CRC mismatches."""
    pass


class Config(object):
    """An object representing the device configuration."""

    _FMT = 'H6sI17sBBB'
    _FMT_CRC = _FMT + 'H'
    START_ADDRESS = 0x00

    def __init__(
            self,
            version,
            network_id,
            report_interval,
            aes_key,
            master_address,
            address,
            broadcast_address
        ):
        self._version = version
        self._network_id = network_id
        self._report_interval = report_interval
        self._aes_key = aes_key
        self._master_address = master_address
        self._address = address
        self._broadcast_address = broadcast_address

    @staticmethod
    def calculate_crc(data):
        """Calculate CRC-16 for the supplied data."""
        crc_16_func = crcmod.mkCrcFun(0x18005, rev=False, initCrc=0x0000)
        return crc_16_func(data)

    @classmethod
    def from_eep_file(cls, eep_file_name):
        """Create an Config instance with data from an EEPROM-file."""
        hex_file = IntelHex()
        hex_file.fromfile(eep_file_name, format='hex')

        data = hex_file.gets(Config.START_ADDRESS, Config.size())
        parameters = struct.unpack(Config._FMT_CRC, data)

        stored_crc = parameters[-1]
        # Remove the two last bytes to exclude the stored CRC value from the
        # CRC calculation.
        calculated_crc = Config.calculate_crc(data[:-2])

        if stored_crc != calculated_crc:
            raise ConfigCrcException('CRC mismatch')

        # Remove last element with the stored CRC since it's only used for
        # validation.
        return cls(*parameters[:-1])

    def to_eep_file(self, eep_file_name):
        """Save the configuration to an EEPROM-file."""
        hex_file = IntelHex()

        # Change any existing files instead of overwriting them.
        if os.path.isfile(eep_file_name):
            hex_file.fromfile(eep_file_name, format='hex')

        data = self.to_bytes(crc=True)
        hex_file.puts(Config.START_ADDRESS, data)
        hex_file.write_hex_file(eep_file_name, byte_count=0x20)

    def to_bytes(self, crc=True):
        """Convert the configuration to an byte array."""
        if crc:
            return struct.pack(
                Config._FMT_CRC,
                self._version,
                self._network_id,
                self._report_interval,
                self._aes_key,
                self._master_address,
                self._address,
                self._broadcast_address,
                self.crc
            )
        else:
            return struct.pack(
                Config._FMT,
                self._version,
                self._network_id,
                self._report_interval,
                self._aes_key,
                self._master_address,
                self._address,
                self._broadcast_address
            )

    @property
    def version(self):
        """Get configuration version."""
        return self._version

    @version.setter
    def version(self, version):
        """Set configuration version."""
        version = int(version)
        if version == 0 or version > 0xffff:
            raise Exception('Invalid version')
        self._version = version

    @property
    def network_id(self):
        """Get network ID."""
        return self._network_id

    @network_id.setter
    def network_id(self, network_id):
        """Set network ID."""
        network_id = bytes.fromhex(network_id)
        if len(network_id) != 6:
            raise Exception('Invalid network ID length')
        self._network_id = network_id

    @property
    def aes_key(self):
        """Get AES-key."""
        return self._aes_key.decode()

    @aes_key.setter
    def aes_key(self, aes_key):
        """Set AES-key."""
        if len(aes_key) != 16:
            raise Exception('Invalid AES-key length')
        self._aes_key = aes_key.encode()

    @property
    def report_interval(self):
        """Get report interval in seconds."""
        return self._report_interval

    @report_interval.setter
    def report_interval(self, report_interval):
        """Set report interval in seconds."""
        report_interval = int(report_interval)
        if report_interval < 10 or report_interval > 0xffffffff:
            raise Exception('Invalid report interval')
        self._report_interval = report_interval

    @property
    def master_address(self):
        """Get master address."""
        return self._master_address

    @master_address.setter
    def master_address(self, master_address):
        """Set master address."""
        master_address = int(master_address)
        if master_address == 0 and master_address > 255:
            raise Exception('Invalid master address')
        self._master_address = master_address

    @property
    def address(self):
        """Get address."""
        return self._address

    @address.setter
    def address(self, address):
        """Set address."""
        address = int(address)
        if address == 0 and address > 255:
            raise Exception('Invalid address')
        self._address = address

    @property
    def broadcast_address(self):
        """Get broadcast address."""
        return self._broadcast_address

    @broadcast_address.setter
    def broadcast_address(self, broadcast_address):
        """Set broadcast address."""
        broadcast_address = int(broadcast_address)
        if broadcast_address == 0 and broadcast_address > 255:
            raise Exception('Invalid broadcast address')
        self._broadcast_address = broadcast_address

    @property
    def crc(self):
        """Get CRC-16 for the current configuration."""
        data = self.to_bytes(crc=False)
        return self.calculate_crc(data)

    @staticmethod
    def size():
        """Get size of the configuration in bytes."""
        config_struct = struct.Struct(Config._FMT_CRC)
        return config_struct.size


class ErrorMessage(object):
    """An object representing the device error message."""

    _FMT = 'IIBB'
    _DESCRIPTIONS = {
        1: 'POWERON',
        2: 'ASSFAIL',
        3: 'LOW_STACK',
        4: 'RTC_FAIL',
        5: 'CORRUPT_CONFIG',
    }

    def __init__(self, index, timestamp, code, information):
        self._index = index
        self._timestamp = timestamp
        self._code = code
        self._information = information

    def to_bytes(self):
        """Convert the error message to an byte array."""
        return struct.pack(
            ErrorMessage._FMT,
            self._index,
            self._timestamp,
            self._code,
            self._information
        )

    @classmethod
    def from_bytes(cls, data):
        """Create an ErrorMessage instance from an byte array."""
        index, timestamp, code, information = struct.unpack(ErrorMessage._FMT, data)

        # Index 0 is used to mark an unused entry in the error log.
        if index != 0:
            return cls(index, timestamp, code, information)
        else:
            return None

    @property
    def index(self):
        """Get the index of the the error message."""
        return self._index

    @property
    def description(self):
        """Get a short error description."""
        try:
            description = ErrorMessage._DESCRIPTIONS[self._code]
        except KeyError:
            description = 'UNKNOWN({})'.format(self._code)
        return description

    @property
    def time(self):
        """Get the time that the error message was saved."""
        date = datetime(2000, 1, 1, tzinfo=timezone.utc) + timedelta(seconds=self._timestamp)
        return date.strftime('%Y-%m-%d %H:%M:%S')

    @property
    def information(self):
        """Get the information saved with the error message."""
        return self._information

    @staticmethod
    def size():
        """Get the size of the error message in bytes."""
        error_message_struct = struct.Struct(ErrorMessage._FMT)
        return error_message_struct.size


class ErrorLog(object):
    """An object representing the device error log."""

    _START_ADDRESS = 0x22
    _END_ADDRESS = 0x340

    def __init__(self):
        self._error_messages = []
        self._iter_index = 0

    def __iter__(self):
        self._iter_index = 0
        return self

    def __next__(self):
        if self._iter_index == len(self._error_messages):
            raise StopIteration
        else:
            self._iter_index = self._iter_index + 1
            return self._error_messages[self._iter_index - 1]

    def __len__(self):
        return len(self._error_messages)

    def append(self, message):
        """Append an error message to the error log."""
        self._error_messages.append(message)

    @classmethod
    def from_eep_file(cls, eep_file_name):
        """Create an ErrorLog instance with the content from the EEPROM-file."""
        hex_file = IntelHex()
        hex_file.fromfile(eep_file_name, format='hex')

        log = cls()
        for addr in range(ErrorLog._START_ADDRESS, ErrorLog._END_ADDRESS, ErrorMessage.size()):
            data = hex_file.gets(addr, ErrorMessage.size())

            error_message = ErrorMessage.from_bytes(data)
            if error_message:
                log.append(error_message)

        return log


class MemoryTool(object):
    """Helper for using avrdude to read and write the device EEPROM."""
    _CMD_FMT = 'avrdude -p {} -P {} -c {} -b {} -U eeprom:{}:{}:i'

    def __init__(self, port, tool='avrisp2', baud_rate=19200, mcu='atmega328p'):
        self._port = port
        self._tool = tool
        self._baud_rate = baud_rate
        self._mcu = mcu

    def read(self, eep_file_name):
        """Read the device EEPROM to hex file."""
        cmd = MemoryTool._CMD_FMT.format(
            self._mcu,
            self._port,
            self._tool,
            self._baud_rate,
            'r',
            eep_file_name
        )
        subprocess.call(cmd, shell=True)

    def write(self, eep_file_name):
        """Write hex file to device EEPROM."""
        cmd = MemoryTool._CMD_FMT.format(
            self._mcu,
            self._port,
            self._tool,
            self._baud_rate,
            'w',
            eep_file_name
        )
        subprocess.call(cmd, shell=True)
