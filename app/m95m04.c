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
#include "led.h"
#include "nkspi.h"
#include "nkspiflash.h"
#include "pins.h"

extern uint8_t spi_buffer[];

// M95M04 EEPROM on X-NUCLEO-EEPRMA2 board

const nkspi_device_t m95m04_bus =
{
    .cs_pin = &nk_pin_table[PIN_IDX_ARD_D10],
    .hspi = &ARD_SPI
};

const struct nk_spiflash_info m95m04 =
{
    .spi_transfer = (int (*)(void *, uint8_t *, uint32_t))nk_spi_transfer,
    .spi_ptr = (void *)&m95m04_bus,
    .buffer = spi_buffer,
    .page_size = 128,
    .busy_timeout = 10000,
    .addr_size = 3,
    .n_erase_options = 0
};

static int cmd_m95m04(nkinfile_t *args)
{
    return nk_spiflash_command(&m95m04, args);
}

COMMAND(cmd_m95m04,
    ">m95m04                    Read/write memory\n"
    "-m95m04   rd <addr>        Read word\n"
    "-m95m04   wr <addr> <val>  Write word\n"
    "-m95m04   hd <addr> <len>  Hex dump memory\n"
    "-m95m04   hd <addr>        Hex dump 256 bytes\n"
    "-m95m04   crc <addr> <len> Calculate CRC of memory\n"
    "-m95m04   erase <addr> <len>\n"
    "-                          Erase memory\n"
    "-m95m04   fill <addr> <len>\n"
    "-                          Fill memory with pattern\n"
    "-m95m04   fill <addr> <len> <val>\n"
    "-                          Fill memory with constant\n"
)
