# Application CLI reference

## Boot up

On power up, or after "reboot" command you will see some boot messages:

~~~
>[Initialize] Work queue
[Initialize] Command Line Interface
[Initialize] Database
Using bank 1
Calibration store loaded OK
[Initialize] LED
[Initialize] Button
[Initialize] External Real Time Clock
  Setting MCU RTC
[Initialize] WDT
[Initialize] Begin main loop
>
~~~

## dbase - Database Store

~~~
dbase clear               Clear database store
dbase show                Show entire database
dbase load                Load database from flash
dbase save                Save database to flash
dbase replace             Replace database: prompts for multi-line value
dbase get <key>           Get value assigned to key
dbase set <key> <value>
                          Assign value to a key
dbase set <key>           Assign value to a key: Prompts for multi-line value
~~~

### "dbase show" example

"dbase show" shows the values of the live RAM copy of the database ("testdb"
in database.c):

~~~
>dbase show
BEGIN
{
  tstruct: {
    tbool: true,
    tint: 2147483646,
    tuint: 4026531838,
    tint8: 126,
    tuint8: 238,
    tint16: 32766,
    tuint16: 61438,
    tdouble: 0.125,
    tfloat: 0.0625,
    tstring: "Hello"
  },
  tarray: [
    {
      tstring: "Hello",
      tfloat: 0.0625,
      tdouble: 0.125,
      tuint16: 61438,
      tint16: 32766,
      tuint8: 238,
      tint8: 126,
      tuint: 4026531838,
      tint: 2147483646,
      tbool: false
    },
    {
      tstring: "Hello",
      tfloat: 0.0626,
      tdouble: 0.125,
      tuint16: 61438,
      tint16: 32766,
      tuint8: 238,
      tint8: 126,
      tuint: 4026531838,
      tint: 2147483646,
      tbool: false
    }
  ],
  tvararray: [
    {
      tbool: true,
      tint: 2147483646,
      tuint: 4026531838,
      tint8: 126,
      tuint8: 238,
      tint16: 32766,
      tuint16: 61438,
      tdouble: 0.126,
      tfloat: 0.0625,
      tstring: "Hello"
    },
    {
      tbool: true,
      tint: 2147483646,
      tuint: 4026531838,
      tint8: 126,
      tuint8: 238,
      tint16: 32766,
      tuint16: 61438,
      tdouble: 0.127,
      tfloat: 0.0625,
      tstring: "Hello"
    }
  ],
  ttable:
    (   tstring tfloat  tdouble tuint16 tint16  tuint8  tint8   tuint   tint    tbool
    :   "Hello" 0.063   0.125   61438   32766   238     126     4026531838      2147483646      false
    :   "Hello" 0.0631  0.125   61438   32766   238     126     4026531838      2147483646      false
    )
}
END
~~~

### "dbase get" example

"dbase get" shows a part of the live RAM copy of the database.

~~~
>dbase get tstruct.tfloat
BEGIN
0.0625
END
>dbase get tvararray[1].tint8
BEGIN
126
END
>
>dbase get ttable
BEGIN
(       tstring tfloat  tdouble tuint16 tint16  tuint8  tint8   tuint   tint    tbool
:       "Hello" 0.063   0.125   61438   32766   238     126     4026531838      2147483646      false
:       "Hello" 0.0631  0.125   61438   32766   238     126     4026531838      2147483646      false
)
END
>dbase get ttable[1]
BEGIN
{
  tstring: "Hello",
  tfloat: 0.0631,
  tdouble: 0.125,
  tuint16: 61438,
  tint16: 32766,
  tuint8: 238,
  tint8: 126,
  tuint: 4026531838,
  tint: 2147483646,
  tbool: false
}
END
~~~

### "dbase set" example

"dbase set" sets a part of the live RAM copy of the database.

~~~
>dbase set tstruct.tfloat 1.5
Setting tstruct.tfloat to 1.5
>dbase get tstruct.tfloat
BEGIN
1.5
END
>
~~~

