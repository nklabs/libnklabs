// Copyright 2021 NK Labs, LLC

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
static int rx_buf_rd;
static volatile int rx_buf_wr;
static int tty_mode;

// Task to submit when rx data available
static int waiting_rx_tid;
static void (*waiting_rx_task)(void *data);
static void *waiting_rx_task_data;

nk_spinlock_t console_lock = SPIN_LOCK_UNLOCKED;

void nk_set_uart_callback(int tid, void (*func)(void *data), void *data)
{
        nk_irq_flag_t irq_flag;
        irq_flag = nk_irq_lock(&console_lock);
	if (rx_buf_rd != rx_buf_wr) {
		// Data is available now
		nk_sched(tid, func, data, 0, "UART ISR");
		waiting_rx_task = 0;
	} else {
		// No data, wait for it to arrive...
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

// OK to call from interrupt level

void nk_putc(char ch)
{
        nk_irq_flag_t irq_flag;
        irq_flag = nk_irq_lock(&console_lock);
	if (!tty_mode && ch == '\n') {
		char cr = '\r';
#ifdef USART_ISR_TXE_TXFNF
		while (!(MAIN_CONSOLE_UART.Instance->ISR & USART_ISR_TXE_TXFNF));
		MAIN_CONSOLE_UART.Instance->TDR = (uint8_t)cr;
#else
#ifdef USART_ISR_TXE
		while (!(MAIN_CONSOLE_UART.Instance->ISR & USART_ISR_TXE));
		MAIN_CONSOLE_UART.Instance->TDR = (uint8_t)cr;
#else
		while (!(MAIN_CONSOLE_UART.Instance->SR & USART_SR_TXE));
		MAIN_CONSOLE_UART.Instance->DR = (uint8_t)cr;
#endif
#endif
		
	}
#ifdef USART_ISR_TXE_TXFNF
	while (!(MAIN_CONSOLE_UART.Instance->ISR & USART_ISR_TXE_TXFNF));
	MAIN_CONSOLE_UART.Instance->TDR = (uint8_t)ch;
#else
#ifdef USART_ISR_TXE
	while (!(MAIN_CONSOLE_UART.Instance->ISR & USART_ISR_TXE));
	MAIN_CONSOLE_UART.Instance->TDR = (uint8_t)ch;
#else
	while (!(MAIN_CONSOLE_UART.Instance->SR & USART_SR_TXE));
	MAIN_CONSOLE_UART.Instance->DR = (uint8_t)ch;
#endif
#endif
	nk_irq_unlock(&console_lock, irq_flag);
}

void nk_puts(const char *s)
{
        nk_irq_flag_t irq_flag;
        irq_flag = nk_irq_lock(&console_lock);
	while (*s) {
		nk_putc(*s++);
	}
	nk_irq_unlock(&console_lock, irq_flag);
}

void nk_uart_write(const char *s, int len)
{
	nk_irq_flag_t irq_flag;
	if (!tty_mode) {
		irq_flag = nk_irq_lock(&console_lock);
		while (len--) {
			nk_putc(*s++);
		}
		nk_irq_unlock(&console_lock, irq_flag);
	} else {
		irq_flag = nk_irq_lock(&console_lock);
		while (len--) {
			char c = *s++;
			HAL_UART_Transmit(&MAIN_CONSOLE_UART, (unsigned char *)&c, 1, 0xFFFF);
		}
		nk_irq_unlock(&console_lock, irq_flag);
	}
}

void nk_uart_irq_handler(void)
{
#ifdef USART_ISR_RXNE_RXFNE
	while (MAIN_CONSOLE_UART.Instance->ISR & USART_ISR_RXNE_RXFNE) // Character available?
#else
#ifdef USART_ISR_RXNE
	while (MAIN_CONSOLE_UART.Instance->ISR & USART_ISR_RXNE) // Character available?
#else
	while (MAIN_CONSOLE_UART.Instance->SR & USART_SR_RXNE)
#endif
#endif
	{
#ifdef USART_ISR_RXNE_RXFNE
		uint8_t ch = MAIN_CONSOLE_UART.Instance->RDR; // Read it
#else
#ifdef USART_ISR_RXNE
		uint8_t ch = MAIN_CONSOLE_UART.Instance->RDR; // Read it
#else
		uint8_t ch = MAIN_CONSOLE_UART.Instance->DR; // Read it
#endif
#endif

		// Append to buffer if we have space
		if (rx_buf_wr - rx_buf_rd != sizeof(rx_buf)) {
			rx_buf[rx_buf_wr++ & (sizeof(rx_buf) - 1)] = ch;
		}
	}
	// Wake up task
	if (waiting_rx_task) {
		nk_sched(waiting_rx_tid, waiting_rx_task, waiting_rx_task_data, 0, "UART ISR");
		waiting_rx_task = 0;
	}
}

// Nobody calls this from interrupt level, so no need for locks

int nk_getc()
{
	int ch = -1;
        // nk_irq_flag_t irq_flag;
        // irq_flag = nk_irq_lock(&console_lock);
	if (rx_buf_rd != rx_buf_wr) {
		ch = rx_buf[rx_buf_rd++ & (sizeof(rx_buf) - 1)];
	}
	// nk_irq_unlock(&console_lock, irq_flag);
	return ch;
}

int nk_kbhit()
{
        // nk_irq_flag_t irq_flag;
        // irq_flag = nk_irq_lock(&console_lock);
	if (rx_buf_rd != rx_buf_wr) {
		// nk_irq_unlock(&console_lock, irq_flag);
		return 1;
	} else {
		// nk_irq_unlock(&console_lock, irq_flag);
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

void nk_init_uart()
{
	// Only for lpuart
  	// HAL_UARTEx_EnableStopMode(&MAIN_CONSOLE_UART);
  	CLEAR_BIT(MAIN_CONSOLE_UART.Instance->CR1, USART_CR1_UE);
  	// Disable overflow checking
  	// UART must be disabled to change this bit
#ifdef USART_CR3_OVRDIS
  	SET_BIT(MAIN_CONSOLE_UART.Instance->CR3, USART_CR3_OVRDIS);
#endif

  	SET_BIT(MAIN_CONSOLE_UART.Instance->CR1, USART_CR1_UE | USART_CR1_TE | USART_CR1_RE);

  	// Enable Rx interrupts
#ifdef USART_CR1_RXNEIE_RXFNEIE
	// FIFO
  	SET_BIT(MAIN_CONSOLE_UART.Instance->CR1, USART_CR1_RXNEIE_RXFNEIE);
#else
  	SET_BIT(MAIN_CONSOLE_UART.Instance->CR1, USART_CR1_RXNEIE);
#endif
}
