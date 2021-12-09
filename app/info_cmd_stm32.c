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

extern char _etext;
extern char _sdata;
extern char _edata;
extern char __bss_start__;
extern char __bss_end__;

reset_cause_t reset_cause;

static int cmd_info(nkinfile_t *args)
{
	if (nk_fscan(args, "")) {
                nk_printf("Firmware version %d.%d\n", firmware_major, firmware_minor);
                nk_printf("Build date: %4.4d-%2.2d-%2.2d %2.2d:%2.2d\n", build_year, build_month, build_day, build_hour, build_minute);
                nk_printf("Git hash: %s\n", git_hash);
	        nk_printf("CPU frequency = %lu\n", HAL_RCC_GetHCLKFreq());
	        nk_printf("HCLKFreq = %lu\r\n", (unsigned long)HAL_RCC_GetHCLKFreq());
	        nk_printf("PCLK1Freq = %lu\r\n", (unsigned long)HAL_RCC_GetPCLK1Freq());
	        //nk_printf("PCLK2Freq = %lu\r\n", (unsigned long)HAL_RCC_GetPCLK2Freq());
	        nk_printf("Reset cause = %s\n", reset_cause_get_name(reset_cause));
	        nk_printf("Memory footprint:\n");
		nk_printf("  _sdata = %p\n", &_sdata);
		nk_printf("  _edata = %p\n", &_edata);
		nk_printf("  __bss_start__ = %p\n", &__bss_start__);
		nk_printf("  __bss_end__ = %p\n", &__bss_end__);
		nk_printf("  _etext = %p\n", &_etext);
		nk_printf("  an address in current stack = %p\n", &args);
		nk_printf("  an address in current text = %p\n", &cmd_info);
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
