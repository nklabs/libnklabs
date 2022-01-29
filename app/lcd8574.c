#include <string.h>
#include "nkdriver_lcd8574.h"
#include "nkdriver_hd44780.h"
#include "nkcli.h"
#include "i2c.h"

// PCF8574 device

const nk_i2c_device_t pcf8574_for_lcd =
{
    .i2c_bus = &ard_i2c_bus, // Which bus it's on
    .i2c_addr = PCF8574_ADDR + 7 // I2C address of device: 0x20 - 0x27 depending on A0 - A2 inputs
};

// HD44780 connected to PCF8574 driver
// This needs to be in RAM, not const

nk_lcd8574_t lcd8574 =
{
    .pcf8574 = &pcf8574_for_lcd,
    .regval = 0
};

// HD44780 driver

const nk_hd44780_t lcd_hd44780 =
{
    .send_command = (int (*)(void *, uint8_t cmd))nk_lcd8574_send_command,
    .send_data = (int (*)(void *, uint8_t *data, int len))nk_lcd8574_send_data,
    .ptr = &lcd8574,
    .width = 40,
    .f_bit = false,
    .n_bit = true
};

static int cmd_lcd8574(nkinfile_t *args)
{
    int rtn;
    int row;
    char buf[32];
    if (nk_fscan(args, "init ")) {
        rtn = nk_lcd8574_init(&lcd8574, true, lcd_hd44780.f_bit, lcd_hd44780.n_bit);
        if (rtn) {
            nk_printf("Pin access error\n");
        }
    } else if (nk_fscan(args, "cls ")) {
        rtn = nk_hd44780_cls(&lcd_hd44780);
        if (rtn) {
            nk_printf("Pin access error\n");
        }
    } else if (nk_fscan(args, "w %d %w ", &row, buf, sizeof(buf))) {
        rtn = nk_hd44780_pos(&lcd_hd44780, row * lcd_hd44780.width);
        if (rtn) {
            nk_printf("Pin access error\n");
        } else {
            rtn = nk_hd44780_write(&lcd_hd44780, (uint8_t *)buf, strlen(buf));
            if (rtn) {
                nk_printf("Pin access error\n");
            } else {
                nk_printf("Wrote '%s' to row %d\n", buf, row);
            }
        }
    } else {
        nk_printf("Syntax error\n");
    }
    return 0;
}

COMMAND(cmd_lcd8574,
    ">lcd8574                   TM1637 commands\n"
    "-lcd8574 init              Setup GPIO\n"
    "-lcd8574 cls               Clear screen\n"
    "-lcd8574 w <row> \"message\" Write string to row\n"
)
