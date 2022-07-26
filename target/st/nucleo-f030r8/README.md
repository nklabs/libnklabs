Set up the Nucleo so that 8 MHz from ST-LINK (MCO output pin of ST-LINK MCU)
is used the HSE input clock.  This requires:

	SB55 Off
	SB54 On
	SB16 On
	SB50 On
	R35 and R37 removed

Boards marked with MB1136 C-02 or higher will be factory configured this
way.

PA5 is overloaded: it is connected to both the on-board LED and to Arduino
D13 (often used for SPI SCLK).  The software disabled the blinking LED
whenever Arduino SPI transactions are performed.

D0 and D1 are lost because PA2 and PA3 are used for the console UART.
