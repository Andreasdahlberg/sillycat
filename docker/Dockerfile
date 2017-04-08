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
    wget \
    cmake \
    libfreetype6 \
    curl

# Install cmocka
RUN wget https://cmocka.org/files/1.1/cmocka-1.1.0.tar.xz && \
    tar xf cmocka-1.1.0.tar.xz && \
    mkdir cmocka-1.1.0/build && \
    cd cmocka-1.1.0/build && \
    cmake -DCMAKE_INSTALL_PREFIX=/usr .. && \
    make && \
    make install

# Install freetype-py
RUN git clone https://github.com/rougier/freetype-py.git freetype-py && \
    cd freetype-py && \
    python3 setup.py install
