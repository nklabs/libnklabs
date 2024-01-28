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
#include "nkprintf.h"
#include "nkmcuflash.h"
#include "nkarch_zynqmp.h"
#include "xil_printf.h"

// Convert delay in milliseconds to number of scheduler timer clock ticks
nk_time_t nk_convert_delay(uint32_t delay)
{
	return ((nk_time_t)delay * NK_TIME_COUNTS_PER_SECOND) / 1000;
}

void nk_init_sched_timer()
{
	// Nothing to do
}

void nk_sched_wakeup(nk_time_t when)
{
	// No need, since tick interrupts wake us up
	(void)when;
}

// Get current time

nk_time_t nk_get_time()
{
	XTime the_time;
	XTime_GetTime(&the_time);

	return the_time;
}

// Busy loop delay

void nk_udelay(unsigned long usec)
{
	// Just use the Xilinx one...
	usleep(usec);
}

void nk_reboot()
{
	// How?
}

XScuGic interrupt_controller;

void iz_intc(void)
{
	xil_printf("[Initialize] Interrupt controller\r\n");
	Xil_ExceptionInit();
	/* Initilize interrupt handler */
	XScuGic_Config *intc_config = XScuGic_LookupConfig(XPAR_SCUGIC_SINGLE_DEVICE_ID);
	if (intc_config) {
		XScuGic_CfgInitialize(&interrupt_controller, intc_config, intc_config->CpuBaseAddress);
		/* Install interrupt handler */
		Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler)XScuGic_InterruptHandler, &interrupt_controller);
		/* Enable interrupts */
		Xil_ExceptionEnable();
	} else {
		xil_printf("  Couldn't find config?\r\n");
	}
}
