FROM ubuntu:16.04

RUN apt-get -y update && apt-get install -y \
    build-essential \
    gcc-avr \
    binutils-avr \
    avr-libc \
    scons \
    python3 \
    python3-setuptools \
    git \
    cmake \
    libfreetype6 \
    curl

# Install cmocka
RUN git clone git://git.cryptomilk.org/projects/cmocka.git && \
    mkdir cmocka/build && \
    cd cmocka/build && \
    cmake -DCMAKE_INSTALL_PREFIX=/usr .. && \
    make && \
    make install

# Install freetype-py
RUN git clone https://github.com/rougier/freetype-py.git freetype-py && \
    cd freetype-py && \
    python3 setup.py install
