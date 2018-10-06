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

import argparse
from nvm import MemoryTool
from nvm import Config

__author__ = 'andreas.dahlberg90@gmail.com (Andreas Dahlberg)'
__version__ = '0.1.0'

def handle_get(args):
    """Get configuration values."""
    config = Config.from_eep_file(args.file)
    for field in args.field:
        value = getattr(config, field)
        print('{}={}'.format(field, value))


def handle_set(args):
    """Set a configuration value."""
    config = Config.from_eep_file(args.file)
    setattr(config, args.field, args.value)
    config.to_eep_file(args.file)


def handle_load(args):
    """Load the configuration from device to file."""
    nvm_tool = MemoryTool(args.port)
    nvm_tool.read(args.file)


def handle_save(args):
    """Save the configuration from file to device."""
    nvm_tool = MemoryTool(args.port)
    nvm_tool.write(args.file)


def main():
    """Main function handling command line arguments."""
    parser = argparse.ArgumentParser()
    subparsers = parser.add_subparsers()

    subparser_get = subparsers.add_parser('get')
    subparser_get.add_argument('file')
    subparser_get.add_argument('field', nargs='*')
    subparser_get.set_defaults(func=handle_get)

    subparser_set = subparsers.add_parser('set')
    subparser_set.add_argument('file')
    subparser_set.add_argument('field')
    subparser_set.add_argument('value')
    subparser_set.set_defaults(func=handle_set)

    subparser_load = subparsers.add_parser('load')
    subparser_load.add_argument('port')
    subparser_load.add_argument('file')
    subparser_load.set_defaults(func=handle_load)

    subparser_save = subparsers.add_parser('save')
    subparser_save.add_argument('port')
    subparser_save.add_argument('file')
    subparser_save.set_defaults(func=handle_save)

    args = parser.parse_args()
    args.func(args)

    return 0


if __name__ == '__main__':
    exit(main())
