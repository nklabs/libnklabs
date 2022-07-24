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
#include "nkarch_riscv.h"

// Scheduler timer- using PICORV32 timer

// Convert delay in milliseconds to number of scheduler timer clock ticks
nk_time_t nk_convert_delay(uint32_t delay)
{
	return delay * (NK_TIME_COUNTS_PER_SECOND / 1000);
}

void picorv32_timer_irq()
{
	// Nothing to do, system should wake up
}

void nk_init_sched_timer()
{
	// Nothing to do
}

// Generate timer interrupt to wake up system

void nk_sched_wakeup(nk_time_t when)
{
	uint32_t junk;
	uint32_t delay;

	nk_time_t now = nk_get_time();
	if (when <= now)
		delay = 1;
	else
		delay = when - now;

	picorv32_timer(junk, delay);
}

// Get current time

nk_time_t nk_get_time()
{
	return reg_wallclock;
}

// Busy loop delay
// Also use sched timer...

void nk_udelay(unsigned long usec)
{
	// Generic implementation
	nk_time_t old = nk_get_time();
	nk_time_t clocks = usec * (NK_TIME_COUNTS_PER_SECOND / 1000000);
	while ((nk_get_time() - old) < clocks);
}

// SPI-transfer

// The code is copied to RAM (an area on the stack) and executed from there.

extern uint32_t flashio_worker_begin;
extern uint32_t flashio_worker_end;

static void flashio(uint8_t *data, uint32_t len, uint8_t wrencmd)
{
	uint32_t flashio_func[&flashio_worker_end - &flashio_worker_begin];
	unsigned long irq_flag;

	// Copy help function to RAM
	uint32_t *src_ptr = &flashio_worker_begin;
	uint32_t *dst_ptr = flashio_func;

	while (src_ptr != &flashio_worker_end)
		*(dst_ptr++) = *(src_ptr++);

#ifdef junk
	uint32_t x;
	nk_printf("flashio out: ");
	for (x = 0; x != len; ++x)
		nk_printf(" %2.2x", data[x]);
	nk_printf("\n");
#endif

	nk_irq_lock(&sched_lock, irq_flag);

	((void(*)(uint8_t*, uint32_t, uint32_t))flashio_func)(data, len, wrencmd);

	nk_irq_unlock(&sched_lock, irq_flag);

#ifdef junk
	nk_printf("flashio in: ");
	for (x = 0; x != len; ++x)
		nk_printf(" %2.2x", data[x]);
	nk_printf("\n");
#endif
}

int nk_init_mcuflash()
{
	return 0;
}

int nk_mcuflash_erase(const void *info, uint32_t address, uint32_t byte_count)
{
	(void)info;
	// As long as we're not done..
	while (byte_count) {
		// Try to use largest erase that works..
		if ((address & 65535) == 0 && byte_count >= 65536) {
			uint8_t xfer[4];
			xfer[0] = 0xd8;
			xfer[1] = (uint8_t)(address >> 16);
			xfer[2] = (uint8_t)(address >> 8);
			xfer[3] = (uint8_t)(address);
			flashio(xfer, sizeof(xfer), 0x06);
			address += 65536;
			byte_count -= 65536;
		} else if ((address & 32767) == 0 && byte_count >= 32768) {
			uint8_t xfer[4];
			xfer[0] = 0x52;
			xfer[1] = (uint8_t)(address >> 16);
			xfer[2] = (uint8_t)(address >> 8);
			xfer[3] = (uint8_t)(address);
			flashio(xfer, sizeof(xfer), 0x06);
			address += 32768;
			byte_count -= 32768;
		} else if ((address & 4095) == 0 && byte_count >= 4096) {
			uint8_t xfer[4];
			xfer[0] = 0x20;
			xfer[1] = (uint8_t)(address >> 16);
			xfer[2] = (uint8_t)(address >> 8);
			xfer[3] = (uint8_t)(address);
			flashio(xfer, sizeof(xfer), 0x06);
			address += 4096;
			byte_count -= 4096;
		} else {
			// None work? Fail..
			break;
		}
	}

	if (byte_count) {
		return -1;
	}
	return 0;
}

int nk_mcuflash_write(const void *info, uint32_t address, const uint8_t *data, uint32_t byte_count)
{
	uint8_t xfer[256+4];
	int rtn = 0; // Assume success
	(void)info;

	uint32_t page_size = 256;

	// Write a page at a time
	while (byte_count) {
		uint32_t page_offset = (address & (page_size - 1)); // Starting offset within page
		uint32_t page_len = page_size - page_offset; // Up to one page

		if (byte_count < page_len)
			page_len = byte_count;

		xfer[0] = 0x02;
		xfer[1] = (uint8_t)(address >> 16);
		xfer[2] = (uint8_t)(address >> 8);
		xfer[3] = (uint8_t)(address);
		memcpy(xfer + 4, data, page_len);
		flashio(xfer, page_len + 4, 0x06);

		byte_count -= page_len;
		address += page_len;
		data += page_len;
	}

	return rtn;
}

int nk_mcuflash_read(const void *info, uint32_t address, uint8_t *data, uint32_t byte_count)
{
	uint8_t xfer[256+4];
	int rtn = 0; // Assume success

	uint32_t page_size = 256;

	(void)info;

	// Read a page at a time
	while (byte_count) {
		uint32_t page_offset = (address & (page_size - 1)); // Starting offset within page
		uint32_t page_len = page_size - page_offset; // Up to one page

		if (byte_count < page_len)
			page_len = byte_count;

		xfer[0] = 0x03;
		xfer[1] = (uint8_t)(address >> 16);
		xfer[2] = (uint8_t)(address >> 8);
		xfer[3] = (uint8_t)(address);
		flashio(xfer, page_len + 4, 0x00);
		memcpy(data, xfer + 4, page_len);

		byte_count -= page_len;
		address += page_len;
		data += page_len;
	}

	return rtn;
}

size_t strlen(const char *s)
{
	const char *t = s;
	while (*t)
		++t;
	return (size_t)(t - s);
}

int strcmp(const char *a, const char * b)
{
	while (*a && *b && *a == *b) {
		++a;
		++b;
	}
	if (*a > *b)
		return 1;
	else if (*a < *b)
		return -1;
	else
		return 0;
}

char *strcpy(char *d, const char *s)
{
	char *t = d;
	while ((*t++ = *s++));
	return d;
}

char *stpcpy(char *d, const char *s)
{
	while ((*d = *s)) {
		++d;
		++s;
	}
	return d;
}

void *memset(void *d, int c, size_t n)
{
	char *t = d;
	while (n--) {
		*t++ = (char)c;
	}
	return d;
}

void *memmove(void *d, const void *s, size_t n)
{
	char *t = d;
	const char *u = s;

	if (d < s) {
		while (n--) {
			*t++ = *u++;
		}
	} else if (d > s) {
		t += n;
		u += n;
		while (n--) {
			*--t = *--u;
		}
	}

	return d;
}

void *memcpy(void *d, const void *s, size_t n)
{
	char *t = d;
	const char *u = s;

	while (n--) {
		*t++ = *u++;
	}

	return d;
}

int memcmp(const void *d, const void *s, size_t n)
{
	const unsigned char *t = d;
	const unsigned char *u = s;
	while (n--) {
		if (*t > *u) return 1;
		if (*t < *u) return -1;
		++t; ++u;
	}
	return 0;
}

int atoi(const char *s)
{
	int val = 0;
	while (*s >= '0' && *s <= '9')
	{
		val = val * 10 + *s++ - '0';
	}
	return val;
}
