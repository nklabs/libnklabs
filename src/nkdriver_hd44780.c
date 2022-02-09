#include "nkdriver_hd44780.h"

// Set display mode

int nk_hd44780_display_mode(const nk_hd44780_t *dev, bool display_on, bool cursor_on, bool cursor_blink)
{
    return dev->send_command(dev->ptr,
        HD44780_DISPLAY |
        (display_on ? HD44780_DISPLAY_D : 0) |
        (cursor_on ? HD44780_DISPLAY_C : 0) |
        (cursor_blink ? HD44780_DISPLAY_B : 0)
    );
}

// Clear screen
int nk_hd44780_cls(const nk_hd44780_t *dev)
{
    int rtn = dev->send_command(dev->ptr, HD44780_CLS);
    nk_udelay(1530);
    return rtn;
}

// Home cursor
int nk_hd44780_home(const nk_hd44780_t *dev)
{
    return dev->send_command(dev->ptr, HD44780_HOME);
}

// Set cursor position
int nk_hd44780_pos(const nk_hd44780_t *dev, uint8_t position)
{
    return dev->send_command(dev->ptr, HD44780_DDRAM + position);
}

// Write data
int nk_hd44780_write(const nk_hd44780_t *dev, uint8_t *data, int len)
{
    return dev->send_data(dev->ptr, data, len);
}
