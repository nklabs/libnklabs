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

#define CONSOLE_UART XPAR_XUARTPS_0_DEVICE_ID
#define CONSOLE_UART_IRQ_ID XPAR_XUARTPS_0_INTR

XUartPs console_uart;

// Console UART Rx buffer

static unsigned char rx_buf[NK_UART_RXBUF_SIZE];
static uint32_t rx_buf_rd;
static volatile uint32_t rx_buf_wr;
static int tty_mode;

nk_spinlock_t console_lock = SPIN_LOCK_UNLOCKED;

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

void nk_putc(char ch)
{
        unsigned long irq_flag;
        nk_irq_lock(&console_lock, irq_flag);
	if (!tty_mode && ch == '\n') {
		/* am_hal_uart_char_transmit_polled(CONSOLE_UART, '\r'); */
		/* XUartPs_SendByte(STDOUT_BASEADDR, '\r'); */
		XUartPs_SendByte(console_uart.Config.BaseAddress, '\r');
	}
	/* am_hal_uart_char_transmit_polled(CONSOLE_UART, ch); */
	XUartPs_SendByte(console_uart.Config.BaseAddress, ch);
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
	if (!tty_mode) {
		nk_irq_lock(&console_lock, irq_flag);
		while (len--) {
			nk_putc(*s++);
		}
		nk_irq_unlock(&console_lock, irq_flag);
	} else {
		nk_irq_lock(&console_lock, irq_flag);
		while (len--) {
			char c = *s++;
			XUartPs_SendByte(console_uart.Config.BaseAddress, c);
		}
		nk_irq_unlock(&console_lock, irq_flag);
	}
}

// Transfer any available characters from UART FIFO to input buffer

static void rx_chars(void)
{
	while (XUartPs_IsReceiveData(console_uart.Config.BaseAddress)) {
		char ch;
		ch = XUartPs_RecvByte(console_uart.Config.BaseAddress);
		if (rx_buf_wr - rx_buf_rd != sizeof(rx_buf)) {
			rx_buf[rx_buf_wr++ & (sizeof(rx_buf) - 1)] = ch;
		}
	}
}

// Interrupt if characters are available: but Apollo gives interrupt on 4 byte units, not single byte, so
// we still have to poll.

static void console_isr(void *data)
{
	// nk_printf("Foo!\n");

	uint32_t status;

	status = XUartPs_ReadReg(console_uart.Config.BaseAddress, XUARTPS_IMR_OFFSET);

        status &= XUartPs_ReadReg(console_uart.Config.BaseAddress, XUARTPS_ISR_OFFSET);

/*	if  (status & (XUARTPS_IXR_RXOVR | XUARTPS_IXR_RXEMPTY | XUARTPS_IXR_RXFULL | XUARTPS_IXR_TOUT | XUARTPS_IXR_OVER)) */ {
		rx_chars();
		if (waiting_rx_task) {
			nk_sched(waiting_rx_tid, waiting_rx_task, waiting_rx_task_data, 0, "UART ISR");
			waiting_rx_task = 0;
		}
	}

	XUartPs_WriteReg(console_uart.Config.BaseAddress, XUARTPS_ISR_OFFSET, status);
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

int nk_uart_read(char *s, int len, uint32_t timeout)
{
	int l = 0;
	int need_time = 1;
	nk_time_t old_time;
	nk_time_t clocks = nk_convert_delay(timeout);
	while (l != len) {
		int c = nk_getc();
		if (c != -1) {
			s[l++] = c;
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
	XUartPs_Config *config = XUartPs_LookupConfig(CONSOLE_UART);

	XUartPs_CfgInitialize(&console_uart, config, config->BaseAddress);

	// The above sets the baud rate to 115200, overriding the hardware default
	// XUartPs_SetBaudRate(&console_uart, 921600);

	XScuGic_Connect(&interrupt_controller, CONSOLE_UART_IRQ_ID, (Xil_ExceptionHandler)console_isr, (void *)&console_uart);
	XScuGic_Enable(&interrupt_controller, CONSOLE_UART_IRQ_ID);

	XUartPs_SetInterruptMask(&console_uart,
		XUARTPS_IXR_TOUT |
		XUARTPS_IXR_RXFULL |
		XUARTPS_IXR_RXOVR |
		XUARTPS_IXR_OVER |
		XUARTPS_IXR_RXEMPTY);

	XUartPs_SetOperMode(&console_uart, XUARTPS_OPER_MODE_NORMAL);

	XUartPs_SetRecvTimeout(&console_uart, 2);
	XUartPs_SetFifoThreshold(&console_uart, 8);

	nk_printf("interrupt mask = %x\n", XUartPs_ReadReg(console_uart.Config.BaseAddress, XUARTPS_IMR_OFFSET));

        // First startup line
        nk_putc('\n');
        nk_startup_message("Console UART\n");
}
