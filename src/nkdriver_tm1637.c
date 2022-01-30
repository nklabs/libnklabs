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

uint8_t font[17] =
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

int nk_tm1637_start(const nk_tm1637_t *dev)
{
    // (clock should already be high)
    nk_pin_setmode(dev->clk, NK_PINMODE_INPUT_PULLUP);
    nk_udelay(dev->trise + 1);

    // Lower DIO
    nk_pin_setmode(dev->dio, NK_PINMODE_OUTPUT);
    nk_pin_write(dev->dio, 0);
    nk_udelay(dev->tfall + 1);
    nk_udelay(dev->trise + 1);
    return 0;
}

int nk_tm1637_stop(const nk_tm1637_t *dev)
{
    // Lower clock (it should have already been high)
    nk_pin_setmode(dev->clk, NK_PINMODE_OUTPUT);
    nk_pin_write(dev->clk, 0);
    nk_udelay(dev->tfall + 1);

    // Lower data (we don't know what it had been)
    nk_pin_setmode(dev->dio, NK_PINMODE_OUTPUT);
    nk_pin_write(dev->dio, 0);
    nk_udelay(dev->tfall + 1);

    // Raise clock
    nk_pin_setmode(dev->clk, NK_PINMODE_INPUT_PULLUP);
    nk_udelay(dev->trise + 1);

    // Raise DIO
    nk_pin_setmode(dev->dio, NK_PINMODE_INPUT_PULLUP);
    nk_udelay(dev->trise + 1);
    return 0;
}

int nk_tm1637_init(const nk_tm1637_t *dev)
{
    // This sequence ensures that TM1637 sees a stop
    nk_tm1637_start(dev);
    nk_tm1637_stop(dev);
    return 0;
}

int nk_tm1637_write_byte(const nk_tm1637_t *dev, uint8_t byte)
{
    int rtn;
    int x;
    int prev = 2; // DIO is in unknown state

    for (x = 0; x != 8; ++x)
    {
        // Lower clock
        nk_pin_setmode(dev->clk, NK_PINMODE_OUTPUT);
        nk_pin_write(dev->clk, 0);
        nk_udelay(dev->tfall + 1);

        // Write data
        if (byte & 1)
        {
            nk_pin_setmode(dev->dio, NK_PINMODE_INPUT_PULLUP);
            if (prev == 1)
                nk_udelay(1); // It was already high
            else
                nk_udelay(dev->trise + 1);
            prev = 1;
        }
        else
        {
            nk_pin_setmode(dev->dio, NK_PINMODE_OUTPUT);
            nk_pin_write(dev->dio, 0);
            if (prev == 0)
                nk_udelay(1); // It was already low
            else
                nk_udelay(dev->tfall + 1);
            prev = 0;
        }
        // Raise clock
        nk_pin_setmode(dev->clk, NK_PINMODE_INPUT_PULLUP);
        nk_udelay(dev->trise + 1);
        byte >>= 1;
    }

    // Lower clock
    nk_pin_setmode(dev->clk, NK_PINMODE_OUTPUT);
    nk_pin_write(dev->clk, 0);
    nk_udelay(dev->tfall + 1); // Fall time

    // Check for ACK
    // Raise DIO so that other side can drive it
    nk_pin_setmode(dev->dio, NK_PINMODE_INPUT_PULLUP);
    if (prev == 1)
        nk_udelay(1); // DIO was already high, wait for it to possibly fall
    else
        nk_udelay(dev->trise + 1); // Wait for DIO to rise

    // Check for ACK
    rtn = nk_pin_read(dev->dio);

    // Raise clock
    nk_pin_setmode(dev->clk, NK_PINMODE_INPUT_PULLUP);
    nk_udelay(dev->trise + 1);

    // Should be low for ACK..
    return rtn;
}

