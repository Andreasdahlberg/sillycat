#!/bin/sh

wget https://cmocka.org/files/1.1/cmocka-1.1.2.tar.xz --no-check-certificate
tar xf cmocka-1.1.2.tar.xz

mkdir cmocka-1.1.2/build
cd cmocka-1.1.2/build

cmake -DCMAKE_INSTALL_PREFIX=/usr ..
make
make install
