// TM1637 7-segment display driver

// This is not I2C: bits are backwards and there is no address byte

#include <string.h>
#include "nkdriver_tm1637.h"

#define SEG_A (0x01)
#define SEG_B (0x02)
#define SEG_C (0x04)
#define SEG_D (0x08)
#define SEG_E (0x10)
#define SEG_F (0x20)
#define SEG_G (0x40)
#define SEG_DP (0x80)

// Convert hexadecimal to 7-segment

uint8_t font[16] =
{
    SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,
    SEG_B | SEG_C,
    SEG_A | SEG_B | SEG_G | SEG_E | SEG_D,
    SEG_A | SEG_B | SEG_G | SEG_C | SEG_D,
    SEG_F | SEG_G | SEG_B | SEG_C,
    SEG_A | SEG_F | SEG_G | SEG_C | SEG_D,
    SEG_A | SEG_F | SEG_G | SEG_E | SEG_D | SEG_C,
    SEG_A | SEG_B | SEG_C,
    SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G,
    SEG_A | SEG_B | SEG_G | SEG_F | SEG_C,
    SEG_A | SEG_B | SEG_F | SEG_G | SEG_F | SEG_C | SEG_E,
    SEG_F | SEG_E | SEG_D | SEG_C | SEG_G,
    SEG_A | SEG_F | SEG_E | SEG_D,
    SEG_G | SEG_E | SEG_C | SEG_D | SEG_B,
    SEG_A | SEG_F | SEG_G | SEG_E | SEG_D,
    SEG_A | SEG_F | SEG_G | SEG_E
};

// Some displays have large capacitors on the SDA / SCK lines,
// so rising edge is quite slow.

int nk_tm1637_bit_delay(const nk_tm1637_t *dev)
{
    int x;
    for (x = 0; x != 50; ++x)
        nk_pin_read(dev->dio);
    return 0;
}

int nk_tm1637_init(const nk_tm1637_t *dev)
{
    nk_pin_setmode(dev->clk, NK_PINMODE_INPUT_PULLUP);
    nk_pin_setmode(dev->dio, NK_PINMODE_INPUT_PULLUP);
    nk_pin_write(dev->clk, 0);
    nk_pin_write(dev->dio, 0);
    return 0;
}

int nk_tm1637_start(const nk_tm1637_t *dev)
{
    nk_pin_setmode(dev->dio, NK_PINMODE_OUTPUT);
    nk_tm1637_bit_delay(dev);
    nk_pin_setmode(dev->clk, NK_PINMODE_OUTPUT);
    nk_tm1637_bit_delay(dev);
    return 0;
}

int nk_tm1637_stop(const nk_tm1637_t *dev)
{
    nk_pin_setmode(dev->dio, NK_PINMODE_OUTPUT);
    nk_tm1637_bit_delay(dev);
    nk_pin_setmode(dev->clk, NK_PINMODE_INPUT_PULLUP);
    nk_tm1637_bit_delay(dev);
    nk_pin_setmode(dev->dio, NK_PINMODE_INPUT_PULLUP);
    nk_tm1637_bit_delay(dev);
    return 0;
}

int nk_tm1637_write_byte(const nk_tm1637_t *dev, uint8_t byte)
{
    int rtn;
    int x;
    for (x = 0; x != 8; ++x)
    {
        if (byte & 1)
            nk_pin_setmode(dev->dio, NK_PINMODE_INPUT_PULLUP);
        else
            nk_pin_setmode(dev->dio, NK_PINMODE_OUTPUT);
        nk_tm1637_bit_delay(dev);
        nk_pin_setmode(dev->clk, NK_PINMODE_INPUT_PULLUP);
        nk_tm1637_bit_delay(dev);
        nk_pin_setmode(dev->clk, NK_PINMODE_OUTPUT);
        nk_tm1637_bit_delay(dev);
        byte >>= 1;
    }
    // Check for ACK
    nk_pin_setmode(dev->clk, NK_PINMODE_INPUT_PULLUP);
    nk_tm1637_bit_delay(dev);
    rtn = nk_pin_read(dev->dio);
    nk_pin_setmode(dev->clk, NK_PINMODE_OUTPUT);
    nk_tm1637_bit_delay(dev);

    // Should be low for ACK..
    return rtn;
}

int nk_tm1637_write(const nk_tm1637_t *dev, uint8_t *data, int len)
{
    int rtn = 0;
    nk_tm1637_start(dev);
    while (len)
    {
        rtn |= nk_tm1637_write_byte(dev, *data);
        ++data;
        --len;
    }
    nk_tm1637_stop(dev);
    return rtn;
}

int nk_tm1637_display(const nk_tm1637_t *dev, uint8_t bright, const uint8_t *digits, int len)
{
    int x;
    uint8_t data[7];
    int rtn;
    data[0] = TM1637_MODE;
    rtn = nk_tm1637_write(dev, data, 1);
    data[0] = TM1637_ADDR;
    for (x = 0; x != len; ++x)
    {
        if (digits[x] >= 16)
            data[x + 1] = 0;
        else
            data[x + 1] = font[digits[x]];
        if (x == 5) break;
    }
    rtn |= nk_tm1637_write(dev, data, 1 + len);
    data[0] = TM1637_DISP + bright;
    rtn |= nk_tm1637_write(dev, data, 1);
    return rtn;
}
