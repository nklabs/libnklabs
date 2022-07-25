// Copyright 2020 NK Labs, LLC

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

// Y-modem CLI command
// Provides receive file handler

#include <string.h>
#include <inttypes.h>
#include "nkcrclib.h"
#include "nkcli.h"
#include "nkdirect.h"
#include "nkmcuflash.h"
#include "nkmcuflash_map.h"
#include "nkymodem.h"

// Packet buffer
static unsigned char packet_buffer[NK_YM_BUFFER_SIZE]; // Enough for STX 01 FE Data[1024] CRC CRC

// Ymodem receive file handler
// Usually set up to write data to flash

static char name_buf[128];
static uint32_t dld_exact; // Exact file size from sender
static int dld_done;

const nk_direct_base_t ymodem_file_fpga =
{
    .area_size = FLASH_FPGA_BUFFER_SIZE,
    .area_base = FLASH_FPGA_BUFFER_MAIN,
    .erase_size = 65536, // Must be at least NK_MCUFLASH_ERASE_SIZE
    .info = NULL,
    .flash_read = (int (*)(const void *, uint32_t, uint8_t *, uint32_t))nk_mcuflash_read, // info, address, data, byte_count
    .flash_erase = (int (*)(const void *info, uint32_t addr, uint32_t size))nk_mcuflash_erase,
    .flash_write = (int (*)(const void *, uint32_t, const uint8_t *, uint32_t))nk_mcuflash_write, // info, address, data, byte_count
    .granularity = 1
};

const nk_direct_base_t ymodem_file_fw0 =
{
    .area_size = FLASH_FIRMWARE_BUFFER_SIZE,
    .area_base = FLASH_FIRMWARE_BUFFER_0,
    .erase_size = 65536, // Must be at least NK_MCUFLASH_ERASE_SIZE
    .info = NULL,
    .flash_read = (int (*)(const void *, uint32_t, uint8_t *, uint32_t))nk_mcuflash_read, // info, address, data, byte_count
    .flash_erase = (int (*)(const void *info, uint32_t addr, uint32_t size))nk_mcuflash_erase,
    .flash_write = (int (*)(const void *, uint32_t, const uint8_t *, uint32_t))nk_mcuflash_write, // info, address, data, byte_count
    .granularity = 1
};

const nk_direct_base_t ymodem_file_fw1 =
{
    .area_size = FLASH_FIRMWARE_BUFFER_SIZE,
    .area_base = FLASH_FIRMWARE_BUFFER_1,
    .erase_size = 65536, // Must be at least NK_MCUFLASH_ERASE_SIZE
    .info = NULL,
    .flash_read = (int (*)(const void *, uint32_t, uint8_t *, uint32_t))nk_mcuflash_read, // info, address, data, byte_count
    .flash_erase = (int (*)(const void *info, uint32_t addr, uint32_t size))nk_mcuflash_erase,
    .flash_write = (int (*)(const void *, uint32_t, const uint8_t *, uint32_t))nk_mcuflash_write, // info, address, data, byte_count
    .granularity = 1
};

nk_direct_t ymodem_file;

int ymodem_recv_file_open(const char *name)
{
    size_t len;
    strcpy(name_buf, name);
    len = strlen(name);
    dld_exact = 0xffffffff;
    if (name[len+1])
    {
        dld_exact = (uint32_t)atoi(name+len+1);
    }

    if (name[0] == 'f' && name[1] == 'p' && name[2] == 'g' && name[3] == 'a') {
        return nk_direct_write_open(&ymodem_file, &ymodem_file_fpga);
    } else {
        if (reg_spictrl & 0x40000000) {
            return nk_direct_write_open(&ymodem_file, &ymodem_file_fw0);
        } else {
            return nk_direct_write_open(&ymodem_file, &ymodem_file_fw1);
        }
    }
}

void ymodem_recv_file_write(unsigned char *buffer, size_t len)
{
    nk_direct_write(&ymodem_file, buffer, len);
}

// Called after a single file has been transferred- do not print anything

void ymodem_recv_file_close()
{
    nk_direct_write_close(&ymodem_file);
    dld_done = 1;
}

// Called after a single file has been canceled- do not print anything

void ymodem_recv_file_cancel()
{
}

// Called after all files successfully transferred: OK to print

void ymodem_recv_all_done()
{
    if (dld_done)
    {
        nk_printf("All done!  filename = '%s', size = %"PRIu32", CRC = %"PRIx32"\n", name_buf, ymodem_file.size, ymodem_file.crc);
        if (ymodem_file.file == &ymodem_file_fw0 || ymodem_file.file == &ymodem_file_fw1) {
            uint32_t buf;
            nk_printf("It's a software image: setting bootloader flag\n");
            // Switch to new bank
            nk_mcuflash_erase(NULL, FLASH_FIRMWARE_FLAG, NK_MCUFLASH_ERASE_SIZE);
            if (reg_spictrl & 0x40000000)
                buf = 0;
            else
                buf = 1;
            nk_mcuflash_write(NULL, FLASH_FIRMWARE_FLAG, (uint8_t *)&buf, 4);
        } else {
            nk_printf("It's an FPGA image\n");
        }
    }
}

// Command line interface

static int cmd_ymodem(nkinfile_t *args)
{
    if (nk_fscan(args, "")) { // Receive a file
        name_buf[0] = 0;
        dld_done = 0;
        nk_yrecv(packet_buffer);
    } else if (nk_fscan(args, "show ")) {
        // State of previous receive
        nk_printf("name = %s\n", name_buf);
        nk_printf("exact = %"PRIu32"\n", dld_exact);
        nk_printf("size = %"PRIu32"\n", ymodem_file.size);
        nk_printf("crc = %"PRIx32"\n", ymodem_file.crc);
        debug_rcv_status();
    } else {
        nk_printf("Syntax error\n");
    }
    
    return 0;
}

COMMAND(cmd_ymodem,
    ">ymodem                    Transfer files over serial\n"
    "-ymodem                    Receive a file\n"
    "-ymodem show               Print receive status\n"
)
