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

// MCP4725 12-bit I2C DAC

#include "nkcli.h"
#include "nkdriver_mcp4725.h"
#include "i2c.h"

const nk_i2c_device_t mcp4725 =
{
    .i2c_bus = &ard_i2c_bus,
    .i2c_addr = MCP4725_I2C_ADDR
};

static int cmd_mcp4725(nkinfile_t *args)
{
    uint16_t val;
    if (nk_fscan(args, "%hu ", &val)) {
        int rtn = nk_mcp4725_write(&mcp4725, val);
        if (rtn) {
            nk_printf("I2C error\n");
        } else {
            nk_printf("Wrote %d to MCP4725\n", val);
        }
    } else {
        nk_printf("Syntax error\n");
    }
    return 0;
}

COMMAND(cmd_mcp4725,
    ">mcp4725                   MCP4725 commands\n"
    "-mcp4725 nnn               Write DAC 0 - 4095\n"
)
