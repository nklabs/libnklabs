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

#include "stm32g0xx_ll_system.h"
#include "stm32g0xx_ll_utils.h"

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
        nkinfile_t f[1];
        nkinfile_open_mem(f, 0, old_addr + len);
	nk_byte_hex_dump(old_addr, len, f);
	old_addr += len;
    } else if (facmode && nk_fscan(args, "hd %lx ", &old_addr)) {
        nkinfile_t f[1];
    	len = 0x100;
        nkinfile_open_mem(f, 0, old_addr + len);
	nk_byte_hex_dump(old_addr, len, f);
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
    "mem hd <addr> <len>       Hex dump memory\n",
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

extern char _etext;
extern char _sdata;
extern char _edata;
extern char __bss_start__;
extern char __bss_end__;

reset_cause_t reset_cause;

static int cmd_info(nkinfile_t *args)
{
        char lot[8];
        uint32_t idxy;
        uint32_t idlow;
        uint32_t idhigh;
	if (nk_fscan(args, "")) {
#ifdef DEV_BOARD
                nk_printf("Dev. board\n");
#else
                nk_printf("Proto board\n");
#endif
                // nk_printf("Model %s\n", model.model);
                // nk_printf("Serial number %s\n", cal.serno);
                nk_printf("Firmware version %d.%d\n", firmware_major, firmware_minor);
#ifdef STM32G070xx
                nk_printf("for STM32G070xx\n");
#endif
#ifdef STM32G071xx
                nk_printf("for STM32G071xx\n");
#endif
                nk_printf("Build date: %4.4d-%2.2d-%2.2d %2.2d:%2.2d\n", build_year, build_month, build_day, build_hour, build_minute);
	        nk_printf("CPU frequency = %lu\n", HAL_RCC_GetHCLKFreq());
	        nk_printf("CPU IDCODE = %lx REV = %lx\n", LL_DBGMCU_GetDeviceID(), LL_DBGMCU_GetRevisionID());
	        idxy = LL_GetUID_Word0();
	        // idxy = *(uint32_t *)0x1fff7590;
	        // idlow = *(uint32_t *)0x1fff7594;
	        idlow = LL_GetUID_Word1();
	        // idhigh = *(uint32_t *)0x1fff7598;
	        idhigh = LL_GetUID_Word2();
	        memcpy(lot, (char *)&idlow + 1, 3);
	        memcpy(lot + 3, (char *)&idhigh, 4);
	        lot[7] = 0;
	        nk_printf("CPU serial no.: die %lx/%lx wafer %lu lot %s\n",
	            (0xFFFF & idxy),
	            (0xFFFF & (idxy >> 16)),
	            (0xFF & idlow),
	            lot
                );
	            
	        nk_printf("Reset cause = %s\n", reset_cause_get_name(reset_cause));
	        //nk_printf("Current time = %u\n", reg_wallclock);
	        nk_printf("Memory footprint:\n");
		nk_printf("_sdata = %p\n", &_sdata);
		nk_printf("_edata = %p\n", &_edata);
		nk_printf("__bss_start__ = %p\n", &__bss_start__);
		nk_printf("__bss_end__ = %p\n", &__bss_end__);
		nk_printf("_etext = %p\n", &_etext);
		nk_printf("an address in current stack = %p\n", &args);
		nk_printf("an address in current text = %p\n", &cmd_info);
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
