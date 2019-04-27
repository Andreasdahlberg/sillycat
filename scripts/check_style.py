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

import sys
import subprocess


def execute_format_command(cmd='scons -C firmware format'):
    """Execute the format command and return the result."""

    output = subprocess.check_output(cmd, shell=True)
    return output.decode('utf-8')


def check_command_output(output):
    """Check if the output contains 'Formatted'."""
    if 'Formatted' in output:
        print(output)
        return 1
    return 0


def main():
    output = execute_format_command()
    return check_command_output(output)


if __name__ == '__main__':
    sys.exit(main())
