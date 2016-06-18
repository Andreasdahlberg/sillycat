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
import sys
import glob
import re
import datetime
from string import Template

REGEXP_TEST_FUNC = '.*\s*void (test_.*)\(void \*\*state'
OUTPUT_FILE_FORMAT = 'run_test_{}.c'

def format_unit_tests(test_names):
    formated_test_names = ''

    #TODO: Handle tests with setup
    for test_name in test_names:
        formated_test_names = formated_test_names + '       cmocka_unit_test({}),\n'.format(test_name)

    return formated_test_names[:-2]


def format_test_headers(test_headers):
    """Format 'test_headers' into C-code(include)"""    
    formated_test_headers = ''

    for test_header in test_headers:
        test_header = test_header.split('/')[-1]
        formated_test_headers = formated_test_headers + ('#include "{}"\n'.format(test_header))

    return formated_test_headers[:-1]


def scan_headers(headers):
    """Scan 'headers' for test function prototypes and extract the function names"""
    unit_tests = []
    for header in headers:
        try:
            header_file = open(header, 'r')
            header_content = header_file.read()
        
            unit_tests = unit_tests + re.findall(REGEXP_TEST_FUNC, header_content)
        except:
            pass
        finally:
            header_file.close()

    return unit_tests


def generate_cmocka_test(path, template_file):
    test_suite_name = os.path.basename(os.path.normpath(path))

    test_headers = glob.glob(os.path.join(path, 'test_*.h'))
    unit_tests = scan_headers(test_headers)

    if len(test_headers) == 0 or len(unit_tests) == 0:
        return False;

    template_file = open(template_file, 'r')
    template = Template(template_file.read())
    template_file.close()

    current_time = datetime.datetime.now()
    formated_date = '{}-{:0>2}-{:0>2}'.format(current_time.year, current_time.month, current_time.day)
    
    out_file_name = os.path.join(path, OUTPUT_FILE_FORMAT.format(test_suite_name))

    try:
        out_file = open(out_file_name, 'w')
        out_file.write(template.substitute(
            test_includes=format_test_headers(test_headers),
            unit_tests=format_unit_tests(unit_tests),
            file=out_file_name,
            date=formated_date))
    except:
        pass
    finally:
        out_file.close()

    return True;


if __name__ == '__main__':
    if len(sys.argv) < 3:
        print('Error: Arguments missing')
        exit(1)

    for path in sys.argv[2:]:
        print(path)
        generate_cmocka_test(path, sys.argv[1])   