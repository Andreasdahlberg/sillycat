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
import sys
import subprocess

def execute_command(cmd):
    """Execute the supplied command and return the result."""
    print('Executing', cmd)
    output = subprocess.check_output(cmd, shell=True)
    return output.decode('utf-8')


def main():
    """Run the Sonar Scanner with different options depending on the build type."""

    try:
        if os.environ['TRAVIS_TAG']:
            print('Skip Sonar scan when build is triggered by a tag.')
            print('TRAVIS_TAG:', os.environ['TRAVIS_TAG'])
            return 0

        pull_request = os.environ['TRAVIS_PULL_REQUEST']
        pull_request_branch = os.environ['TRAVIS_PULL_REQUEST_BRANCH']
        pull_request_base = os.environ['TRAVIS_BRANCH']
    except KeyError as error:
        print('Missing environment variable:', error)
        print('This script must be called by Travis CI!')
        return 1

    if pull_request_branch:
        print('Detected pull request')
        cmd = 'docker exec build sonar-scanner ' \
            '-Dsonar.login=$SONAR_TOKEN ' \
            '-Dsonar.pullrequest.branch=$TRAVIS_PULL_REQUEST_BRANCH ' \
            '-Dsonar.pullrequest.key=$TRAVIS_PULL_REQUEST ' \
            '-Dsonar.pullrequest.base=$TRAVIS_BRANCH'
    else:
        print('Detected push')
        cmd = 'docker exec build sonar-scanner ' \
            '-Dsonar.login=$SONAR_TOKEN ' \
            '-Dsonar.branch.name=$TRAVIS_BRANCH'

    print('PULL_REQUEST:', pull_request)
    print('PULL_REQUEST_BRANCH:', pull_request_branch)
    print('BRANCH:', pull_request_base)

    execute_command(cmd)
    return 0


if __name__ == '__main__':
    sys.exit(main())
