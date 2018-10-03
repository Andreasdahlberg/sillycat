# Memory helper scripts

## Reader
Reader is a script for reading and printing device error logs. This can be done
directly from the device by dumping the EEPROM or by reading the EEPROM content
from a HEX-file.

### Usage

Read error log from device:
```bash
./reader.py port <PORT>
```

Read error log from file:
```bash
./reader.py file <FILENAME>
```

### Hardware Requirements
When dumping the EEPROM from device an AVR programmer such as
[Olimex AVR-ISP-MK2](https://www.olimex.com/Products/AVR/Programmers/AVR-ISP-MK2/open-source-hardware)
is needed.
