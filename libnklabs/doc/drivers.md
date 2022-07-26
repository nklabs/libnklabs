# Drivers for common ICs

Most of the devices referenced here are widely available on breakout boards
designed around the Arduino and Raspberry-PI ecosystems (look on eBay,
Adafruit, Amazon, etc.).

## SPI-Flash ICs

* Most SPI-Flash ICs are supported with the nkspiflash driver

## I2C-EEPROM ICs

* Most I2C-EEPROM ICs are supported with the nki2c_eeprom driver

## Real time clocks

* [DS1307 64 x 8, Serial, I²C Real-Time Clock](https://www.maximintegrated.com/en/products/analog/real-time-clocks/DS1307.html)

DS1307 is 5V only, and appears on older Adafruit Data Logging Shields.

* [DS3231 Extremely Accurate I²C-Integrated RTC/TCXO/Crystal](https://www.maximintegrated.com/en/products/analog/real-time-clocks/DS3231.html)

* [PCF8523 100 NA Real-Time Clock/Calendar with Battery Backup](https://www.nxp.com/products/peripherals-and-logic/signal-chain/real-time-clocks/rtcs-with-ic-bus/100-na-real-time-clock-calendar-with-battery-backup:PCF8523)

PCF8523 appears on newer Adafruit Data Logging Shields.

* [RV-8803 Real-Time Clock Module with I2C-Bus / Micro Crystal Switzerland](https://www.microcrystal.com/fileadmin/Media/Products/RTC/App.Manual/RV-8803-C7_App-Manual.pdf)

## I2C Bus Switch

* [TCA9548A 8-channel 1.65- to 5.5-V I2C/SMBus switch with reset & voltage translation](https://www.ti.com/product/TCA9548A)

## GPIO expanders

* [MCP23017 16-Bit I2C I/O Expander with Serial Interface](https://www.microchip.com/en-us/product/mcp23017)
* [PCF8574 Remote 8-Bit I/O Expander for I²C‑Bus with Interrupt](https://www.nxp.com/products/interfaces/ic-spi-serial-interface-devices/ic-general-purpose-i-o/remote-8-bit-i-o-expander-for-icbus-with-interrupt:PCF8574_74A)
* [PCF8575 Remote 16-Bit I/O Expander for I²C‑Bus with Interrupt](https://www.nxp.com/products/interfaces/ic-spi-serial-interface-devices/ic-general-purpose-i-o/remote-16-bit-i-o-expander-for-i178c-bus:PCF8575)

## Temperature sensors

* [LM75A Digital Temperature Sensor and Thermal Watchdog](https://www.nxp.com/products/sensors/ic-digital-temperature-sensors/digital-temperature-sensor-and-thermal-watchdog:LM75A)
* [MCP9808 ±0.5°C Max. Accuracy Digital Temp. Sensor](https://www.microchip.com/en-us/product/MCP9808)

## Thermocouple

* [MAX6675 Cold-Junction-Compensated K-Thermocouple-to-Digital Converter (0°C to +1024°C)](https://www.maximintegrated.com/en/products/interface/signal-integrity/MAX6675.html)

## DAC

* [MCP4725 12-Bit Single Output DAC w/EEPROM and I2C™](https://www.microchip.com/en-us/product/MCP4725)

## ADC

* [ADS1115 16-bit, 860-SPS, 4-channel, delta-sigma ADC with PGA, oscillator, VREF, comparator and I2C](https://www.ti.com/product/ADS1115)

## Power / Curent monitor

* [INA219 26V, 12-bit, i2c output current/voltage/power monitor](https://www.ti.com/product/INA219)

## PWM controllers

* [PCA9633 4-Bit Fm+ I²C-Bus LED Driver](https://www.nxp.com/products/power-management/lighting-driver-and-controller-ics/ic-led-controllers/4-bit-fm-plus-ic-bus-led-driver:PCA9633)

PCA9633 is for LEDs only- some LCD1602 modules use this chip for the backlight.

* [PCA9685 16-Channel, 12-Bit PWM Fm+ I²C-Bus LED Controller](https://www.nxp.com/products/power-management/lighting-driver-and-controller-ics/ic-led-controllers/16-channel-12-bit-pwm-fm-plus-ic-bus-led-controller:PCA9685)

PCA9685 is good for both LEDs and RC servos.

## LED display drivers

* [HT16K33 I2C 16x8 LED drive with keyscan](https://cdn-shop.adafruit.com/datasheets/ht16K33v110.pdf)

HT16K33 is use on [Adafruit 14-segment LED Alphanumeric Backpack](https://www.adafruit.com/product/1910)

* [MAX7219/MAX7221 Serially Interfaced, 8-Digit LED Display Drivers](https://datasheets.maximintegrated.com/en/ds/MAX7219-MAX7221.pdf)

MAX7219 is found on a number of 8-digit LED display boards.  MAX7219 is
serially cascadable.

* [TM1637 7-segment LED display driver](https://www.mcielectronics.cl/website_MCI/static/documents/Datasheet_TM1637.pdf)

TM1637 uses a two-wire serial interface, but is not I2C (dedicated GPIOs
must be used).  It can drive up to six common cathode LED digits (up to 8
segments each).  It can also scan a 16-key keypad.

TM1637 is a 5V part, but uses open-drain drivers.  It is compatible with
3.3V MCUs with 5V tolerant inputs as long a pull-ups to 5V are used.

* [TM1638 Special Circuit for LED drive control](https://www.futurashop.it/image/catalog/data/Download/TM1638_V1.3_EN.pdf)

TM1638 uses a three-wire serial interface, but is not SPI (MISO and MOSI are
on a shared pin).  It can drive up to eight common cathode LED digits (up to
10 segments each).  It can also scan a 24-key keypad.

TM1638 is a 5V part, but uses open-drain drivers.  It is compatible with
3.3V MCUs with 5V tolerant inputs as long a pull-ups to 5V are used.

## LCD display drivers

[HD44780](https://www.sparkfun.com/datasheets/LCD/HD44780.pdf) (and clones such as [ST7066U](https://www.newhavendisplay.com/app_notes/ST7066U.pdf)) based LCD text displays are supported:

HD44780 natively uses a parallel interface, for example on this [OSEPP LCD DISLAY / KEYPAD Shield](https://www.osepp.com/electronic-modules/shields/45-16-2-lcd-display-keypad-shield).

But it can also be used through an I2C GPIO expander like this: [I2C Adapter](http://www.handsontec.com/dataspecs/module/I2C_1602_LCD.pdf). 
Here are some LCD display modules based on these chips:

* "1602A" (2 line, 16 column LCD display with backlight) connected to a PCF8574-based I2C adapter [TC1602A-01T](https://cdn-shop.adafruit.com/datasheets/TC1602A-01T.pdf)
* "2004A" (4 line, 20 column LCD display with backlight) connected to a PCF8574-based I2C adapter [TC2004A-01](https://cdn-shop.adafruit.com/datasheets/TC2004A-01.pdf)

Some LCD display ICs have an I2C interface built-in:

* [AIP31068 40 SEG / 16 COM Driver & Controller for Dot Matrix LCD](https://www.newhavendisplay.com/resources_dataFiles/datasheets/LCDs/AiP31068.pdf)

For example, as found on this display module: [Waveshare LCD1602RGB](https://www.waveshare.com/lcd1602-rgb-module.htm)

# On deck..

* [SI5351A PLL clock generator](https://cdn-shop.adafruit.com/datasheets/Si5351.pdf)

* [AD9954 400 MSPS, 14-Bit, 1.8 V CMOS, Direct Digital Synthesizer](https://www.analog.com/en/products/ad9954.html)
* [AD9958 2-Channel, 500 MSPS DDS with 10-Bit DACs](https://www.analog.com/en/products/ad9958.html)

* [DS1391 Low-Voltage SPI/3-Wire RTCs with Trickle Charger](https://www.maximintegrated.com/en/products/analog/real-time-clocks/DS1391.html)

* [SSD1306 128 x 64 Dot Matrix OLED/PLED Segment/Common Driver with Controller](https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf)

* [MCP23S08 8-Bit I/O Expander with SPI Interface](https://ww1.microchip.com/downloads/en/DeviceDoc/MCP23008-MCP23S08-Data-Sheet-20001919F.pdf)

* [MAX11666 500ksps, Low-Power, Serial 12-/10-/8-Bit ADCs](https://www.maximintegrated.com/en/products/analog/data-converters/analog-to-digital-converters/MAX11666.html)
* [MCP4922 12-Bit Dual Output DAC with SPI](https://www.microchip.com/en-us/product/MCP4922)
* [AD7276 3 MSPS, 12-Bit ADC](https://www.analog.com/en/products/ad7276.html)
* [AD5443 12-Bit High Bandwidth Multiplying DAC's with SPI](https://www.analog.com/en/products/ad5443.html)
* [MAX5590 Buffered, Fast-Settling, Octal, 12/10/8-Bit, Voltage-Output DACs](https://www.maximintegrated.com/en/products/analog/data-converters/digital-to-analog-converters/MAX5590.html)

* [BMI055 IMU combining accelerometer and gyroscope](https://www.bosch-sensortec.com/products/motion-sensors/imus/bmi055/)

* [AD5113 Single Channel, 64-Position, Up/Down, ±8 % Resistor Tolerance, Nonvolatile Digital Potentiometer](https://www.analog.com/en/products/ad5113.html)
* [MCP4561 8-Bit Single Digital POT with NVM and I2C™](https://www.microchip.com/en-us/product/MCP4561)
