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

// W25Q64 SPI Flash device

const nkspi_device_t w25q64_bus =
{
    .cs_pin = &nk_pin_table[PIN_IDX_ARD_D10],
    .hspi = &ARD_SPI
};

const struct nk_spiflash_info w25q64 =
{
    .spi_transfer = (int (*)(void *, uint8_t *, uint32_t))nk_spi_transfer,
    .spi_ptr = (void *)&w25q64_bus,
    .buffer = spi_buffer,
    .page_size = 128,
    .busy_timeout = 10000,
    .addr_size = 3,
    .n_erase_options = 3,
    .erase_options = {
        { 65536, 0xd8 },
        { 32768, 0x52 },
        { 4096, 0x20 }
    }
};

static int cmd_w25q64(nkinfile_t *args)
{
    return nk_spiflash_command(&w25q64, args);
}

COMMAND(cmd_w25q64,
    ">w25q64                    Read/write memory\n"
    "-w25q64   rd <addr>        Read word\n"
    "-w25q64   wr <addr> <val>  Write word\n"
    "-w25q64   hd <addr> <len>  Hex dump memory\n"
    "-w25q64   hd <addr>        Hex dump 256 bytes\n"
    "-w25q64   crc <addr> <len> Calculate CRC of memory\n"
    "-w25q64   erase <addr> <len>\n"
    "-                          Erase memory\n"
    "-w25q64   unlock           Unlock device (write 0 to status register)\n"
    "-w25q64   fill <addr> <len>\n"
    "-                          Fill memory with pattern\n"
    "-w25q64   fill <addr> <len> <val>\n"
    "-                          Fill memory with constant\n"
)
