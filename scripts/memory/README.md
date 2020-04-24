# Memory helper scripts

## Reader
Reader is a script for reading and printing device error logs. This can be done
directly from the device by dumping the EEPROM or by reading the EEPROM content
from a HEX-file.

### Usage
Read error log from device:
```bash
$ ./reader.py port <PORT>
```

Read error log from file:
```bash
$ ./reader.py file <FILENAME>
```

## Config
Config is a script used for configuration of both node and main devices.

### Usage
Load the configuration from the device into a configuration file:
```bash
$ ./config.py load <PORT> <FILENAME>
```

Print configuration values from the supplied configuration file:
```bash
$ ./config.py get <FILENAME> version address
version=1
address=170
```

Set a configuration value in the configuration file:
```bash
$ ./config.py set <FILENAME> address 128
```

Save the configuration file to device:
```bash
$ ./config.py save <PORT> <FILENAME>
```

## Hardware Requirements
When dumping the EEPROM from device an AVR programmer such as
[Olimex AVR-ISP-MK2](https://www.olimex.com/Products/AVR/Programmers/AVR-ISP-MK2/open-source-hardware)
is needed.
