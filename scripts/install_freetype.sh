#!/bin/sh

git clone https://github.com/rougier/freetype-py.git freetype-py
cd freetype-py
python3 setup.py install
