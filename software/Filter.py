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


def begins_with(stream_data, keyword):
	return stream_data.startswith(keyword)

def ends_with(stream_data, keyword):
	return stream_data.endswith(keyword)	

def contains(stream_data, keyword):
	return stream_data.count(keyword) > 0



class Filter():
	def __init__(self):
		self._active_ = []


	def filter(stream_data):
		pass


	def add_start_keyword(keyword):
		pass






if __name__ == '__main__':
	print(contains('spam end eggs', 'end'))