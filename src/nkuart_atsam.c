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

// sem_down means take
// sem_up means give

#include "nkarch.h"
#include "nksched.h"
#include "nkprintf.h"
#include "nkuart.h"

#include "hal_usart_async.h"
#include <hal_atomic.h>
#include <utils.h>

static int tty_mode;

// Task to submit when rx data available
int waiting_rx_tid;
void (*waiting_rx_task)(void *data);
void *waiting_rx_task_data;

void nk_set_uart_callback(int tid, void (*func)(void *data), void *data)
{
        unsigned long irq_flag;
        nk_irq_lock(&console_lock, irq_flag);
        if (nk_kbhit()) {
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

void nk_putc(char ch)
{
	struct io_descriptor *io_descr;
	usart_async_get_io_descriptor(&USART_0, &io_descr);
	struct usart_async_descriptor *descr = CONTAINER_OF(io_descr, struct usart_async_descriptor, io);

	if (!tty_mode && ch == '\n') {
		_usart_async_write_byte(&descr->device, '\r');
                while (!hri_sercomusart_get_interrupt_DRE_bit(descr->device.hw));
	}
	_usart_async_write_byte(&descr->device, ch);
	while (!hri_sercomusart_get_interrupt_DRE_bit(descr->device.hw));
}


void nk_puts(const char *s)
{
	while (*s) {
		nk_putc(*s++);
	}
}

void nk_uart_write(const char *s, int len)
{
	while (len--) {
		nk_putc(*s++);
	}
}

int nk_getc()
{
        struct io_descriptor *const io_descr;
	usart_async_get_io_descriptor(&USART_0, &io_descr);
        struct usart_async_descriptor *descr = CONTAINER_OF(io_descr, struct usart_async_descriptor, io);

        if (ringbuffer_num(&descr->rx) < 1) {
        	return -1;
        } else {
        	uint8_t ch;
		ringbuffer_get(&descr->rx, &ch);
		return ch;
        }
}

int nk_kbhit()
{
        struct io_descriptor *const io_descr;
	usart_async_get_io_descriptor(&USART_0, &io_descr);

        struct usart_async_descriptor *descr = CONTAINER_OF(io_descr, struct usart_async_descriptor, io);

        if (ringbuffer_num(&descr->rx) < 1) {
        	return 0;
        } else {
        	return 1;
        }
	return 1;
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

static void rx_cb(const struct usart_async_descriptor *const io_descr)
{
	if (waiting_rx_task) {
		nk_sched(waiting_rx_tid, waiting_rx_task, waiting_rx_task_data, 0, "UART ISR");
		waiting_rx_task = 0;
	}
}

// UART configuration settings.

void nk_init_uart()
{
	struct io_descriptor *io;
	int x;
	//usart_async_set_baud_rate(&USART_0, 115200);
        usart_async_register_callback(&USART_0, USART_ASYNC_RXC_CB, rx_cb);

	usart_async_enable(&USART_0);
	usart_async_get_io_descriptor(&USART_0, &io);

        // First startup line
        nk_putc('\n');
        startup("Console UART\n");
}
