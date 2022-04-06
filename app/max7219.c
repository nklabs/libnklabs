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
#include "nkdriver_max7219.h"
#include "pins.h"

const nk_max7219_t max7219 =
{
    .sclk = &nk_pin_table[PIN_IDX_ARD_D2],
    .mosi = &nk_pin_table[PIN_IDX_ARD_D3],
    .load = &nk_pin_table[PIN_IDX_ARD_D4],

    .trise = 1,
    .tfall = 1,

    .digit_map = { 7, 6, 5, 4, 3, 2, 1, 0 } // We want 0 to be left-most digit
};

int max7219_tid;
int max7219_counter;

void max7219_count(void *data)
{
    uint8_t buf[9];
    nk_snprintf((char *)buf, sizeof(buf), "%6d--", max7219_counter);
    nk_max7219_display(&max7219, (1 << (max7219_counter & 7)), buf);
    ++max7219_counter;
    nk_sched(max7219_tid, max7219_count, NULL, 1000, "MAX7219 counter");
}

static int cmd_max7219(nkinfile_t *args)
{
    uint8_t buf[12];
    int rtn;
    int bright;
    if (nk_fscan(args, "init "))
    {
        rtn = nk_max7219_init(&max7219);
        rtn |= nk_max7219_display_on(&max7219, 4);
        if (rtn)
        {
            nk_printf("Pin access error\n");
        }
    }
    else if (nk_fscan(args, "w %w", buf, sizeof(buf)))
    {
	uint8_t write_array[8];
	nk_sfprintf((char *)write_array, sizeof(write_array), "%s", buf);
        rtn = nk_max7219_display(&max7219, 0, write_array);
        if (rtn)
        {
            nk_printf("No ack\n");
        }
        else
        {
            nk_printf("Wrote '%s'\n", buf);
        }
    }
    else if (nk_fscan(args, "raw %e"))
    {
	uint8_t write_array[8];
	uint32_t write_len = 0;
	memset(write_array, 0, sizeof(write_array));
        while (nk_fscan(args, "%hx %e", &write_array[write_len]))
            if (write_len != 7)
	        ++write_len;
        rtn = nk_max7219_display_raw(&max7219, write_array);
        if (rtn)
        {
            nk_printf("No ack\n");
        }
        else
        {
            nk_printf("Wrote %lu bytes\n", write_len + 1);
        }
    }
    else if (nk_fscan(args, "count "))
    {
        rtn = nk_max7219_init(&max7219);
        if (rtn)
        {
            nk_printf("Pin access error\n");
            return 0;
        }
        if (!max7219_tid)
            max7219_tid = nk_alloc_tid();
        max7219_counter = 0;
        max7219_count(NULL);
    }
    else if(nk_fscan(args, "on %d ", &bright))
    {
        rtn = nk_max7219_display_on(&max7219, bright);
        if (rtn)
        {
            nk_printf("Pin access error\n");
        }
    }
    else if(nk_fscan(args, "off "))
    {
        rtn = nk_max7219_display_off(&max7219);
        if (rtn)
        {
            nk_printf("Pin access error\n");
        }
    }
    else
    {
        nk_printf("Syntax error\n");
    }
    return 0;
}

COMMAND(cmd_max7219,
    ">max7219                   MAX7219 commands\n"
    "-max7219 init              Setup GPIO\n"
    "-max7219 on <brightness>   Display on with brightness 0 - 7\n"
    "-max7219 off               Display off\n"
    "-max7219 w <ascii>         Write ASCII to 7-segment\n"
    "-max7219 raw ff ff ..      Write 6 8-bit words to MAX7219 memory\n"
    "-max7219 count             Start counter\n"
)
