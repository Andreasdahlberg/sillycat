#!/bin/sh

wget https://cmocka.org/files/1.1/cmocka-1.1.0.tar.xz --no-check-certificate
tar xf cmocka-1.1.0.tar.xz

mkdir cmocka-1.1.0/build
cd cmocka-1.1.0/build

cmake ..
make
make install
