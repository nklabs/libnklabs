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

// INA219 bidirectional I2C current/power/voltage monitor

#include "nkcli.h"
#include "nki2c.h"
#include "nkdriver_ina219.h"
#include "i2c.h"

const nk_i2c_device_t ina219 =
{
    .i2c_bus = &ard_i2c_bus,
    .i2c_addr = INA219_I2C_ADDR
};

static int cmd_ina219(nkinfile_t *args)
{
    float vbus, vshunt, power, current;
    if (nk_fscan(args, "")) {
        int rtn = nk_ina219_read(&ina219, &vbus, &vshunt, &power, &current);
        if (rtn) {
            nk_printf("I2C error\n");
        } else {
            nk_printf("VBUS = %g V\n", vbus);
            nk_printf("VSHUNT = %g V\n", vshunt);
            nk_printf("POWER = %g W\n", power);
            nk_printf("CURRENT = %g A\n", current);
        }
    } else if (nk_fscan(args, "init ")) {
        int rtn = nk_ina219_init(&ina219);
        if (rtn) {
            nk_printf("I2C error\n");
        } else {
            nk_printf("Calibration value installed\n");
        }
    } else {
        nk_printf("Syntax error\n");
    }
    return 0;
}

COMMAND(cmd_ina219,
    ">ina219                    INA219 commands\n"
    "-ina219                    Read voltage/current/power\n"
)
