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

// PCF8575 I2C GPIO expander

// This simple device uses quasi-birdirection pins.  For each bit:
//   Write 1 for input with pull-up
//   Write 0 for output low

// This is just like the PCF8574, except 16-bits instead of 8

#include "nkcli.h"
#include "nkdriver_pcf8575.h"
#include "i2c.h"

const nk_i2c_device_t pcf8575 =
{
    .i2c_bus = &ard_i2c_bus, // Which bus it's on
    .i2c_addr = PCF8575_I2C_ADDR // I2C address of device: 0x20 - 0x27 depending on A0 - A2 inputs
};

// Command line

int cmd_pcf8575(nkinfile_t *args)
{
    uint16_t val;
    int rtn;
    if (nk_fscan(args, " ")) {
        rtn = pcf8575_read(&pcf8575, &val);
        if (rtn)
            nk_printf("I2C error\n");
        else
            nk_printf("Read value = %x\n", val);
    } else if (nk_fscan(args, "%hx ", &val)) {
        rtn = pcf8575_write(&pcf8575, val);
        if (rtn)
            nk_printf("I2C error\n");
        else
            nk_printf("Wrote %x\n", val);
    } else {
        nk_printf("Syntax error\n");
    }
    return 0;
}

COMMAND(cmd_pcf8575,
    ">pcf8575                   PCF8575 commands\n"
    "-pcf8575 [xx]              Read [write] PCF8575\n"
)
