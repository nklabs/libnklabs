The design assumes that SB17 has been bridged, so that 8 MHz from ST-LINK
(MCO output pin of ST-LINK MCU) is used as the HSE input clock.

PA5 is overloaded: it is connected to both the on-board LED and to Arduino
D13 (often used for SPI SCLK).  The software disabled the blinking LED
whenever Arduino SPI transactions are performed.
