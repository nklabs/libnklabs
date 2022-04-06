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
#include "nkarch.h"
#include "nkcli.h"
#include "i2c.h"

uint8_t i2c_buffer[16];

// Arduino pin I2C bus

const nk_i2c_bus_t ard_i2c_bus =
{
	.i2c_write = nk_hal_i2c_write,
	.i2c_write_nostop = nk_hal_i2c_write_nostop,
	.i2c_read = nk_hal_i2c_read,
	.i2c_ptr = &ARD_I2C
};

// I2C bus CLI command

static int cmd_i2c(nkinfile_t *args)
{
	return nk_i2c_command(&ard_i2c_bus, args);
}

COMMAND(cmd_i2c,
	">i2c                       I2C access\n"
	"-i2c scan                  Scan I2C bus\n"
	"-i2c [w AA DD ...] [r AA NN]\n"
	"-                          Raw I2C transaction\n"
)
