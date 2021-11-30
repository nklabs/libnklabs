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

spinlock_t console_lock = SPIN_LOCK_UNLOCKED;

void nk_set_uart_callback(int tid, void (*func)(void *data), void *data)
{
        unsigned long irq_flag;
        nk_irq_lock(&console_lock, irq_flag);
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
        unsigned long irq_flag;
        nk_irq_lock(&console_lock, irq_flag);
	if (!tty_mode && ch == '\n') {
		char cr = '\r';
		while (!(console_uart.Instance->ISR & USART_ISR_TXE_TXFNF));
		console_uart.Instance->TDR = (uint8_t)cr;
		
	}
	while (!(console_uart.Instance->ISR & USART_ISR_TXE_TXFNF));
	console_uart.Instance->TDR = (uint8_t)ch;
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
			HAL_UART_Transmit(&console_uart, (unsigned char *)&c, 1, 0xFFFF);
		}
		nk_irq_unlock(&console_lock, irq_flag);
	}
}

void nk_uart_irq_handler(void)
{
	while (console_uart.Instance->ISR & USART_ISR_RXNE_RXFNE) // Character available?
	{
		uint8_t ch = console_uart.Instance->RDR; // Read it

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
        // unsigned long irq_flag;
        // nk_irq_lock(&console_lock, irq_flag);
	if (rx_buf_rd != rx_buf_wr) {
		ch = rx_buf[rx_buf_rd++ & (sizeof(rx_buf) - 1)];
	}
	// nk_irq_unlock(&console_lock, irq_flag);
	return ch;
}

int nk_kbhit()
{
        // unsigned long irq_flag;
        // nk_irq_lock(&console_lock, irq_flag);
	if (rx_buf_rd != rx_buf_wr) {
		// nk_irq_unlock(&console_lock, irq_flag);
		return 1;
	} else {
		// nk_irq_unlock(&console_lock, irq_flag);
		return 0;
	}
}

int nk_uart_read(char *s, int len, nk_time_t timeout)
{
	int l = 0;
	int need_time = 1;
	nk_time_t old_time;
	timeout = (timeout / 1000); // Convert to milliseconds
	nk_time_t clocks = timeout * NK_TIME_COUNTS_PER_SECOND / 1000;
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
	//HAL_UART_Receive_IT(&console_uart, &tty_ch, 1);
	// Only for lpuart
  	// HAL_UARTEx_EnableStopMode(&console_uart);
  	CLEAR_BIT(console_uart.Instance->CR1, USART_CR1_UE);
  	// Disable overflow checking
  	// UART must be disabled to change this bit
  	SET_BIT(console_uart.Instance->CR3, USART_CR3_OVRDIS);
  	SET_BIT(console_uart.Instance->CR1, USART_CR1_UE | USART_CR1_TE | USART_CR1_RE);

  	// Enable Rx interrupts
  	SET_BIT(console_uart.Instance->CR1, USART_CR1_RXNEIE_RXFNEIE);
}
