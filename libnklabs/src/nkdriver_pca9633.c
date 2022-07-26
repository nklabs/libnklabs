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

// PCA9633 PWM LED modulator
//  This appears on one of the LCD1602 displays to control the backlight
//  On that board: Blue is PWM0, Green is PWM1 and Red is PWM2.  PWM3 is unused.

#include "nkdriver_pca9633.h"

// To set PWM frequency 

int nk_pca9633_setup(const nk_i2c_device_t *dev)
{
    int rtn;

    // Enable it
    rtn = nk_i2c_put_byte(dev, PCA9633_REG_MODE1, 0);

    // Allow PWM and GRP to control each LED
    rtn |= nk_i2c_put_byte(dev, PCA9633_REG_LEDOUT,
        (PCA9633_LDR_PWM << 0) |
        (PCA9633_LDR_PWM << 2) |
        (PCA9633_LDR_PWM << 4) |
        (PCA9633_LDR_PWM << 6)
    );

    return rtn;
}

// Set PWM fraction for a channel.  PWM should be between 0.0 and 1.0

int nk_pca9633_set(const nk_i2c_device_t *dev, int led, float pwm)
{
    uint8_t val = (pwm * 255.9f);
    return nk_i2c_put_byte(dev, PCA9633_REG_PWM0 + led, val);
}

// Read PWM fraction from a channel

int nk_pca9633_get(const nk_i2c_device_t *dev, int led, float *pwm)
{
    uint8_t val;
    int rtn;
    
    rtn = nk_i2c_get_byte(dev, PCA9633_REG_PWM0 + led, &val);
    *pwm = (float)val / 255.0f;

    return rtn;
}
