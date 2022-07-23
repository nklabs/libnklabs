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


#include "nkarch.h"
#include "nkuart.h"
#include "nksched.h"
#include "nkcli.h"
#include "nkdbase.h"
#include "database.h"
#include "fpga.h"

// All interrupts come here

// When irq pin in pulsed, it is latched (as long as corresponding LATCHED bit is high, which is the default)

// When this handler is called, the 'eoi' output has which interrupts were present to cause the interrupt
// Also those pending interrupts are cleared.

// 'eoi' output is cleared when we return from this handler.

extern void picorv32_uart_irq();

uint32_t *irq(uint32_t *regs, uint32_t irqs)
{
	if (irqs & TIMER_IRQ)
		picorv32_timer_irq();

	if (irqs & UART_IRQ)
		picorv32_uart_irq();

//	if (irqs & FPGA_IRQ)
//		fpga_irq();

	return regs;
}

int main()
{
	reg_leds = 0xA;
	nk_init_uart();
	nk_init_sched();
	database_init();
	nk_init_fpga();
	nk_init_cli();
	nk_printf("End-point\n");
	nk_sched_loop();
	return 0;
}
