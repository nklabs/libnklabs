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

//#include "hal_usart_async.h"
#include "hal_usart_sync.h"
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

volatile int tx_busy;

void nk_putc(char ch)
{
	struct io_descriptor *io;
	char cr = '\r';
	//usart_async_get_io_descriptor(&USART_0, &io);
	usart_sync_get_io_descriptor(&USART_0, &io);
	tx_busy = 1;
	if (!tty_mode && ch == '\n') {
		io_write(io, (const uint8_t *)&cr, 1);
	}
	io_write(io, (const uint8_t *)&ch, 1);
	//while (tx_busy);
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
	uint8_t ch;
	struct io_descriptor *io;
	usart_sync_get_io_descriptor(&USART_0, &io);
	if (1 == io_read(io, &ch, 1))
		return ch;
	else
		return -1;

#if 0
        struct io_descriptor *const io_descr = &USART_0.io;
        struct usart_async_descriptor *descr = CONTAINER_OF(io_descr, struct usart_async_descriptor, io);

        if (ringbuffer_num(&descr->rx) < 1) {
        	return -1;
        } else {
        	uint8_t ch;
		ringbuffer_get(&descr->rx, &ch);
		return ch;
        }
#endif
}

int nk_kbhit()
{
#if 0
        struct io_descriptor *const io_descr = &USART_0.io;
        struct usart_async_descriptor *descr = CONTAINER_OF(io_descr, struct usart_async_descriptor, io);

        if (ringbuffer_num(&descr->rx) < 1) {
        	return 0;
        } else {
        	return 1;
        }
#endif
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

#if 0
static void tx_cb(const struct usart_async_descriptor *const io_descr)
{
	tx_busy = 0;
}

static void rx_cb(const struct usart_async_descriptor *const io_descr)
{
	if (waiting_rx_task) {
		nk_sched(waiting_rx_tid, waiting_rx_task, waiting_rx_task_data, 0, "UART ISR");
		waiting_rx_task = 0;
	}
}
#endif

// UART configuration settings.

void nk_init_uart()
{
	struct io_descriptor *io;
	//usart_async_set_baud_rate(&USART_0, 115200);
        //usart_async_register_callback(&USART_0, USART_ASYNC_TXC_CB, tx_cb);
        //usart_async_register_callback(&USART_0, USART_ASYNC_RXC_CB, rx_cb);
	//usart_async_get_io_descriptor(&USART_0, &io);
	//usart_async_enable(&USART_0);
	usart_sync_get_io_descriptor(&USART_0, &io);
	//usart_sync_set_baud_rate(&USART_0, 115200);
	usart_sync_enable(&USART_0);

	io_write(io, "Hello, world!\r\n", 15);

        // First startup line
        nk_putc('\n');
        startup("Console UART\n");
}
