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

import platform

def platform_is_windows():
    return platform.system().lower() == 'windows'