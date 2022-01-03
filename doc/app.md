# Application CLI reference

## dbase - Database Store

~~~
dbase clear               Clear database store
dbase show                Show entire database
dbase load                Load database from flash
dbase flush               Save database to flash
dbase replace             Replace database: prompts for multi-line value
dbase get <key>           Get value assigned to key
dbase set <key> <value>
                          Assign value to a key
dbase set <key>           Assign value to a key: Prompts for multi-line value
~~~

## echo - Control command echoing

~~~
echo on                   Enable echoing
echo off                  Disable echoing
echo                      Show current echo mode
~~~

## extrtc - External Real Time Clock access

~~~
extrtc                    Show date/time
extrtc YYYY-MM-DD HH:MM:SS
                          Set date/time
~~~

## help - Help Command

~~~
help <name>               Print help for a command
help                      List available commands
help all                  Print help for all commands

## i2c - I2C bus access

i2c scan                  Scan I2C bus
i2c [w AA DD ...] [r AA NN]
                          Raw I2C transaction
~~~

## info - Display serial number and firmware information

~~~
info                      Display serial number and firmware information
~~~

## m24256 - Access m24256 I2C-EEPROM on X-NUCLEO-EEPRMA2

~~~
m24256   rd <addr>        Read word
m24256   wr <addr> <val>  Write word
m24256   hd <addr> <len>  Hex dump memory
m24256   hd <addr>        Hex dump 256 bytes
m24256   hd               Hex dump next 256 bytes
m24256   crc <addr> <len> Calculate CRC of memory
m24256   fill <addr> <len>
                          Fill memory with pattern
m24256   fill <addr> <len> <val>
                          Fill memory with constant
~~~

## m24c02 - Access m24c02 I2C-EEPROM on X-NUCLEO-EEPRMA2

~~~
m24c02   rd <addr>        Read word
m24c02   wr <addr> <val>  Write word
m24c02   hd <addr> <len>  Hex dump memory
m24c02   hd <addr>        Hex dump 256 bytes
m24c02   hd               Hex dump next 256 bytes
m24c02   crc <addr> <len> Calculate CRC of memory
m24c02   fill <addr> <len>
                          Fill memory with pattern
m24c02   fill <addr> <len> <val>
                          Fill memory with constant
~~~

## m24m01 - Access m24m01 I2C-EEPROM on X-NUCLEO-EEPRMA2

~~~
m24m01   rd <addr>        Read word
m24m01   wr <addr> <val>  Write word
m24m01   hd <addr> <len>  Hex dump memory
m24m01   hd <addr>        Hex dump 256 bytes
m24m01   hd               Hex dump next 256 bytes
m24m01   crc <addr> <len> Calculate CRC of memory
m24m01   fill <addr> <len>
                          Fill memory with pattern
m24m01   fill <addr> <len> <val>
                          Fill memory with constant
~~~

## m95040 - Access m95040 SPI-EEPROM on X-NUCLEO-EEPRMA2

~~~
m95040   rd <addr>        Read word
m95040   wr <addr> <val>  Write word
m95040   hd <addr> <len>  Hex dump memory
m95040   hd <addr>        Hex dump 256 bytes
m95040   hd               Hex dump next 256 bytes
m95040   crc <addr> <len> Calculate CRC of memory
m95040   erase <addr> <len>
                          Erase memory
m95040   fill <addr> <len>
                          Fill memory with pattern
m95040   fill <addr> <len> <val>
                          Fill memory with constant
~~~

## m95256 - Access m95256 SPI-EEPROM on X-NUCLEO-EEPRMA2

~~~
m95256   rd <addr>        Read word
m95256   wr <addr> <val>  Write word
m95256   hd <addr> <len>  Hex dump memory
m95256   hd <addr>        Hex dump 256 bytes
m95256   hd               Hex dump next 256 bytes
m95256   crc <addr> <len> Calculate CRC of memory
m95256   erase <addr> <len>
                          Erase memory
m95256   fill <addr> <len>
                          Fill memory with pattern
m95256   fill <addr> <len> <val>
                          Fill memory with constant
~~~

## m95m04 - Access m95m04 SPI-EEPROM on X-NUCLEO-EEPRMA2

~~~
m95m04   rd <addr>        Read word
m95m04   wr <addr> <val>  Write word
m95m04   hd <addr> <len>  Hex dump memory
m95m04   hd <addr>        Hex dump 256 bytes
m95m04   hd               Hex dump next 256 bytes
m95m04   crc <addr> <len> Calculate CRC of memory
m95m04   erase <addr> <len>
                          Erase memory
m95m04   fill <addr> <len>
                          Fill memory with pattern
m95m04   fill <addr> <len> <val>
                          Fill memory with constant
~~~

## mcuflash - Access MCU flash memory

~~~
mcuflash rd <addr>        Read word
mcuflash wr <addr> <val>  Write word
mcuflash hd <addr> <len>  Hex dump flash memory
mcuflash crc <addr> <len> Calculate CRC of flash memory
mcuflash erase <addr> <len>
                          Erase flash memory
mcuflash fill <addr> <len>
                          Fill flash memory with pattern
mcuflash fill <addr> <len> <val>
                          Fill flash memory with byte
~~~

## mcurtc - Access MCU Real Time CLock

~~~
mcurtc                    Show date/time
mcurtc YYYY-MM-DD HH:MM:SS
                          Set date/time
~~~

## mem - Access MCU RAM

~~~
mem rd <addr>             Read 32-bit word from address
mem wr <addr> <data>      Write 32-bit word to address
mem hd <addr> <len>       Hex dump memory
mem hd <addr>             Hex dump 256 bytes
mem hd                    Hex dump next 256 bytes
~~~

## power - Set / Control idle power mode

~~~
power                     Show power mode
power <n>                 Set power mode (0, 1 or 2)
~~~

## reboot - Reboot system

~~~
reboot                    Reboot system
~~~

## test - Test commands

~~~
test stop                 Emit ^S
test wdt                  Infinite loop to test WDT
test sched                Test scheduler event
test sched nn             Test scheduler in nn ms
~~~

## work - Show work queue

~~~
work                      Show work queue
~~~

## ymodem - Transfer files over console UART

~~~
ymodem                    Receive a file
ymodem send               Send a file
~~~