int nk_tm1637_read_byte(const nk_tm1637_t *dev, uint8_t *byte)
{
    int rtn;
    int x;
    uint8_t val = 0;

    for (x = 0; x != 8; ++x)
    {
        val >>= 1;

        // Lower clock
        nk_pin_setmode(dev->clk, NK_PINMODE_OUTPUT);
        nk_pin_write(dev->clk, 0);
        nk_udelay(dev->tfall + 1); // Wait clock fall time

        nk_pin_setmode(dev->dio, NK_PINMODE_INPUT_PULLUP); // Release DIO
        nk_udelay(dev->trise + 1); // Wait rise time for TM1637 driving DIO

        // Check input
        if (nk_pin_read(dev->dio))
            val |= 0x80;

        // Raise clock
        nk_pin_setmode(dev->clk, NK_PINMODE_INPUT_PULLUP);
        nk_udelay(dev->trise + 1);
    }

    // Lower clock
    nk_pin_setmode(dev->clk, NK_PINMODE_OUTPUT);
    nk_pin_write(dev->clk, 0);
    nk_udelay(dev->tfall + 1); // Fall time

    // Check for ACK
    // Raise DIO so that other side can drive it
    nk_pin_setmode(dev->dio, NK_PINMODE_INPUT_PULLUP);
    nk_udelay(dev->trise + 1); // DIO rise time

    // Check for ACK
    rtn = nk_pin_read(dev->dio);

    // Raise clock
    nk_pin_setmode(dev->clk, NK_PINMODE_INPUT_PULLUP);
    nk_udelay(dev->trise + 1);

    *byte = val;

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

int nk_tm1637_read(const nk_tm1637_t *dev, uint8_t cmd, uint8_t *data, int len)
{
    int rtn = 0;
    nk_tm1637_start(dev);
    rtn |= nk_tm1637_write_byte(dev, cmd);
    while (len)
    {
        rtn |= nk_tm1637_read_byte(dev, data);
        ++data;
        --len;
    }
    nk_tm1637_stop(dev);
    return rtn;
}

int nk_tm1637_display_raw(const nk_tm1637_t *dev, const uint8_t *digits)
{
    int x;
    uint8_t data[7];
    int rtn;

    data[0] = TM1637_MODE_LOAD;
    rtn = nk_tm1637_write(dev, data, 1);

    data[0] = TM1637_ADDR;
    for (x = 0; x != 6; ++x)
    {
        data[x + 1] = digits[x];
    }
    rtn |= nk_tm1637_write(dev, data, 7);
    return rtn;
}

int nk_tm1637_display(const nk_tm1637_t *dev, uint8_t dp, const uint8_t *digits)
{
    uint8_t data[6];
    int x;
    for (x = 0; x != 6; ++x)
    {
        uint8_t val;
        if (digits[x] == '-')
            val = font[16];
        else if (digits[x] == ' ')
            val = 0; // All off
        else if (digits[x] >= 'A' && digits[x] <= 'F')
            val = font[digits[x] - 'A' + 10];
        else if (digits[x] >= 'a' && digits[x] <= 'f')
            val = font[digits[x] - 'a' + 10];
        else
            val = font[digits[x] & 0x0F];
        // Decimal point
        if (dp & (1 << x))
            val |= SEG_DP;
        data[dev->digit_map[x]] = val;
    }
    return nk_tm1637_display_raw(dev, data);
}

int nk_tm1637_display_off(const nk_tm1637_t *dev)
{
    uint8_t data[1];
    data[0] = TM1637_DISP_OFF;
    return nk_tm1637_write(dev, data, 1);
}

int nk_tm1637_display_on(const nk_tm1637_t *dev, uint8_t brightness)
{
    uint8_t data[1];
    data[0] = TM1637_DISP_ON + brightness;
    return nk_tm1637_write(dev, data, 1);
}

int nk_tm1637_keyscan(const nk_tm1637_t *dev, uint8_t *data)
{
    return nk_tm1637_read(dev, TM1637_MODE_READ, data, 1);
}
