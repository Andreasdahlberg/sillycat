FROM ubuntu:20.04

RUN DEBIAN_FRONTEND="noninteractive" apt-get update && apt-get -y install tzdata

RUN apt-get update && apt-get install -y \
    build-essential \
    gcc-avr \
    binutils-avr \
    avr-libc \
    python3 \
    python3-setuptools \
    python3-pip \
    git \
    cmake \
    libfreetype6 \
    curl \
    valgrind \
    astyle \
    wget \
    unzip \
    gcovr \
    openjdk-11-jdk

#Install SCons
RUN python3 -m pip install scons

# Install cmocka
RUN git clone https://git.cryptomilk.org/projects/cmocka.git && \
    mkdir cmocka/build && \
    cd cmocka/build && \
    cmake -DCMAKE_INSTALL_PREFIX=/usr .. && \
    make && \
    make install

# Install freetype-py
RUN git clone https://github.com/rougier/freetype-py.git freetype-py && \
    cd freetype-py && \
    python3 setup.py install

# Download SonarCloud build wrapper and add to path
RUN wget http://sonarcloud.io/static/cpp/build-wrapper-linux-x86.zip && \
    unzip -j build-wrapper-linux-x86.zip -d /usr/local/bin && \
    cp /usr/local/bin/libinterceptor-x86_64.so /usr/local/bin/libinterceptor-haswell.so && \
    rm build-wrapper-linux-x86.zip

# Download SonarScanner and install
RUN wget https://binaries.sonarsource.com/Distribution/sonar-scanner-cli/sonar-scanner-cli-4.4.0.2170-linux.zip && \
    unzip sonar-scanner-cli-4.4.0.2170-linux.zip && \
    rm sonar-scanner-cli-4.4.0.2170-linux.zip && \
    mv sonar-scanner-4.4.0.2170-linux /usr/lib/sonar-scanner && \
    ln -s /usr/lib/sonar-scanner/bin/sonar-scanner /usr/local/bin/sonar-scanner

ENV SONAR_RUNNER_HOME=/usr/lib/sonar-scanner

COPY sonar-scanner.properties /usr/lib/sonar-scanner/conf/sonar-scanner.properties


