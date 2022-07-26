Set up the Nucleo so that 8 MHz from ST-LINK (MCO output pin of ST-LINK MCU)
is used the HSE input clock.  This requires:

	Solder jumper to SB17

PA5 is overloaded: it is connected to both the on-board LED and to Arduino
D13 (often used for SPI SCLK).  The software disabled the blinking LED
whenever Arduino SPI transactions are performed.
