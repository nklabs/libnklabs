# NK Labs common library

## C-language Software library

### Including libnklabs in your own project

Each module includes C source and header files.  But many also include a
configuration header file (.h) for customization options.  The idea is that
configuration header files should be tailored for the specific project, but
the source files should not be modified.

The configuration and header files are located in their own directories. 
These must be added to the include file search path, for example:

	gcc -I../config -I../inc -c nkscan.c

### Unit tests

To execute the unit tests:

	cd tests
	make

### Example targets

A number of example targets are provided in the target directory.  To build
a particular example and install it on a target board:

	cd target/nucleo-g071rb
	make
	make flash

### Modules

[nksched - minimal work queue scheduler](doc/nksched.md)

[nkarch - processor specific functions](doc/nkarch.md)

[nkreadline - command line editor](doc/nkreadline.md)

[nkcli - command line interface](doc/nkcli.md)

[nkscan - formatted input](doc/nkscan.md)

[nkinfile - input abstraction](doc/nkinfile.md)

[nkprintf - formatted output](doc/nkprintf.md)

[nkoutfile - output abstraction](doc/nkoutfile.md)

[nkserialize - schema driven serialization](doc/nkserialize.md)

[nkzmodem - ZMODEM protocol](doc/nkzmodem.md)

# Licensing

The MIT License file is called "nklib_mit_license.txt".  Add the contents to
the top of all new source and header files.
