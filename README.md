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
    $ sudo python3 setup.py install
    ```

### Building
```bash
$ cd firmware
$ scons build-main-release build-node-release
```

### Board bringup
1. Set fuses
    ```bash
    $ scons init-fuses
    ```

1. Get configuration
    ```bash
    $ scripts/memory/config.py load /dev/ttyACM0 conf.eep
    ```

1. Set configuration values
    ```bash
    $ scripts/memory/config.py set conf.eep version 1
    $ scripts/memory/config.py set conf.eep network_id AABBCCDDEEFF
    $ scripts/memory/config.py set conf.eep aes_key 1DUMMYKEYFOOBAR1
    $ scripts/memory/config.py set conf.eep master_address 170
    $ scripts/memory/config.py set conf.eep address 170
    $ scripts/memory/config.py set conf.eep broadcast_address 255
    $ scripts/memory/config.py set conf.eep report_interval 60
    ```
    
    See [Configuration](scripts/memory/README.md#config) for details.

1. Save configuration to device
    ```bash
    $ /config.py save /dev/ttyACM0 conf.eep
    ```

1. Flash firmware
    ```bash
    scons flash-main-release
    ```   

### Testing
See [Testing](firmware/tests/README.md)

### CI
See [Docker](docker/README.md)

## Support
If you're having any problems, please raise an issue on GitHub and i will be happy to help.
