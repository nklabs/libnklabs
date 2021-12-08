# Example targets

Each directory here includes vendor produced HAL files for their chip on one
of their development boards.  Their Makefile has been modified to include
libnklabs and an example application.  To build any of these examples:

Connect USB cable between computer and target board, then:

	cd target/nucleo-f103rb

	make

	make flash

A command line interface is available on the MCU UART that is connected to
the embedded debugger's USB to serial port adapters.  You can access it with
picocom:

	sudo picocom --baud 115200 /dev/ttyACM0

When the MCU resets, you should see something like:

	[Initialize] Console UART
	[Initialize] Work queue
	[Initialize] Command Line Interface
	[Initialize] Begin main loop
	>

There is on-line help:

    >help
    help <name> for help with a specific command

    Available commands:

    help                      Help command
    info                      Display serial number and firmware information
    reboot                    Reboot system
    mem                       Read/write memory
    work                      Show work queue
    i2c                       I2C access

# Prerequisites

GCC 6.3.1 for ARM, aka gcc-arm-none-eabi-6-2017-q2-update.  Get from here:

[https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads/6-2017-q2-update](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads/6-2017-q2-update)

OpenOCD 0.11 or higher (for Microchip/Atmel's ATSAM parts):

[https://sourceforge.net/p/openocd/code/ci/master/tree/](https://sourceforge.net/p/openocd/code/ci/master/tree/)

OpenOCD requires HIDAPI for the CMSIS-DAP interface:

[https://github.com/signal11/hidapi](https://github.com/signal11/hidapi)

STM32CubeProgrammer

[https://www.st.com/en/development-tools/stm32cubeprog.html](https://www.st.com/en/development-tools/stm32cubeprog.html)

Install this in /opt/STM32CubeProgrammer

STM32CubeMX ST's "STM32Cube initialization code generator"

[https://www.st.com/en/development-tools/stm32cubemx.html](https://www.st.com/en/development-tools/stm32cubemx.html)

Install "picocom" to connect your terminal emulator to a USB serial port:

	sudo apt-get install picocom
	sudo apt-get install lrzsz

## Disable Modem Manager

The USB to serial adapter typically provided on embedded debuggers shows up
in Linux as a modem.  The Linux modem manager will try to talk to it (you'll
see ATxx commands being sent to the MCU after you connect with picocom).  To
prevent this:

Modify the file /lib/systemd/system/ModemManager.service

        [Unit]
        Description=Modem Manager

        [Service]
        Type=dbus
        BusName=org.freedesktop.ModemManager1
        ExecStart=/usr/sbin/ModemManager --filter-policy=default   <-- Change from strict to default
        StandardError=null
        Restart=on-abort
        CapabilityBoundingSet=CAP_SYS_ADMIN
        ProtectSystem=true
        ProtectHome=true
        PrivateTmp=true
        RestrictAddressFamilies=AF_NETLINK AF_UNIX
        NoNewPrivileges=true
        User=root
        Environment="MM_FILTER_RULE_TTY_ACM_INTERFACE=0"     <-- Add this line

        [Install]
        WantedBy=multi-user.target
        Alias=dbus-org.freedesktop.ModemManager1.service

Then:

        systemctl daemon-reload
        systemctl ModemManager.service

## STM32 targets

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

