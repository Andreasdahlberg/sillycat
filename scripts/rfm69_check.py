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

__author__ = 'andreas.dahlberg90@gmail.com (Andreas Dahlberg)'
__version__ = '0.1.0'

FXOSC = 32000000.0
FSTEP = FXOSC / (2^19)

def check_oversampling_rule(bitrate, channel_filter_bandwith):
    return bitrate < channel_filter_bandwith / 2

def check_receiver_bandwidth_rule(channel_filter_bandwith, frequency_deviation):
    return channel_filter_bandwith > 2 * frequency_deviation

def check_modulation_index_rule(bitrate, frequency_deviation):
    return (2 * frequency_deviation / bitrate) >= 0.5 and (2 * frequency_deviation / bitrate) <= 10

def check_modulation_rule(bitrate, frequency_deviation):
    return frequency_deviation + bitrate / 2 <= 500000

def check_frequency_deviation_rule(frequency_deviation):
    return frequency_deviation > 600

def check_all_rules(bitrate, channel_filter_bandwith, frequency_deviation, verbose):

    failed_checks = 0

    if not check_oversampling_rule(bitrate, channel_filter_bandwith):
        failed_checks += 1
        if verbose:
            print('* Oversampling rule violated')
            print('  The bit-rate cannot be set higher than two times the channel filter '
                  'bandwidth.')
            print()

    if not check_receiver_bandwidth_rule(channel_filter_bandwith, frequency_deviation):
        failed_checks += 1
        if verbose:
            print('* Receiver bandwidth rule violated')
            print('  The channel filter bandwidth must be set higher than two times the frequency '
                  'deviation.')
            print()

    if not check_modulation_index_rule(bitrate, frequency_deviation):
        failed_checks += 1
        if verbose:
            print('* Modulation index rule violated')
            print('  The modulation index must be between 0.5 and 10.')
            print()

    if not check_modulation_rule(bitrate, frequency_deviation):
        failed_checks += 1
        if verbose:
            print('* Modulation rule violated')
            print('  fdev + bitrate / 2 <= 500000')
            print()

    if not check_frequency_deviation_rule(frequency_deviation):
        failed_checks += 1
        if verbose:
            print('* Frequency deviation rule violated')
            print('  Frequency deviation must be set higher than 600 Hz.')
            print()

    if verbose:
        if failed_checks == 0:
            print('All checks passed')
        else:
            print('Number of failed checks: {}'.format(failed_checks))

    return failed_checks

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('bitrate', help="Transceiver bit-rate in b/s", type=int)
    parser.add_argument('rxbw', help="Channel filter bandwidth in Hz", type=int)
    parser.add_argument('fdev', help="Frequency deviation in Hz", type=int)
    parser.add_argument('-v', '--verbose', action='store_true')
    args = parser.parse_args()

    return check_all_rules(args.bitrate, args.rxbw, args.fdev, args.verbose)

if __name__ == '__main__':
    exit(main())
