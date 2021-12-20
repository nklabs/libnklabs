# NK Labs common library

## C-language Software library

This is our library for very quickly making products based on common
microcontrollers.  It is a layer added on top of the vendor libraries that
provides features required for almost every product.

It is important to work with and not replace the vendor tools and libraries
in commercial products.  First, some vendor tools facilitate rapid
development.  In particular:

* STM32CubeMX allows you to instantiate peripherals, define the pinouts, set up clocking and generate startup code for ST Mirco's MCUs.

* Atmel Start is a web tool that does the same Microchip/Atmel's ATSAM ARM microcontrollers.

These tools do have a learning curve that we believe is worth overcoming. 
The example application provided here can be a good starting point for this.

It is important to note that these tools are not IDEs.  They can target
IDEs, but they also can generate a command line build process using Make and
Gcc.  We prefer this since it facilites automated testing and building.

Second, very importantly, if you want technical support you must use the
vendor tools.

Back to our library: it provides these features:

* User interface

In this case, a UART-based command line interface (CLI) with history,
editing and tab-completion.  Commands are defined near the code they intend
to use- there is no need to edit a central command table, or call any
registration functions.  This makes it easy to casually add commands for
debugging or bring-up.

The CLI is powerful enough to be customer facing.  There is on-line help and
commands can be marked to be disabled and hidden unless your product is in
some kind of "factory service mode".

Even if you do not need or want a customer facing CLI, having a CLI almost
always saves time.  It provides a way for team members to use the product
before it is finished.  For example, an Electrical Engineer might have to
focus on a specific peripheral for qualification.  Commands can be easily
provided to allow him or her to be independent, not needing a firmware
engineer to be help for every action.

* Firmware updates

Almost every product needs some kind of remote firmware update capabilty. 
This library provides one way to do it which can be extended to match your
needs:

First, you can download a firmware update file into local flash memory using
the Y-MODEM protocol over the CLI.  Y-MODEM is an old standard protocol for
transferring files over a serial port that is built into most terminal /
modem programs, such as TeraTerm and PuTTY.  This is nice, since no
host-side software has to be developed.

Second, after reboot, a bootloader copies the firmware from the local flash
memory (after doing a final integrity check) to the main MCU flash memory to
complete the update.

This local flash memory could be unused space in the MCU flash memory, or it
could be an external device such as a SPI-flash chip.

* Embedded database

Many products need some kind of non-volatile storage for calibration and
configuration settings.  We provide a schema-defined database with schema
change / migration support for this.  The idea is to save the serialized
version of a memory structure into non-volatile memory such as SPI-flash. 
On boot up, the database is deserialized back into RAM.  Fields can be added
or deleted as the product evolves.  New fields will be loaded with a default
value, and date for deleted fields will be ignored.

The serialized format of a database can also be transferred over the CLI. 
This allows you to save the database on a host computer.

The CLI provides an automatic interface to set or get individual fields from
the database.

* Common drivers

Some basic drivers for common devices are included:

* Flash memory
* EEPROM
* Real Time Clock

This library provides functionality useful for embedded applications on
small microcontrollers:

* A command line interface (CLI) with line editor, on-line help and history. 
  Commands are declared statically anywhere in the project instead of
  requiring a central command table or the calling of registration functions.

* A schema-defined database with schema change  / migration support,
  typically used for holding calibration and configuration parameters in local flash memory

* A serialization / de-serialization system so that databases can be transferred over the CLI
* Y-MODEM protocol for transferring files over the CLI, for example for firmware updates
* Replacement printf() that supports floating point but is much smaller than the newlib one
* Repacement scanf() designed for parsing instead of simple input reading. 
  CLI commands use this to parse command line arguments.
* A work-queue scheduler which can be used in place of an RTOS if preemptive multitasking is not required
* Commonly used CRC and cryptographic functions including SHA and AES
* Support for firmware version numbers and automatic build-date insertion
* CLI commands for many basic things, such as access to SPI and I2C devices
* Real time clock support
* Watchdog timer support
* Malloc is not used at all to ensure determinism

### Including libnklabs in your own project

The library is designed to be modular, though many of the modules depend on
each other.

Each module includes C source and header files.  But many also include a
configuration header file (.h) for customization options.  The idea is that
configuration header files should be tailored for the specific project, but
the source files should not be modified.

The configuration and header files are located in their own directories. 
These must be added to the include file search path, for example:

	gcc -I../config -I../inc -c nkscan.c

It is recommended to use -Os and link-time optimizations: these save space
by eliminating unused code.

### Unit tests

To execute the unit tests:

	cd tests
	make

### Examples

An example application is available in [app/](app/).

It can be compiled for various targets, see [target/](target/).

To build a particular example and install it on a target board:

	cd target/nucleo-g071rb
	make
	make flash

### Module Documentation

[nksched - minimal work queue scheduler](doc/nksched.md)

[nkarch - processor specific functions](doc/nkarch.md)

[nkreadline - command line editor](doc/nkreadline.md)

[nkcli - command line interface](doc/nkcli.md)

[nkscan - formatted input](doc/nkscan.md)

[nkinfile - input abstraction](doc/nkinfile.md)

[nkprintf - formatted output](doc/nkprintf.md)

[nkoutfile - output abstraction](doc/nkoutfile.md)

[nkserialize - schema driven serialization](doc/nkserialize.md)

[nkdbase - schema driven database](doc/nkdbase.md)

[nkzmodem - ZMODEM protocol](doc/nkzmodem.md)

# Licensing

The MIT License file is called "nklib_mit_license.txt".  Add the contents to
the top of all new source and header files.
