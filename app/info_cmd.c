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

#ifdef NK_PLATFORM_ATSAM
#include "peripheral_clk_config.h"
extern char _sfixed;
extern char _efixed;
extern char _srelocate;
extern char _erelocate;
extern char _sbss;
extern char _ebss;
#endif

#ifdef NK_PLATFORM_STM32
extern char _etext;
extern char _sdata;
extern char _edata;
extern char __bss_start__;
extern char __bss_end__;

reset_cause_t reset_cause;
#endif

static int cmd_info(nkinfile_t *args)
{
	if (nk_fscan(args, "")) {
                nk_printf("Firmware version %d.%d\n", firmware_major, firmware_minor);
                nk_printf("Build date: %4.4d-%2.2d-%2.2d %2.2d:%2.2d\n", build_year, build_month, build_day, build_hour, build_minute);
                nk_printf("Git hash: %s\n", git_hash);
                nk_printf("Target platform: %s\n", NK_PLATFORM);

#ifdef NK_PLATFORM_STM32
		nk_udelay(0);
		nk_printf("Loops per ms = %lu\n", nk_ticks_per_ms);
	        nk_printf("CPU frequency = %lu\n", HAL_RCC_GetHCLKFreq());
	        nk_printf("HCLKFreq = %lu\r\n", (unsigned long)HAL_RCC_GetHCLKFreq());
	        nk_printf("PCLK1Freq = %lu\r\n", (unsigned long)HAL_RCC_GetPCLK1Freq());
	        //nk_printf("PCLK2Freq = %lu\r\n", (unsigned long)HAL_RCC_GetPCLK2Freq());
	        nk_printf("Reset cause = %s\n", reset_cause_get_name(reset_cause));
#endif

#ifdef NK_PLATFORM_ATSAM
	        nk_printf("CPU frequency = %u\n", CONF_CPU_FREQUENCY);
#ifdef CHIPID
	        nk_printf("CHIPID_CIDR = %lx\n", CHIPID->CHIPID_CIDR);
	        nk_printf("CHIPID_EXID = %lx\n", CHIPID->CHIPID_EXID);
#endif
#endif

#ifdef NK_PLATFORM_ATSAM
#ifndef __SAME54P20A__ // _get_reset_reason() is missing for same54? why?
		enum reset_reason x = _get_reset_reason();
		nk_printf("Reset reason code = %d (", x);

// These are enums.. no easy way to use macros.. yuck..

#ifdef __SAMD21J18A__
		switch(x) {
                    case RESET_REASON_POR: nk_printf("POR"); break;
                    case RESET_REASON_BOD12: nk_printf("BOD12"); break;
                    case RESET_REASON_BOD33: nk_printf("BOD33"); break;
                    case RESET_REASON_EXT:  nk_printf("EXT"); break;
                    case RESET_REASON_WDT:  nk_printf("WDT"); break;
                    case RESET_REASON_SYST:  nk_printf("SYST"); break;
                    default: nk_printf("unknown"); break;
		}
#endif

#ifdef __SAME54P20A__
		switch(x) {
                    case RESET_REASON_POR: nk_printf("POR"); break;
                    case RESET_REASON_BOD12: nk_printf("BOD12"); break;
                    case RESET_REASON_BOD33: nk_printf("BOD33"); break;
                    case RESET_REASON_NVM: nk_printf("NVM"); break;
                    case RESET_REASON_EXT:  nk_printf("EXT"); break;
                    case RESET_REASON_WDT:  nk_printf("WDT"); break;
                    case RESET_REASON_SYST:  nk_printf("SYST"); break;
                    case RESET_REASON_BACKUP:  nk_printf("BACKUP"); break;
                    default: nk_printf("unknown"); break;
		}
#endif

#ifdef __SAME70Q21__
		switch(x) {
                    case RESET_TYPE_GENERAL: nk_printf("GENERAL (power loss..)"); break;
                    case RESET_TYPE_BACKUP: nk_printf("BACKUP"); break;
                    case RESET_TYPE_WDT: nk_printf("WDT"); break;
                    case RESET_TYPE_SOFT:  nk_printf("SOFT"); break;
                    case RESET_TYPE_USER:  nk_printf("USER (reset button..)"); break;
                    case RESET_TYPE_CPU_FAIL:  nk_printf("CPU_FAIL"); break;
                    case RESET_TYPE_SLCK_XTA:  nk_printf("SLCK_XTA"); break;
                    default: nk_printf("unknown"); break;
		}
#endif

		nk_printf(")\n");

#endif
#endif

	        nk_printf("Memory footprint:\n");

#ifdef NK_PLATFORM_ATSAM
		nk_printf("  _sdata = %p\n", &_srelocate);
		nk_printf("  _edata = %p\n", &_erelocate);
		nk_printf("  __bss_start__ = %p\n", &_sbss);
		nk_printf("  __bss_end__ = %p\n", &_ebss);
		nk_printf("  _sfixed = %p\n", &_sfixed);
		nk_printf("  _efixed = %p\n", &_efixed);
		nk_printf("  an address in current stack = %p\n", &args);
		nk_printf("  an address in current text = %p\n", &cmd_info);
		nk_printf("Flash size = %d\n", NK_MCUFLASH_SIZE);
#endif

#ifdef NK_PLATFORM_STM32
		nk_printf("  _sdata = %p\n", &_sdata);
		nk_printf("  _edata = %p\n", &_edata);
		nk_printf("  __bss_start__ = %p\n", &__bss_start__);
		nk_printf("  __bss_end__ = %p\n", &__bss_end__);
		nk_printf("  _etext = %p\n", &_etext);
		nk_printf("  an address in current stack = %p\n", &args);
		nk_printf("  an address in current text = %p\n", &cmd_info);
#endif


	} else {
		nk_printf("Syntax error\n");
	}
	return 0;
}

COMMAND(cmd_info,
    ">info                      Display serial number and firmware information\n"
    "-info                      Display serial number and firmware information\n"
)
