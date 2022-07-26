## Atmel ATSAMxx targets

A number of "XPLAINED PRO" evaluation boards are avilable from
Microchip/Atmel:

[https://www.microchip.com/en-us/development-tool/atsamd21-xpro](https://www.microchip.com/en-us/development-tool/atsamd21-xpro)

[https://www.microchip.com/en-us/development-tool/atsame54-xpro](https://www.microchip.com/en-us/development-tool/atsame54-xpro)

[https://www.microchip.com/en-us/development-tool/ATSAME70-XPLD](https://www.microchip.com/en-us/development-tool/ATSAME70-XPLD)

### Atmel ASF4 and Atmel Start

Atmel Start is used to configure ASF4 (Atmel's "Advance Software
Framework"), which includes the HAL and startup code for ATSAMxxx
microcontrollers.

Atmel Start is a web GUI tool that allows you select components of ASF4 for
your specific project.  It also allows you to set up the clocking and pinout
using GUI tools.  Atmel Start can be accessed here:

[https://start.atmel.com/](https://start.atmel.com/)

This is the file produced by Atmel Start.  It's a .zip file, for example for
the ATSAMD21 XPLAINED PRO target, it's this one:

        atsamd21pro/atsamd21pro.atzip

This contains the unzipped contents of the .atzip file (the Makefile will
automatically unzip it):

        atsamd21pro/atmel_start/

Important! None of the files in atmel_start/ should be modified since Atmel Start will
overwrite them.

The build directory is here:

        atsamd21pro/build/

It contains a modified copy of atmel_start/gcc/Makefile.

The Makefile is modified so that it extracts the file lists from
atmel_start/gcc/Makefile.  In this way, the atsamd21pro/build/ Makefile does
not have to be modified after Atmel Start changes the project.

To update the Atmel Start project, follow this procedure:

1. Go the Atmel Start web page

2. Click on "Load Project from File"

3. Click browse, and select atsamd21pro.atzip

4. Click "Open Selected File"

5. Make your changes to the project

6. Click "Export Project" tab

7. Make sure Makefile(standalone) is checked.

8. Set the download name you want to use in "Specify file name" box

8. Click on "Download Pack"

9. Copy the downloaded .atzip file to overwrite the atsamd21pro.atzip file
in the repository

10. Type make clean followed by make

