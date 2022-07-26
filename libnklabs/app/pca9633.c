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
#include "nkdriver_pca9633.h"
#include "i2c.h"

const nk_i2c_device_t pca9633 =
{
    .i2c_bus = &ard_i2c_bus,
    .i2c_addr = PCA9633_I2C_ADDR_10PIN
};

static int cmd_pca9633(nkinfile_t *args)
{
    int rtn;
    int led;
    double pwm;
    if (nk_fscan(args, "")) {
    } else if (nk_fscan(args, "init ")) {
        rtn = nk_pca9633_setup(&pca9633);
        if (rtn) {
            nk_printf("I2C error\n");
        } else {
            nk_printf("PCA9633 Set up for 50 Hz\n");
        }
    } else if (nk_fscan(args, "pwm %d %f ", &led, &pwm)) {
        rtn = nk_pca9633_set(&pca9633, led, pwm);
        if (rtn) {
            nk_printf("I2C error\n");
        } else {
            nk_printf("PCA9633 channel %d set to %g%%\n", led, pwm/100.0);
        }
    } else {
        nk_printf("Syntax error\n");
    }
    return 0;
}

COMMAND(cmd_pca9633,
    ">pca9633                   PCA9633 commands\n"
    "-pca9633 init              Initialize\n"
    "-pca9633 pwm nn pp         Set channel nn (0 - 3) to a specific PWM duty cycle (0 - 100)\n"
)
