// TM1638 7-segment display driver

// This is not I2C: bits are backwards and there is no address byte

#include <string.h>
#include "nkdriver_tm1638.h"

#define SEG_A (0x01)
#define SEG_B (0x02)
#define SEG_C (0x04)
#define SEG_D (0x08)
#define SEG_E (0x10)
#define SEG_F (0x20)
#define SEG_G (0x40)
#define SEG_DP (0x80)

// Convert hexadecimal to 7-segment

uint8_t tm1638_font[17] =
{
    SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F, // 0
    SEG_B | SEG_C, // 1
    SEG_A | SEG_B | SEG_G | SEG_E | SEG_D, // 2
    SEG_A | SEG_B | SEG_G | SEG_C | SEG_D, // 3
    SEG_F | SEG_G | SEG_B | SEG_C, // 4
    SEG_A | SEG_F | SEG_G | SEG_C | SEG_D, // 5
    SEG_A | SEG_F | SEG_G | SEG_E | SEG_D | SEG_C, // 6
    SEG_A | SEG_B | SEG_C, // 7
    SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G, // 8
    SEG_A | SEG_B | SEG_G | SEG_F | SEG_C, // 9
    SEG_A | SEG_B | SEG_F | SEG_G | SEG_F | SEG_C | SEG_E, // A
    SEG_F | SEG_E | SEG_D | SEG_C | SEG_G, // b
    SEG_A | SEG_F | SEG_E | SEG_D, // C
    SEG_G | SEG_E | SEG_C | SEG_D | SEG_B, // d
    SEG_A | SEG_F | SEG_G | SEG_E | SEG_D, // E
    SEG_A | SEG_F | SEG_G | SEG_E, // F
    SEG_G // -
};

// Some displays have large capacitors on the SDA / SCK lines,
// so rising edge is quite slow.

int nk_tm1638_init(const nk_tm1638_t *dev)
{
    nk_pin_write(dev->clk, 1);
    nk_pin_write(dev->dio, 1);
    nk_pin_write(dev->stb, 1);
    nk_pin_setmode(dev->clk, NK_PINMODE_INPUT_PULLUP);
    nk_pin_setmode(dev->dio, NK_PINMODE_INPUT_PULLUP);
    nk_pin_setmode(dev->stb, NK_PINMODE_INPUT_PULLUP);
    return 0;
}

int nk_tm1638_start(const nk_tm1638_t *dev)
{
    nk_pin_setmode(dev->stb, NK_PINMODE_OUTPUT);
    nk_pin_write(dev->stb, 0);
    nk_udelay(dev->tfall + 1);
    return 0;
}

int nk_tm1638_stop(const nk_tm1638_t *dev)
{
    nk_pin_setmode(dev->stb, NK_PINMODE_INPUT_PULLUP);
    nk_udelay(dev->trise + 1);
    return 0;
}

int nk_tm1638_write_byte(const nk_tm1638_t *dev, uint8_t byte)
{
    int x;
    int prev = 0;
    for (x = 0; x != 8; ++x)
    {
        if (byte & 1)
        {
            nk_pin_setmode(dev->dio, NK_PINMODE_INPUT_PULLUP);
            if (prev)
                nk_udelay(1); // Previous bit was high, so save time
            else
                nk_udelay(dev->trise + 1);
            prev = 1;
        }
        else
        {
            nk_pin_setmode(dev->dio, NK_PINMODE_OUTPUT);
            nk_pin_write(dev->dio, 0);
            if (prev)
                nk_udelay(dev->tfall + 1);
            else
                nk_udelay(1); // Previous bit was low, so save time
            prev = 0;
        }
        nk_pin_setmode(dev->clk, NK_PINMODE_OUTPUT);
        nk_pin_write(dev->clk, 0);
        nk_udelay(dev->tfall + 1);
        nk_pin_setmode(dev->clk, NK_PINMODE_INPUT_PULLUP);
        nk_udelay(dev->trise + 1);
        byte >>= 1;
    }

    return 0;
}

