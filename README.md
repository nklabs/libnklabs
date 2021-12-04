# NK Labs common library

## C-language Software library

This library provides functionality useful for embedded applications on
small microcontrollers:

* A command line interface (CLI) with line editor, on-line help and history. 
  Commands are declared statically anywhere in the project instead of
  requiring a central command table or registration functions.

* A schema-defined database with simple schema change  / migration support,
  typically used for holding calibration and configuration parameters in local flash memory

* A serialization / de-serialization system so that databases can be transferred over the CLI
* Y-MODEM protocol for transferring files over the CLI, for example for firmware updates
* Replacment printf() that supports floating point but is much smaller than the newlib one
* Repacement scanf() designed for command line parsing
* A work-queue scheduler which can be used in place of a RTOS if preemptive multitasking is not required
* Commonly used CRC and cryptographic functions including SHA and AES
* Support for firmware version numbers and automatic build-date insertion
* CLI commands for many basic things, such as access to SPI and I2C devices
* Real time clock support
* Watchdog timer support
* Malloc is not used at all for determinism

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
