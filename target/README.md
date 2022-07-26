# Example targets

Each directory here includes vendor produced HAL files for their chip on one
of their development boards.

[atmel/](atmel/)

[st/](st/)

Their Makefile has been modified to include libnklabs and an example
application.  To build any of these examples:

Connect USB cable between computer and target development board, then:

	cd target/st/nucleo-f103rb

	make

	make flash

These target boards all have embedded debuggers.  A command line interface
is available on the target MCU's UART that is connected to the embedded
debugger's USB to serial port adapter.  You can access it with picocom:

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

Before building OpenOCD, install HIDAPI.  It's required for OpenOCD's CMSIS-DAP interface:

[https://github.com/signal11/hidapi](https://github.com/signal11/hidapi)

Install "picocom" to connect your terminal emulator to a USB serial port:

	sudo apt-get install picocom
	sudo apt-get install lrzsz

## Disable Modem Manager

The USB to serial adapter typically provided on embedded debuggers shows up
in Linux as a modem.  The Linux modem manager will try to talk to it (you'll
see ATxx commands being sent to the MCU after you connect with picocom).  To
prevent this:

	sudo systemctl disable ModemManager.service

Alternatively, you can try this (if you need the modem manager): It did not
work on the ATSAMD21 Xplained Pro board, but did on the others.  Modify the
file /lib/systemd/system/ModemManager.service

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