int nk_tm1638_read_byte(const nk_tm1638_t *dev, uint8_t *byte)
{
    int x;
    uint8_t val = 0;

    nk_pin_setmode(dev->dio, NK_PINMODE_INPUT_PULLUP);
    nk_udelay(dev->trise + 1);

    for (x = 0; x != 8; ++x)
    {
        val >>= 1;

        nk_pin_setmode(dev->clk, NK_PINMODE_OUTPUT);
        nk_pin_write(dev->clk, 0);
        nk_udelay(dev->tfall + 1); // Delay for clock to fall

        nk_udelay(dev->trise + 1); // Delay for data to rise

        if (nk_pin_read(dev->dio)) // Check input
            val |= 0x80;

        nk_pin_setmode(dev->clk, NK_PINMODE_INPUT_PULLUP);
        nk_udelay(dev->trise + 1);
    }

    *byte = val;
    return 0;
}

int nk_tm1638_write(const nk_tm1638_t *dev, uint8_t *data, int len)
{
    int rtn = 0;
    nk_tm1638_start(dev);
    while (len)
    {
        rtn |= nk_tm1638_write_byte(dev, *data);
        ++data;
        --len;
    }
    nk_tm1638_stop(dev);
    return rtn;
}

int nk_tm1638_read(const nk_tm1638_t *dev, uint8_t cmd, uint8_t *data, int len)
{
    int rtn = 0;
    nk_tm1638_start(dev);
    rtn |= nk_tm1638_write_byte(dev, cmd);

    while (len)
    {
        rtn |= nk_tm1638_read_byte(dev, data);
        ++data;
        --len;
    }
    nk_tm1638_stop(dev);
    return rtn;
}

int nk_tm1638_display_raw(const nk_tm1638_t *dev, const uint16_t *digits)
{
    int x;
    uint8_t data[17];
    int rtn;
    data[0] = TM1638_MODE_LOAD;
    rtn = nk_tm1638_write(dev, data, 1);
    data[0] = TM1638_ADDR;
    for (x = 0; x != 8; ++x)
    {
        data[1 + x*2] = digits[x];
        data[1 + x*2 + 1] = (digits[x] >> 8);
    }
    rtn |= nk_tm1638_write(dev, data, 17);
    return rtn;
}

int nk_tm1638_display(const nk_tm1638_t *dev, uint8_t dp, uint8_t extra1, uint8_t extra2, const uint8_t *digits)
{
    uint16_t data[8];
    int x;
    for (x = 0; x != 8; ++x)
    {
        uint16_t val;
        if (digits[x] == '-')
            val = tm1638_font[16];
        else if (digits[x] == ' ')
            val = 0; // All off
        else if (digits[x] >= 'A' && digits[x] <= 'F')
            val = tm1638_font[digits[x] - 'A' + 10];
        else if (digits[x] >= 'a' && digits[x] <= 'f')
            val = tm1638_font[digits[x] - 'a' + 10];
        else
            val = tm1638_font[digits[x] & 0x0F];
        // Decimal point
        if (dp & (1 << x))
            val |= 0x80;
        // Extra segments
        if (extra1 & (1 << x))
            val |= 0x100;
        if (extra2 & (1 << x))
            val |= 0x200;
        data[dev->digit_map[x]] = val;
    }
    return nk_tm1638_display_raw(dev, data);
}

int nk_tm1638_display_off(const nk_tm1638_t *dev)
{
    uint8_t data[1];
    data[0] = TM1638_DISP_OFF;
    return nk_tm1638_write(dev, data, 1);
}

int nk_tm1638_display_on(const nk_tm1638_t *dev, uint8_t brightness)
{
    uint8_t data[1];
    data[0] = TM1638_DISP_ON + brightness;
    return nk_tm1638_write(dev, data, 1);
}

int nk_tm1638_keyscan(const nk_tm1638_t *dev, uint32_t *data)
{
    uint8_t val[4];
    int rtn = nk_tm1638_read(dev, TM1638_MODE_READ, val, 4);
    *data = val[0] + (val[1] << 8) + (val[2] << 16) + (val[3] << 24);
    return rtn;
}
