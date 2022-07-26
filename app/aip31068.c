// Copyright 2021 NK Labs, LLC

// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:

// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
// OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
// THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <string.h>
#include "nkdriver_aip31068.h"
#include "nkcli.h"
#include "i2c.h"

// PCF8574 device

const nk_i2c_device_t aip31068 =
{
    .i2c_bus = &ard_i2c_bus, // Which bus it's on
    .i2c_addr = 0x3E // I2C address of device: 0x20 - 0x27 depending on A0 - A2 inputs
};

// HD44780 driver

const nk_hd44780_t aip31068_hd44780 =
{
    .send_command = (int (*)(void *, uint8_t cmd))nk_aip31068_send_command,
    .send_data = (int (*)(void *, uint8_t *data, int len))nk_aip31068_send_data,
    .ptr = &aip31068,
    .f_bit = false,
    .n_bit = true,
    .width = 16,
    .height = 2,
    .offsets = { 0, 64, 0, 64 }
};

static int cmd_aip31068(nkinfile_t *args)
{
    int rtn;
    int row;
    int col;
    char buf[32];
    if (nk_fscan(args, "init ")) {
        rtn = nk_aip31068_init(&aip31068, aip31068_hd44780.f_bit, aip31068_hd44780.n_bit);
        if (rtn) {
            nk_printf("Pin access error\n");
        }
    } else if (nk_fscan(args, "cls ")) {
        rtn = nk_hd44780_cls(&aip31068_hd44780);
        if (rtn) {
            nk_printf("Pin access error\n");
        }
    } else if (nk_fscan(args, "cursor on ")) {
        rtn = nk_hd44780_display_mode(&aip31068_hd44780, true, true, false);
        if (rtn) {
            nk_printf("Pin access error\n");
        }
    } else if (nk_fscan(args, "cursor off ")) {
        rtn = nk_hd44780_display_mode(&aip31068_hd44780, true, false, false);
        if (rtn) {
            nk_printf("Pin access error\n");
        }
    } else if (nk_fscan(args, "cursor blink ")) {
        rtn = nk_hd44780_display_mode(&aip31068_hd44780, true, true, true);
        if (rtn) {
            nk_printf("Pin access error\n");
        }
    } else if (nk_fscan(args, "cursor %d %d ", &row, &col)) {
        rtn = nk_hd44780_pos(&aip31068_hd44780, aip31068_hd44780.offsets[row] + col);
        if (rtn) {
            nk_printf("Pin access error\n");
        } else {
            nk_printf("Cursor set to row %d col %d\n", row, col);
        }
    } else if (nk_fscan(args, "cursor %d ", &row)) {
        rtn = nk_hd44780_pos(&aip31068_hd44780, aip31068_hd44780.offsets[row]);
        if (rtn) {
            nk_printf("Pin access error\n");
        } else {
            nk_printf("Cursor set to row %d col 0\n", row);
        }
    } else if (nk_fscan(args, "w %d %w ", &row, buf, sizeof(buf))) {
        rtn = nk_hd44780_pos(&aip31068_hd44780, aip31068_hd44780.offsets[row]);
        if (rtn) {
            nk_printf("Pin access error\n");
        } else {
                nk_printf("Wrote '%s' to row %d\n", buf, row);
            rtn = nk_hd44780_write(&aip31068_hd44780, (uint8_t *)buf, strlen(buf));
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

COMMAND(cmd_aip31068,
    ">aip31068                  AIP31068 commands\n"
    "-aip31068 init             Initialize\n"
    "-aip31068 cls              Clear screen\n"
    "-aip31068 cursor on|off|blink|<row>|<row> <col>\n"
    "-                          Control cursor\n"
    "-ai31068 w <row> \"message\" Write string to row 0..1\n"
)
