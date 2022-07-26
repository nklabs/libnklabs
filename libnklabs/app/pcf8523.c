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

#include "nkdriver_pcf8523.h"
#include "i2c.h"
#include "nkrtc.h"

const nk_i2c_device_t pcf8523_device =
{
    .i2c_bus = &ard_i2c_bus, // Which bus it's on
    .i2c_addr = PCF8523_I2C_ADDR // I2C address of device
};

const nk_rtc_t pcf8523 =
{
    .set_datetime = (int (*)(const void *, const nkdatetime_t *))nk_pcf8523_set_datetime,
    .get_datetime = (int (*)(const void *, nkdatetime_t *))nk_pcf8523_get_datetime,
    .port = &pcf8523_device
};

int cmd_pcf8523(nkinfile_t *args)
{
    return nk_rtc_command(&pcf8523, args);
}

COMMAND(cmd_pcf8523,
	">pcf8523                   External RTC access\n"
        "-pcf8523                   Show date/time\n"
        "-pcf8523 YYYY-MM-DD HH:MM:SS\n"
        "-                          Set date/time\n"
)
