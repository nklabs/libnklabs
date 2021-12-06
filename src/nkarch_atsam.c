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
#include "nkarch_atsam.h"

#define SCHED_TIMER_HZ 1000

static volatile uint32_t current_time; // The current time in ticks
static volatile uint32_t timeout; // Amount of delay from timer in ticks

uint32_t nk_get_sched_time(void)
{
	return current_time;
}

uint32_t nk_get_sched_timeout(void)
{
	return timeout;
}

// Number of milliseconds per tick

// Convert delay in milliseconds to number of scheduler timer ticks
uint32_t nk_convert_delay(uint32_t delay)
{
	return delay * SCHED_TIMER_HZ / 1000;
}

// Timer callback: execution context for this is timer service task

static void timer_callback(const struct timer_task *const timer_task)
{
	current_time += timeout;
	timeout = 0;
}

void nk_init_sched_timer()
{
	current_time = 0;
}

static struct timer_task sched_timer_task;

// delay in timer ticks
void nk_start_sched_timer(uint32_t delay)
{
	timeout = delay;
	sched_timer_task.interval = timeout;
	sched_timer_task.cb = timer_callback;
	sched_timer_task.mode = TIMER_TASK_ONE_SHOT;
	timer_add_task(&TIMER_0, &sched_timer_task);
	timer_start(&TIMER_0);
}

// Get current time

nk_time_t nk_get_time()
{
	return current_time;
}

// Busy loop delay
// Also use sched timer...

void nk_udelay(unsigned long usec)
{
	unsigned long ms = usec / 1000;
	unsigned long us = usec - (ms * 1000);
	// Atmel
	if (ms)
		delay_ms(ms);
	if (us)
		delay_us(us);

	// Generic implementation
	//nk_time_t old = nk_get_time();
	//nk_time_t clocks = usec * (NK_TIME_COUNTS_PER_SECOND / 1000000);
	//while ((nk_get_time() - old) < clocks);
}

void reboot(void)
{
	NVIC_SystemReset();
}


int nk_init_mcuflash()
{
	return 0;
}

int nk_mcuflash_erase(uint32_t address, uint32_t byte_count)
{
	int rtn = 0;
	// As long as we're not done..
	while (byte_count) {
		// Try to use largest erase that works..
		if ((address & 8191) == 0 && byte_count >= 8192) {
			rtn = flash_erase(&FLASH_0, address, 1);
			if (rtn) break;
			address += 8192;
			byte_count -= 892;
		} else {
			// None work? Fail..
			break;
		}
	}

	if (rtn) {
		nk_printf("ERROR: flash_erase returned %d\n", rtn);
	} else if (byte_count) {
		nk_printf("ERROR: Invalid erase size\n");
		return -1;
	}
	return 0;
}

int nk_mcuflash_write(uint32_t address, uint8_t *data, uint32_t byte_count)
{
	int rtn = 0; // Assume success

	uint32_t page_size = 512;

	// Write a page at a time
	while (byte_count) {
		uint32_t page_offset = (address & (page_size - 1)); // Starting offset within page
		uint32_t page_len = page_size - page_offset; // Up to one page

		if (byte_count < page_len)
			page_len = byte_count;

		rtn = flash_write(&FLASH_0, address, data, page_len);
		if (rtn)
			break;

		byte_count -= page_len;
		address += page_len;
		data += page_len;
	}

	if (rtn) {
		nk_printf("ERROR: Flash_write returned %d\n", rtn);
	}

	return rtn;
}

int nk_mcuflash_read(uint32_t address, uint8_t *data, uint32_t byte_count)
{
	int rtn = 0; // Assume success

	uint32_t page_size = 512;

	// Read a page at a time
	while (byte_count) {
		uint32_t page_offset = (address & (page_size - 1)); // Starting offset within page
		uint32_t page_len = page_size - page_offset; // Up to one page

		if (byte_count < page_len)
			page_len = byte_count;

		rtn = flash_read(&FLASH_0, address, data, page_len);
		if (rtn)
			break;

		byte_count -= page_len;
		address += page_len;
		data += page_len;
	}

	if (rtn) {
		nk_printf("ERROR: Flash_read returned %d\n", rtn);
	}

	return rtn;
}
