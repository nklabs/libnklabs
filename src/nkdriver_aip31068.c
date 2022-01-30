#include "nkdriver_aip31068.h"

// Initialize HD44780 into 8-bit mode

int nk_aip31068_init(const nk_i2c_device_t *dev, bool f_bit, bool n_bit)
{
    int rtn = 0;
    uint8_t func;

    func = HD44780_FUNC | HD44780_FUNC_DL | (f_bit ? HD44780_FUNC_F : 0) | (n_bit ? HD44780_FUNC_N : 0);

    nk_udelay(15000); // 15 ms for display to boot
    rtn |= nk_aip31068_send_command(dev, func);
    nk_udelay(4100);
    rtn |= nk_aip31068_send_command(dev, func);
    nk_udelay(61);
    rtn |= nk_aip31068_send_command(dev, func);

    // Display off, cursor off, blink off
    rtn |= nk_aip31068_send_command(dev, HD44780_DISPLAY);

    // Clear screen, home cursor
    rtn |= nk_aip31068_send_command(dev, HD44780_CLS);
    nk_udelay(1530);

    // Display on, cursor on, blink on
    rtn |= nk_aip31068_send_command(dev, HD44780_DISPLAY | HD44780_DISPLAY_D | HD44780_DISPLAY_C | HD44780_DISPLAY_B);
 
    return rtn;
}

// Send HD44780 command byte

int nk_aip31068_send_command(const nk_i2c_device_t *dev, uint8_t cmd)
{
    uint8_t buf[2];
    buf[0] = AIP31068_CO;
    buf[1] = cmd;
    return nk_i2c_write(dev, 2, buf);
}

// Write HD44780 text

int nk_aip31068_send_data(const nk_i2c_device_t *dev, uint8_t *data, int len)
{
    int rtn = 0;
    while (len--)
    {
        uint8_t buf[2];
        buf[0] = AIP31068_CO + AIP31068_RS;
        buf[1] = *data++;
        rtn |= nk_i2c_write(dev, 2, buf);
    }
    return rtn;
}
