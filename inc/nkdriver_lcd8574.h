#ifndef _Inkdriver_lcd8574
#define _Inkdriver_lcd8574

#include "nkdriver_pcf8574.h"
#include "nkdriver_hd44780.h"

// An HD44780-style LCD display connected to a PCF8574 I2C GPIO expander

typedef struct
{
    const nk_i2c_device_t *pcf8574; // PCF8574 device
    uint8_t regval; // Latest register write value
} nk_lcd8574_t;

#define LCD8574_RS 0x01
#define LCD8574_RW 0x02
#define LCD8574_E 0x04
#define LCD8574_BACKLIGHT 0x08
#define LCD8574_D4 0x10
#define LCD8574_D5 0x20
#define LCD8574_D6 0x40
#define LCD8574_D7 0x80

#define LCD8574_MASK 0xF0

int nk_lcd8574_init(nk_lcd8574_t *dev, bool backlight, bool f_bit, bool n_bit);

int nk_lcd8574_backlight(nk_lcd8574_t *dev, bool on);

int nk_lcd8574_send_command(nk_lcd8574_t *dev, uint8_t cmd);

int nk_lcd8574_send_data(nk_lcd8574_t *dev, uint8_t *data, int len);

#endif
