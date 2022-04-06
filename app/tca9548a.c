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

#include "nkcli.h"
#include "nkdriver_tca9548a.h"
#include "i2c.h"

const nk_i2c_device_t tca9548a =
{
    .i2c_bus = &ard_i2c_bus,
    .i2c_addr = TCA9548A_I2C_ADDR + 0 
};

static int cmd_tca9548a(nkinfile_t *args)
{
    uint8_t val;
    if (nk_fscan(args, "")) {
        int rtn = nk_tca9548a_get(&tca9548a, &val);
        if (rtn) {
            nk_printf("I2C error\n");
        } else {
            nk_printf("Bus switch set to %x\n", val);
        }
    } else if (nk_fscan(args, "%hhx ", &val)) {
        int rtn = nk_tca9548a_set(&tca9548a, val);
        if (rtn) {
            nk_printf("I2C error\n");
        } else {
            nk_printf("Bus switch set to %x\n", val);
        }
    } else {
        nk_printf("Syntax error\n");
    }
    return 0;
}

COMMAND(cmd_tca9548a,
    ">tca9548a                  TCA9548A commands\n"
    "-tca9548a [xx]             Show / [Set] bus switch\n"
)
