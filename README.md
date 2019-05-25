SillyCat
========

[![Build Status](https://travis-ci.com/Andreasdahlberg/sillycat.svg?branch=master)](https://travis-ci.com/Andreasdahlberg/sillycat)
[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=Andreasdahlberg_sillycat&metric=alert_status)](https://sonarcloud.io/dashboard?id=Andreasdahlberg_sillycat)

## Development

### Getting Started
The following steps are required

1. Install required packages
    ```bash
    $ sudo apt install scons gcc-avr avr-libc avrdude
    ```

1. Install optional packages
    ```bash
    $ sudo apt install astyle valgrind
    ```

1. Install [cmocka](https://cmocka.org/)
    ```bash
    $ sudo apt install cmake
    $ git clone git://git.cryptomilk.org/projects/cmocka.git
    $ mkdir cmocka/build && cd cmocka/build
    $ cmake -DCMAKE_INSTALL_PREFIX=/usr ..
    $ make
    $ sudo make install
    ```

1. Install [freetype-py](https://pypi.org/project/freetype-py/)
    ```bash
    $ sudo apt install python3-setuptools
    $ git clone https://github.com/rougier/freetype-py.git freetype-py
    $ cd freetype-py
    $ python3 setup.py install
    ```

### Building
```bash
$ cd firmware
$ scons build-main-firmware build-node-firmware
```

### Testing
See [Testing](firmware/tests/README.md)

### Configuration
See [Configuration](scripts/memory/README.md#config)

### CI
See [Docker](docker/README.md)

## Support
If you're having any problems, please raise an issue on GitHub and i will be happy to help.
