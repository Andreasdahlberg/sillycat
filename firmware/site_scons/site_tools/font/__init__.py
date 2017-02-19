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

import SCons.Builder
import SCons.Action

__author__ = 'andreas.dahlberg90@gmail.com (Andreas Dahlberg)'
__version__ = '0.1.0'


class FontGeneratorNotFound(SCons.Warnings.Warning):
    pass


def _detect(env):
    """Check if the font generator tool is available."""
    try:
        return env['FONT']
    except KeyError:
        pass

    generator = env.WhereIs('generator.py', 'site_scons/site_tools/font')
    if generator:
        return generator

    raise SCons.Errors.StopError(
        FontGeneratorNotFound,
        "Could not detect the font generator")
    return None


def build_function(target, source, env):
    """Call the font generator."""
    assert len(target) == 1
    assert len(source) == 1

    generator_options = [
        'site_scons/site_tools/font/generator.py',
        '/usr/share/fonts/truetype/ubuntu-font-family/UbuntuMono-R.ttf',
        '16',
        source[0].abspath,
        target[0].abspath,
        '-m'
    ]

    # Since SCons does not support python 3 subproces is used as a workaround.
    subprocess.check_call(generator_options)

    return None


def _get_font_builder():
    return SCons.Builder.Builder(
        action=SCons.Action.Action(build_function, '${FONT_COMSTR}')
    )


def generate(env):
    """Add builders and construction variables to the environment."""

    env['FONT'] = _detect(env)
    env.Append(BUILDERS={
        'Font': _get_font_builder()
    })


def exists(env):
    """Check if the font generator exists."""
    return _detect(env)


SCons.Warnings.enableWarningClass(FontGeneratorNotFound)
