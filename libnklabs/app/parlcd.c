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
#include "nkdriver_parlcd.h"
#include "nkcli.h"
#include "pins.h"

// HD44780 connected directly to Arduino pins

const nk_parlcd_t parlcd =
{
    .d4 = &nk_pin_table[PIN_IDX_ARD_D4],
    .d5 = &nk_pin_table[PIN_IDX_ARD_D5],
    .d6 = &nk_pin_table[PIN_IDX_ARD_D6],
    .d7 = &nk_pin_table[PIN_IDX_ARD_D7],
    .e = &nk_pin_table[PIN_IDX_ARD_D9],
    .rs = &nk_pin_table[PIN_IDX_ARD_D8],
    .backlight = &nk_pin_table[PIN_IDX_ARD_D10]
};

// HD44780 driver

const nk_hd44780_t parlcd_hd44780 =
{
    .send_command = (int (*)(void *, uint8_t cmd))nk_parlcd_send_command,
    .send_data = (int (*)(void *, uint8_t *data, int len))nk_parlcd_send_data,
    .ptr = &parlcd,
    .f_bit = false,
    .n_bit = true,
    .width = 16,
    .height = 2,
    // This works for both 1602 and 2004
    .offsets = { 0, 40, 20, 84 }
    // This probably makes more sense for 2004..
    // { 0, 64, 20, 84 }
};

static int cmd_parlcd(nkinfile_t *args)
{
    int rtn;
    int row;
    int col;
    char buf[32];
    if (nk_fscan(args, "init ")) {
        rtn = nk_parlcd_init(&parlcd, true, parlcd_hd44780.f_bit, parlcd_hd44780.n_bit);
        if (rtn) {
            nk_printf("Pin access error\n");
        }
    } else if (nk_fscan(args, "cls ")) {
        rtn = nk_hd44780_cls(&parlcd_hd44780);
        if (rtn) {
            nk_printf("Pin access error\n");
        }
    } else if (nk_fscan(args, "cursor on ")) {
        rtn = nk_hd44780_display_mode(&parlcd_hd44780, true, true, false);
        if (rtn) {
            nk_printf("Pin access error\n");
        }
    } else if (nk_fscan(args, "cursor off ")) {
        rtn = nk_hd44780_display_mode(&parlcd_hd44780, true, false, false);
        if (rtn) {
            nk_printf("Pin access error\n");
        }
    } else if (nk_fscan(args, "cursor blink ")) {
        rtn = nk_hd44780_display_mode(&parlcd_hd44780, true, true, true);
        if (rtn) {
            nk_printf("Pin access error\n");
        }
    } else if (nk_fscan(args, "cursor %d %d ", &row, &col)) {
        rtn = nk_hd44780_pos(&parlcd_hd44780, parlcd_hd44780.offsets[row] + col);
        if (rtn) {
            nk_printf("Pin access error\n");
        } else {
            nk_printf("Cursor set to row %d col %d\n", row, col);
        }
    } else if (nk_fscan(args, "cursor %d ", &row)) {
        rtn = nk_hd44780_pos(&parlcd_hd44780, parlcd_hd44780.offsets[row]);
        if (rtn) {
            nk_printf("Pin access error\n");
        } else {
            nk_printf("Cursor set to row %d col 0\n", row);
        }
    } else if (nk_fscan(args, "w %d %w ", &row, buf, sizeof(buf))) {
        rtn = nk_hd44780_pos(&parlcd_hd44780, parlcd_hd44780.offsets[row]);
        if (rtn) {
            nk_printf("Pin access error\n");
        } else {
            rtn = nk_hd44780_write(&parlcd_hd44780, (uint8_t *)buf, strlen(buf));
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

COMMAND(cmd_parlcd,
    ">parlcd                    Parallel LCD commands\n"
    "-parlcd init               Setup GPIO\n"
    "-parlcd cls                Clear screen\n"
    "-parlcd cursor on|off|blink|<row>|<row> <col>\n"
    "-                          Control cursor\n"
    "-parlcd w <row> \"message\" Write string to row 0..3\n"
)
