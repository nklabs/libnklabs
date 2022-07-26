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

#include "lib.h"

#define SCHED_TIMER XPAR_XTTCPS_0_DEVICE_ID
#define SCHED_TIMER_IRQ_ID XPAR_XTTCPS_0_INTR
#define SCHED_TIMER_HZ XPAR_XTTCPS_0_CLOCK_HZ
#define SCHED_TIMER_PRESCALE 128
#define SCHED_TIMER_PRESCALE_LOG2 6
// 0 means divide by 2
// 1 means divide by 4
// ...
// 16 means prescaler is disabled.

XTtcPs sched_timer;

static uint32_t current_time; // The current time
static uint32_t timeout; // Amount of delay from timer

uint32_t nk_get_sched_time(void)
{
	return current_time;
}

uint32_t nk_get_sched_timeout(void)
{
	return timeout;
}

uint32_t nk_convert_delay(uint32_t delay)
{
	return ((unsigned long long)delay * (SCHED_TIMER_HZ / 1000)) / SCHED_TIMER_PRESCALE;
}

// Called by stimer, see osc32khz module
void sched_isr(void *data)
{
	u32 StatusEvent;
	UNUSED(data);
	current_time += timeout;
	timeout = 0;
	XTtcPs_Stop(&sched_timer);
	StatusEvent = XTtcPs_GetInterruptStatus(&sched_timer);
	XTtcPs_ClearInterruptStatus(&sched_timer, StatusEvent);
}

void nk_init_sched_timer()
{
	current_time = 0;
	XTtcPs_Config *config = XTtcPs_LookupConfig(SCHED_TIMER);
	if (config) {
		u32 StatusEvent;
		nk_printf("Got SchTimer config\n");
		XTtcPs_CfgInitialize(&sched_timer, config, config->BaseAddress);
		// Timer is fed by LPD_LSBUS_CLK, which is 100 MHz
		// Prescaler is 16 bits
		// Timer is 32 bits

		XTtcPs_SetOptions(&sched_timer, XTTCPS_OPTION_INTERVAL_MODE | XTTCPS_OPTION_WAVE_DISABLE);

		XTtcPs_Stop(&sched_timer);
		XTtcPs_SetPrescaler(&sched_timer, SCHED_TIMER_PRESCALE_LOG2);

		StatusEvent = XTtcPs_GetInterruptStatus(&sched_timer);
		XTtcPs_ClearInterruptStatus(&sched_timer, StatusEvent);
		XTtcPs_EnableInterrupts(&sched_timer, XTTCPS_IXR_INTERVAL_MASK);

		XScuGic_Connect(&interrupt_controller, SCHED_TIMER_IRQ_ID, (Xil_ExceptionHandler)sched_isr, (void *)&sched_timer);
		XScuGic_Enable(&interrupt_controller, SCHED_TIMER_IRQ_ID);
	}

	// XTtcPs_LoadTimer(&sched_timer, value);
	// XTtcPs_Start(&sched_timer);
}

// delay in timer ticks
void nk_start_sched_timer(uint32_t delay)
{
	timeout = delay;
	// if (delay < 1)
	//	delay = 1;
	delay = delay + 1;


	XTtcPs_Stop(&sched_timer);
	XTtcPs_ResetCounterValue(&sched_timer);
	XTtcPs_SetInterval(&sched_timer, delay);
	XTtcPs_Start(&sched_timer);
}

// Get current time

nk_time_t nk_get_time()
{
	XTime the_time;
	XTime_GetTime(&the_time);

#if 0
	// XTime_GetTime just reads the ARM timer
	XTime_StartTimer();
	the_time = mfcp(CNTPCT_EL0);
#endif

	return the_time;
}

// Busy loop delay

void nk_udelay(unsigned long usec)
{
	// Just use the Xilinx one...
	usleep(usec);
#if 0
	// Generic implementation
	nk_time_t old = nk_get_time();
	nk_time_t clocks = usec * NK_TIME_COUNTS_PER_USECOND;
	while ((nk_get_time() - old) < clocks);
#endif
}
