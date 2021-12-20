The design assumes that you have installed solder bridge SB17 so that the 8
MHz clock from ST-LIMK MCU is applied to PA0.  This means Arduino pin A0 is
lost.

By default SB16 and SB18 are installed.  This is to allow I2C to work on
Arduino A4 and A5:

SB16 installed (default) (I2C):

PB6, PA6, A5 and D5 all connected.  PB6 set up for SDA, PA6 left floating.

SB16 removed (Analog):

PB6 connects to D5.  PA6 connects to A5.

SB18 installed (default) (I2C):

PB7, PA5, D4 and A4 all connected.  PB7 set up for SCL, PA5 left floating.

SB18 removed (Analog):

PB7 connects to D4.  PA5 connects to A4.

Because of above, Arduino D4 and D5 are lost.

Arduino D7 and D8 are lost because PF0 and PF1 are connected to the 32 KHz
crystal.

PB3 is overloaded: it is connected to both the on-board LED and to Arduino
D13 (often used for SPI SCLK).  The software disabled the blinking LED
whenever Arduino SPI transactions are performed.

This board has Arduino Nano A6.

Aruino Nano A7 is lost because it is used for the console UART connected to
the ST-LINK.
