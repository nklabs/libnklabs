// Copyright 2020 NK Labs, LLC

// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:

// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
// OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
// THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <string.h>
#include "version.h"
#include "nkreadline.h"
#include "nkcli.h"

// Transmit Ctrl-S to test XON/XOFF

static int cmd_stop(nkinfile_t *args)
{
    if (facmode && nk_fscan(args, "")) {
        nk_putc('S'-'@');
        
    } else {
        nk_printf("Syntax error\n");
    }
    return 0;
}

COMMAND(stop,
    "stop                      Emit ^S\n",
    "stop                      Emit ^S\n",
    ""
)
 
// Control echoing

static int cmd_echo(nkinfile_t *args)
{
    if (nk_fscan(args, "on ")) {
        nk_set_echo(1);
        nk_printf("Echoing is enabled\n");
    } else if (nk_fscan(args, "off ")) {
        nk_set_echo(0);
        nk_printf("Echoing is disabled\n");
    } else if (nk_fscan(args, "")) {
        if (nk_get_echo()) 
            nk_printf("Echoing is enabled\n");
        else
            nk_printf("Echoing is disabled\n");
    } else {
        nk_printf("Syntax error\n");
    }
    return 0;
}

COMMAND(echo,
    "echo                      Control command echoing\n",
    "echo on                   Enable echoing\n"
    "echo off                  Disable echoing\n"
    "echo                      Show current echo mode\n",
    ""
)

// Read/Write/hex-dump memory

static uint32_t old_addr;

static int cmd_mem(nkinfile_t *args)
{
    uint32_t addr;
    uint32_t len;
    uint32_t val;
    if (facmode && nk_fscan(args, "rd %lx ", &addr)) {
        nk_printf("[%lx] has %lx\n", addr, *(uint32_t *)addr);
    } else if (facmode && nk_fscan(args, "wr %lx %x ", &addr, &val)) {
        *(uint32_t *)addr = val;
        nk_printf("Wrote %lx to [%lx]\n", val, addr);
    } else if (facmode && nk_fscan(args, "hd %lx %x ", &old_addr, &len)) {
	nk_byte_hex_dump((unsigned char *)0, 0, old_addr, len);
	old_addr += len;
    } else if (facmode && nk_fscan(args, "hd %lx ", &old_addr)) {
    	len = 0x100;
	nk_byte_hex_dump((unsigned char *)0, 0, old_addr, len);
	old_addr += len;
    } else if (facmode && nk_fscan(args, "hd ")) {
    	len = 0x100;
	nk_byte_hex_dump((unsigned char *)0, 0, old_addr, len);
	old_addr += len;
    } else {
        nk_printf("Syntax error\n");
    }
    return 0;
}

COMMAND(mem,
    "mem                       Read/write memory\n",
    "mem rd <addr>             Read 32-bit word from address\n"
    "mem wr <addr> <data>      Write 32-bit word to address\n"
    "mem hd <addr> <len>       Hex dump memory\n"
    "mem hd <addr>             Hex dump 256 bytes\n"
    "mem hd                    Hex dump next256 bytes\n",
    ""
)

// Reboot the system

static int cmd_reboot(nkinfile_t *args)
{
        if (nk_fscan(args, "")) {
            nk_printf("Rebooting...\n");
            nk_udelay(2000);
            reboot();
        } else {
            nk_printf("Syntax error\n");
        }
        return 0;
}


COMMAND(reboot,
    "reboot                    Reboot system\n",
    "reboot                    Reboot system\n",
    ""
)

// Print information about firmware and system

extern char _sfixed;
extern char _efixed;

extern char _srelocate;
extern char _erelocate;
extern char _sbss;
extern char _ebss;


#if 0
reset_cause_t reset_cause;
#endif

static int cmd_info(nkinfile_t *args)
{
	if (nk_fscan(args, "")) {
                nk_printf("Firmware version %d.%d\n", firmware_major, firmware_minor);
                nk_printf("Build date: %4.4d-%2.2d-%2.2d %2.2d:%2.2d\n", build_year, build_month, build_day, build_hour, build_minute);

#if 0
	        nk_printf("CPU frequency = %u\n", CONF_CPU_FREQUENCY);
	        nk_printf("CHIPID_CIDR = %lx\n", CHIPID->CHIPID_CIDR);
	        nk_printf("CHIPID_EXID = %lx\n", CHIPID->CHIPID_EXID);
#endif

	        //nk_printf("Current time = %u\n", reg_wallclock);
	        nk_printf("Memory footprint:\n");
		nk_printf("_sdata = %p\n", &_srelocate);
		nk_printf("_edata = %p\n", &_erelocate);
		nk_printf("__bss_start__ = %p\n", &_sbss);
		nk_printf("__bss_end__ = %p\n", &_ebss);
		nk_printf("_sfixed = %p\n", &_sfixed);
		nk_printf("_efixed = %p\n", &_efixed);
		nk_printf("an address in current stack = %p\n", &args);
		nk_printf("an address in current text = %p\n", &cmd_info);

#if 0
                // ATSAME70 only?
		x = _get_reset_reason();
		nk_printf("Reset reason = %d\n", x);
		switch(x) {
                    case RESET_TYPE_GENERAL: nk_printf("  GENERAL (power los..)\n"); break;
                    case RESET_TYPE_BACKUP: nk_printf("  BACKUP\n"); break;
                    case RESET_TYPE_WDT: nk_printf("  WDT\n"); break;
                    case RESET_TYPE_SOFT:  nk_printf("  SOFT\n"); break;
                    case RESET_TYPE_USER:  nk_printf("  USER (reset button..)\n"); break;
                    case RESET_TYPE_CPU_FAIL:  nk_printf("  CPU_FAIL\n"); break;
                    case RESET_TYPE_SLCK_XTA:  nk_printf("  SLCK_XTA\n"); break;
		}
#endif

	} else {
		nk_printf("Syntax error\n");
	}
	return 0;
}

COMMAND(info,
    "info                      Display serial number and firmware information\n",
    "info                      Display serial number and firmware information\n",
    ""
)
