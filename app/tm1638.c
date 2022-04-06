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
#include "nkcli.h"
#include "nksched.h"
#include "nkdriver_tm1638.h"
#include "pins.h"

const nk_tm1638_t tm1638 =
{
    .clk = &nk_pin_table[PIN_IDX_ARD_D2],
    .dio = &nk_pin_table[PIN_IDX_ARD_D3],
    .stb = &nk_pin_table[PIN_IDX_ARD_D4],
    .trise = 2,
    .tfall = 1,

    // Most are like this
    // .digit_map = { 0, 1, 2, 3, 4, 5, 6, 7 }, // Map so that digit[0] is left-most
    // .ca = false

    // "QYF-1638" (the board with 8 digits and 16 buttons) is like this
    .digit_map = { 7, 6, 5, 4, 3, 2, 1, 0, 9, 8 },
    .ca = true
};

int tm1638_tid;
int tm1638_counter;

void tm1638_count(void *data)
{
    uint8_t buf[9];
    nk_snprintf((char *)buf, 9, "%6d--", tm1638_counter);
    nk_tm1638_display(&tm1638, (1 << (tm1638_counter & 7)), (0x80 >> (tm1638_counter & 7)), 0, buf);
    ++tm1638_counter;
    nk_sched(tm1638_tid, tm1638_count, NULL, 1000, "TM1638 counter");
}

static int cmd_tm1638(nkinfile_t *args)
{
    uint8_t buf[12];
    int rtn;
    int bright;
    if (nk_fscan(args, "init ")) {
        rtn = nk_tm1638_init(&tm1638);
        rtn |= nk_tm1638_display_on(&tm1638, 4);
        if (rtn) {
            nk_printf("Pin access error\n");
        }
    } else if (nk_fscan(args, "w %w", buf, sizeof(buf))) {
	uint8_t write_array[10];
	nk_sfprintf((char *)write_array, sizeof(write_array), "%s", buf);
        rtn = nk_tm1638_display(&tm1638, 0, 0, 0, write_array);
        if (rtn) {
            nk_printf("No ack\n");
        } else {
            nk_printf("Wrote '%s'\n", buf);
        }
    } else if (nk_fscan(args, "raw %e")) {
	uint16_t write_array[8];
	uint32_t write_len = 0;
	memset(write_array, 0, sizeof(write_array));
        while (nk_fscan(args, "%hx %e", &write_array[write_len]))
            if (write_len != 7)
	        ++write_len;
        rtn = nk_tm1638_display_raw(&tm1638, write_array);
        if (rtn) {
            nk_printf("No ack\n");
        } else {
            nk_printf("Wrote %lu shorts\n", write_len + 1);
        }
    } else if (nk_fscan(args, "count ")) {
        rtn = nk_tm1638_init(&tm1638);
        if (rtn) {
            nk_printf("Pin access error\n");
            return 0;
        }
        if (!tm1638_tid)
            tm1638_tid = nk_alloc_tid();
        tm1638_counter = 0;
        tm1638_count(NULL);
    } else if(nk_fscan(args, "scan ")) {
        uint32_t val;
        rtn = nk_tm1638_keyscan(&tm1638, &val);
        if (rtn) {
            nk_printf("Pin access error\n");
        } else {
            nk_printf("%8.8lx\n", val);
        }
    } else if(nk_fscan(args, "on %d ", &bright)) {
        rtn = nk_tm1638_display_on(&tm1638, bright);
        if (rtn) {
            nk_printf("Pin access error\n");
        }
    } else if(nk_fscan(args, "off ")) {
        rtn = nk_tm1638_display_off(&tm1638);
        if (rtn) {
            nk_printf("Pin access error\n");
        }
    } else {
        nk_printf("Syntax error\n");
    }
    return 0;
}

COMMAND(cmd_tm1638,
    ">tm1638                    TM1638 commands\n"
    "-tm1638 init               Setup GPIO\n"
    "-tm1638 on <brightness>    Display on with brightness 0 - 7\n"
    "-tm1638 off                Display off\n"
    "-tm1638 scan               Keyscan\n"
    "-tm1638 w <ascii>          Write ASCII to 7-segment\n"
    "-tm1638 raw ffff ffff ..   Write 8 16-bit words to TM1638 memory\n"
    "-tm1638 count              Start counter\n"
)
