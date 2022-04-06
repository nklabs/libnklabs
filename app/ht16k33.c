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
#include "nkdriver_ht16k33.h"
#include "i2c.h"

const nk_i2c_device_t ht16k33 =
{
    .i2c_bus = &ard_i2c_bus, // Which bus it's on
    .i2c_addr = HT16K33_I2C_ADDR // I2C address of device
};

int ht16k33_tid;
int ht16k33_counter;

void ht16k33_count(void *data)
{
    uint8_t buf[9];
    nk_snprintf((char *)buf, 9, "%4d", ht16k33_counter);
    nk_ht16k33_display(&ht16k33, (1 << (ht16k33_counter & 7)), buf);
    ++ht16k33_counter;
    nk_sched(ht16k33_tid, ht16k33_count, NULL, 1000, "HT16K33 counter");
}

static int cmd_ht16k33(nkinfile_t *args)
{
    uint8_t buf[12];
    int rtn;
    int bright;
    if (nk_fscan(args, "init ")) {
        rtn = nk_ht16k33_init(&ht16k33);
        rtn |= nk_ht16k33_display_on(&ht16k33, 4);
        if (rtn) {
            nk_printf("Pin access error\n");
        }
    } else if (nk_fscan(args, "w %w", buf, sizeof(buf))) {
	uint8_t write_array[8];
	nk_sfprintf((char *)write_array, sizeof(write_array), "%s", buf);
        rtn = nk_ht16k33_display(&ht16k33, 0, write_array);
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
        rtn = nk_ht16k33_display_raw(&ht16k33, write_array);
        if (rtn) {
            nk_printf("No ack\n");
        } else {
            nk_printf("Wrote %lu shorts\n", write_len + 1);
        }
    } else if (nk_fscan(args, "count ")) {
        rtn = nk_ht16k33_init(&ht16k33);
        if (rtn) {
            nk_printf("Pin access error\n");
            return 0;
        }
        if (!ht16k33_tid)
            ht16k33_tid = nk_alloc_tid();
        ht16k33_counter = 0;
        ht16k33_count(NULL);
    } else if(nk_fscan(args, "scan ")) {
        uint8_t data[6];
        rtn = nk_ht16k33_keyscan(&ht16k33, data);
        if (rtn) {
            nk_printf("Pin access error\n");
        } else {
            int x;
            for (x = 0; x != 6; ++x)
                nk_printf(" %2.2x", data[x]);
            nk_printf("\n");
        }
    } else if(nk_fscan(args, "on %d ", &bright)) {
        rtn = nk_ht16k33_display_on(&ht16k33, bright);
        if (rtn) {
            nk_printf("Pin access error\n");
        }
    } else if(nk_fscan(args, "off ")) {
        rtn = nk_ht16k33_display_off(&ht16k33);
        if (rtn) {
            nk_printf("Pin access error\n");
        }
    } else {
        nk_printf("Syntax error\n");
    }
    return 0;
}

COMMAND(cmd_ht16k33,
    ">ht16k33                   HT16K33 commands\n"
    "-ht16k33 init              Setup GPIO\n"
    "-ht16k33 on <brightness>   Display on with brightness 0 - 7\n"
    "-ht16k33 off               Display off\n"
    "-ht16k33 scan              Keyscan\n"
    "-ht16k33 w ff ff ..        Write hex bytes, translate ASCII to 7-segment\n"
    "-ht16k33 raw ffff ffff ..  Write 8 16-bit words to HT16K33 memory\n"
    "-ht16k33 count             Start counter\n"
)
