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
import argparse
import uuid
from nvm import MemoryTool
from nvm import ErrorLog

__author__ = 'andreas.dahlberg90@gmail.com (Andreas Dahlberg)'
__version__ = '0.1.0'


def print_error_log_from_file(file_name):
    """Read HEX-file and print the error log."""
    error_log = ErrorLog.from_eep_file(file_name)

    print("Total errors: {}".format(len(error_log)))
    for error in error_log:
        print('[{}] {} {}:{}'.format(error.index, error.time, error.description, error.information))


def handle_port(args):
    """Read HEX-file from device EEPROM and print the error log."""
    eep_file = str(uuid.uuid4()) + '.eep'
    nvm_tool = MemoryTool(args.name)
    nvm_tool.read(eep_file)

    print_error_log_from_file(eep_file)

    if args.clean:
        os.remove(eep_file)
    else:
        print('EEPROM stored in ´{}´.'.format(eep_file))


def handle_file(args):
    """Read HEX-file and print the error log."""
    print_error_log_from_file(args.name)


def main():
    """Main function handling command line arguments."""
    parser = argparse.ArgumentParser()
    subparsers = parser.add_subparsers()

    parser_port = subparsers.add_parser('port')
    parser_port.add_argument('name')
    parser_port.add_argument('-c', '--clean', action='store_true')
    parser_port.set_defaults(func=handle_port)

    parser_file = subparsers.add_parser('file')
    parser_file.add_argument('name')
    parser_file.set_defaults(func=handle_file)

    args = parser.parse_args()
    args.func(args)

    return 0


if __name__ == '__main__':
    exit(main())
