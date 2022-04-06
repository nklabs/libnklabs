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
#include "nkspi.h"
#include "nkspiflash.h"

extern uint8_t spi_buffer[];

// AT25DF081A SPI-flash

#ifdef ALT_SPI

const nkspi_device_t at25df081_bus =
{
    .cs_pin = ALT_SPI_CS_L,
    .hspi = &ALT_SPI
};

const struct nk_spiflash_info at25df081 =
{
    .spi_transfer = nk_spi_transfer,
    .spi_ptr = (void *)&at25df081_bus,
    .buffer = spi_buffer,
    .page_size = 128,
    .busy_timeout = 10000,
    .addr_size = 3,
    .n_erase_options = 3,
    .erase_options = {
        { 65536, 0xD8 },
        { 32768, 0x52 },
        { 4096, 0x20 }
    }
};

static int cmd_at25df081(nkinfile_t *args)
{
    return nk_spiflash_command(&at25df081, args);
}

COMMAND(cmd_at25df081,
    ">at25df081                  Read/write memory\n"
    "-at25df081 rd <addr>        Read word\n"
    "-at25df081 wr <addr> <val>  Write word\n"
    "-at25df081 hd <addr> <len>  Hex dump memory\n"
    "-at25df081 hd <addr>        Hex dump 256 bytes\n"
    "-at25df081 crc <addr> <len> Calculate CRC of memory\n"
    "-at25df081 erase <addr> <len>\n"
    "-                           Erase memory\n"
    "-at25df081 fill <addr> <len>\n"
    "-                           Fill memory with pattern\n"
    "-at25df081 fill <addr> <len> <val>\n"
    "-                          Fill memory with constant\n"
)

#endif
