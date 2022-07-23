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
#include "nksched.h"
#include "nkuart.h"

// Console UART Rx buffer

static unsigned char rx_buf[NK_UART_RXBUF_SIZE];
static uint32_t rx_buf_rd;
static volatile uint32_t rx_buf_wr;
static int tty_mode;

spinlock_t console_lock = SPIN_LOCK_UNLOCKED;

// Task to submit when rx data available
int waiting_rx_tid;
void (*waiting_rx_task)(void *data);
void *waiting_rx_task_data;

void nk_set_uart_callback(int tid, void (*func)(void *data), void *data)
{
        unsigned long irq_flag;
        nk_irq_lock(&console_lock, irq_flag);
	if (rx_buf_rd != rx_buf_wr) {
		// Data is available now
		nk_sched(tid, func, data, 0, "UART ISR");
		waiting_rx_task = 0;
	} else {
		waiting_rx_tid = tid;
		waiting_rx_task = func;
		waiting_rx_task_data = data;
	}
	nk_irq_unlock(&console_lock, irq_flag);
}

int nk_get_uart_mode()
{
	return tty_mode;
}

int nk_set_uart_mode(int new_mode)
{
	int old_mode = tty_mode;
	tty_mode = new_mode;
	return old_mode;
}

static void wait_for_space()
{
	while (reg_uart_clkdiv & 0x40000000);
}

void nk_putc(char ch)
{
        unsigned long irq_flag;
        nk_irq_lock(&console_lock, irq_flag);
	if (!tty_mode && ch == '\n') {
		wait_for_space();
		reg_uart_data = '\r';
	}
	wait_for_space();
	reg_uart_data = ch;
	nk_irq_unlock(&console_lock, irq_flag);
}

void nk_puts(const char *s)
{
        unsigned long irq_flag;
        nk_irq_lock(&console_lock, irq_flag);
	while (*s) {
		nk_putc(*s++);
	}
	nk_irq_unlock(&console_lock, irq_flag);
}

void nk_uart_write(const char *s, int len)
{
	unsigned long irq_flag;
	nk_irq_lock(&console_lock, irq_flag);
	while (len--) {
		nk_putc(*s++);
	}
	nk_irq_unlock(&console_lock, irq_flag);
}

// Transfer any available characters from UART FIFO to input buffer

static void rx_chars(void)
{
	uint32_t c;
	while ((c = reg_uart_data) != 0xFFFFFFFF) {
		if (rx_buf_wr - rx_buf_rd != sizeof(rx_buf)) {
			rx_buf[rx_buf_wr++ & (sizeof(rx_buf) - 1)] = (unsigned char)c;
		}
	}
}

// Interrupt if characters are available: but Apollo gives interrupt on 4 byte units, not single byte, so
// we still have to poll.

void picorv32_uart_irq()
{
	rx_chars();
	if (waiting_rx_task) {
		nk_sched(waiting_rx_tid, waiting_rx_task, waiting_rx_task_data, 0, "UART ISR");
		waiting_rx_task = 0;
	}
}

int nk_getc()
{
	int ch = -1;
        unsigned long irq_flag;
        nk_irq_lock(&console_lock, irq_flag);
        rx_chars();
	if (rx_buf_rd != rx_buf_wr) {
		ch = rx_buf[rx_buf_rd++ & (sizeof(rx_buf) - 1)];
	}
	nk_irq_unlock(&console_lock, irq_flag);
	return ch;
}

int nk_kbhit()
{
        unsigned long irq_flag;
        nk_irq_lock(&console_lock, irq_flag);
	if (rx_buf_rd != rx_buf_wr) {
		nk_irq_unlock(&console_lock, irq_flag);
		return 1;
	} else {
		nk_irq_unlock(&console_lock, irq_flag);
		return 0;
	}
}

int nk_uart_read(char *s, int len, nk_time_t timeout)
{
	int l = 0;
	int need_time = 1;
	nk_time_t old_time;
	nk_time_t clocks = timeout * NK_TIME_COUNTS_PER_USECOND;
	while (l != len) {
		int c = nk_getc();
		if (c != -1) {
			s[l++] = (char)c;
			need_time = 1;
		} else {
			if (need_time) {
				old_time = nk_get_time();
				need_time = 0;
			}
			if ((nk_get_time() - old_time) >= clocks)
				break;
		}
	}
	return l;
}

// UART configuration settings.

void nk_init_uart()
{
}
