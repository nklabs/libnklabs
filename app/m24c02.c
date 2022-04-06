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
#include "nki2c_eeprom.h"
#include "nki2c.h"
#include "i2c.h"

// m24c02 I2C EEPROM on X-NUCLEO-EEPRMA2 board

#define M24C02_I2C_ADDR 0x54

const nk_i2c_device_t m24c02_dev =
{
    .i2c_bus = &ard_i2c_bus,
    .i2c_addr = M24C02_I2C_ADDR
};

const struct nk_i2c_eeprom_info m24c02 =
{
	.dev = &m24c02_dev,

	.buffer = i2c_buffer,

	.page_size = 16,
	.busy_timeout = 100,
	.addr_size = 1
};

static int cmd_m24c02(nkinfile_t *args)
{
    return nk_i2c_eeprom_command(&m24c02, args);
}

COMMAND(cmd_m24c02,
    ">m24c02                    Read/write memory\n"
    "-m24c02   rd <addr>        Read word\n"
    "-m24c02   wr <addr> <val>  Write word\n"
    "-m24c02   hd <addr> <len>  Hex dump memory\n"
    "-m24c02   hd <addr>        Hex dump 256 bytes\n"
    "-m24c02   crc <addr> <len> Calculate CRC of memory\n"
    "-m24c02   fill <addr> <len>\n"
    "-                          Fill memory with pattern\n"
    "-m24c02   fill <addr> <len> <val>\n"
    "-                          Fill memory with constant\n"
)
