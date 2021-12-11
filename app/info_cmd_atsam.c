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

// Print information about firmware and system

extern char _sfixed;
extern char _efixed;

extern char _srelocate;
extern char _erelocate;
extern char _sbss;
extern char _ebss;

static int cmd_info(nkinfile_t *args)
{
	if (nk_fscan(args, "")) {
                nk_printf("Firmware version %d.%d\n", firmware_major, firmware_minor);
                nk_printf("Build date: %4.4d-%2.2d-%2.2d %2.2d:%2.2d\n", build_year, build_month, build_day, build_hour, build_minute);
                nk_printf("Git hash: %s\n", git_hash);

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
		nk_printf("Flash size = %d\n", NK_MCUFLASH_SIZE);

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
