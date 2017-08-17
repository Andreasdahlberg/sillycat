#!/usr/bin/env python3
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
# along with SillyCat Development Tools.  If not, see
# <http://www.gnu.org/licenses/>.

import os
import time
import datetime
import logging

__author__ = 'andreas.dahlberg90@gmail.com (Andreas Dahlberg)'
__version__ = '0.1.0'


class DataLog():

    def __init__(self, path):
        self.trigger_map = {'<WARNING>': logging.warning, '<ERROR>': logging.error,
                            '<INFO>': logging.info, '<CRITICAL>': logging.critical}

        if not os.path.isdir(path):
            os.makedirs(path)

        log_file = os.path.join(path, self.get_filename())
        logging.basicConfig(filename=log_file, level=logging.INFO,
                            format='%(asctime)s; <%(levelname)s> %(message)s')
        logging.info('Log started')

    def handle_signal(self, text):
        for key in self.trigger_map:
            if text.count(key) > 0:
                log_text = text.split(key)[1].strip()
                self.trigger_map[key](log_text)

                return
        logging.debug(text.strip())
        return

    def get_filename(self, prefix='devlog'):
        current_time = time.time()
        return datetime.datetime.fromtimestamp(current_time).strftime(
            prefix + '_%Y-%m-%d_%H-%M.log')