### "dbase save" example

"dbase save" writes the live RAM copy of the database ("testdb" in
database.c) to flash.  Note that every time the database is saved, the rev
number is bumped.  On boot up, the latest version is loaded back into RAM.

There are two banks in the flash: bank 0 and bank 1.  "dbase save"
overwrites the bank with the older revision in it.  If a "dbase save" is
interrupted by a power, the other bank will still be valid.

~~~
>dbase save
Saving to bank 1...
Writing...
  size = 985
  rev = 5
done.
>
~~~

### "dbase load" example

"dbase load" reads the latest revision of the database from flash into RAM
(into "testdb" in database.c):

~~~
>dbase load
Using bank 1
Calibration store loaded OK
>
~~~

### "dbase clear" example

"dbase clear" replaces the values in the live RAM database ("testdb" in
database.c) with defaults specified in the firmware ("testdb_defaults" in
database.c).  It then performs "dbase save" twice so that both banks of the
flash have the default values.

~~~
>dbase clear
Clearing calibration store
Saving to bank 0...
Writing...
  size = 988
  rev = 8
done.
Saving to bank 1...
Writing...
  size = 988
  rev = 9
done.
done.
Please power cycle!
>
~~~

## echo - Control command echoing

~~~
echo on                   Enable echoing
echo off                  Disable echoing
echo                      Show current echo mode
~~~

## extrtc - Access PCF8523 Real Time Clock on Adalogger Shield

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
~~~

## i2c - I2C bus access

~~~
i2c scan                  Scan I2C bus
i2c [w AA DD ...] [r AA NN]
                          Raw I2C transaction
~~~

### "i2c scan" example

~~~
>i2c scan
Found device 54
Found device 55
Found device 56
Found device 57
Found device 5d
Found device 5e
Found device 5f
Found device 68
~~~

### "i2c w" example

~~~
>i2c w 68 00 r 68 8
 write 68 0 read addr=68 len=8 got back: 0 0 8 11 1 11 3 1
>i2c w 68 00 r 68 8
 write 68 0 read addr=68 len=8 got back: 0 0 8 13 1 11 3 1
>i2c w 68 00 r 68 8
 write 68 0 read addr=68 len=8 got back: 0 0 8 16 1 11 3 1
>
~~~

"w 68 0" means we write 0 to I2C device 68 (a real time clock).  This sets
the register address pointer within the device to 0.

"r 68 8" means we read 8 bytes from device 68.  You can see the seconds
incrementing.

## info - Display serial number and firmware information

~~~
info                      Display serial number and firmware information
~~~

### info example

~~~
>info
Firmware version 0.0
Build date: 2022-01-03 16:11
Git hash: 1b341c0dbad2faea4c8ced0eed85de9555f487e5-dirty
Target platform: NK_PLATFORM_STM32
CPU frequency = 32000000
HCLKFreq = 32000000
PCLK1Freq = 32000000
Reset cause = SOFTWARE_RESET
Memory footprint:
  _sdata = 20000000
  _edata = 200006c0
  __bss_start__ = 200006c0
  __bss_end__ = 20001ec0
  _etext = 800db2c
  an address in current stack = 20008ef4
  an address in current text = 8005a21
>
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

### "work" example

~~~
>work
Current time = 534248
Pending tasks:
 "Button poller": tid=3 func=button_poll(0x8004fdc) data=0x0 when=534296 [submitted by ../../../app/button.c:79]
 "LED blinker": tid=2 func=led_blinker(0x8005b74) data=0x0 when=534697 [submitted by ../../../app/led.c:42]
 "Watchdog timer poker": tid=4 func=wdt_poke(0x8006004) data=0x0 when=535197 [submitted by ../../../app/wdt.c:21]
End of list.
>
~~~

## ymodem - Transfer files over console UART

~~~
ymodem                    Receive a file
ymodem send               Send a file
~~~
