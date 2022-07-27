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
#include "nkchecked.h"
#include "nkspiflash.h"
#include "nkymodem.h"

// Packet buffer
static unsigned char packet_buffer[NK_YM_BUFFER_SIZE]; // Enough for STX 01 FE Data[1024] CRC CRC

// Test data for transmit
static char sdata_test[]="This is fake data\n";

// Ymodem receive file handler
// Usually set up to write data to flash

static char name_buf[128];
static uint32_t dld_exact; // Exact file size from sender
static int dld_done;

extern const struct nk_spiflash_info m95m04;

const nk_checked_base_t ymodem_file_base =
{
    .area_size = 131072,
    .area_base = 0,
    .erase_size = 256,
    .info = &m95m04,
    .flash_read = (int (*)(const void *, uint32_t, uint8_t *, uint32_t))nk_spiflash_read, // info, address, data, byte_count
    .flash_erase = 0,
    .flash_write = (int (*)(const void *, uint32_t, const uint8_t *, uint32_t))nk_spiflash_write, // info, address, data, byte_count
    .granularity = 1
};

nk_checked_t ymodem_file;

static int ymodem_recv_file_open(const char *name)
{
    int len;
    strcpy(name_buf, name);
    len = strlen(name);
    dld_exact = 0xffffffff;
    if (name[len+1])
    {
        dld_exact = (uint32_t)atoi(name+len+1);
    }

    dld_done = 0;
    return nk_checked_write_open(&ymodem_file, &ymodem_file_base);
}

static void ymodem_recv_file_write(unsigned char *buffer, size_t len)
{
    nk_checked_write(&ymodem_file, buffer, len);
}

static void ymodem_recv_file_close()
{
    nk_checked_write_close(&ymodem_file);
    dld_done = 1;
}

static void ymodem_recv_file_cancel()
{
}

// Called after all files successfully transferred: OK to print

static void ymodem_recv_all_done()
{
    if (dld_done)
    {
        nk_printf("All done!  filename = '%s', size = %"PRIu32", CRC = %"PRIx32"\n", name_buf, ymodem_file.size, ymodem_file.crc);
    }
}

const nk_yrecv_struct_t nk_yrecv_str =
{
    .packet_buffer = packet_buffer,
    .xname = "anonymous\0",
    .open = ymodem_recv_file_open,
    .write = ymodem_recv_file_write,
    .close = ymodem_recv_file_close,
    .cancel = ymodem_recv_file_cancel,
    .all_done = ymodem_recv_all_done
};

// Command line interface

int cmd_ymodem(nkinfile_t *args)
{
    unsigned char ebuf[256];
    if (nk_fscan(args, "")) { // Receive a file
        name_buf[0] = 0;
        nk_yrecv(&nk_yrecv_str);
    } else if (nk_fscan(args, "send ")) { // Send a file
        static nkinfile_t ysend_file; // Important! ysend_file must stay around after cmd_ymodem returns
        nkinfile_open_mem(&ysend_file, sdata_test, sizeof(sdata_test));
        nk_ysend_file(packet_buffer, &ysend_file, "foo", sizeof(sdata_test));
    } else if (nk_fscan(args, "show ")) {
        // State of previous receive
        nk_printf("name = %s\n", name_buf);
        nk_printf("exact = %lu\n", dld_exact);
        nk_printf("size = %lu\n", ymodem_file.size);
        nk_printf("crc = %lx\n", ymodem_file.crc);
        debug_rcv_status();
    } else if (nk_fscan(args, "write ")) { // Write test file to flash
        int rtn;
        rtn = nk_checked_write_open(&ymodem_file, &ymodem_file_base);
        if (rtn)
            nk_printf("write_open = %d\n", rtn);
        rtn = nk_checked_write(&ymodem_file, (unsigned char *)"Hello, world!\n", 14);
        if (rtn)
            nk_printf("write = %d\n", rtn);
        rtn = nk_checked_write_close(&ymodem_file);
        if (rtn)
            nk_printf("write_close = %d\n", rtn);
        nk_printf("write done!\n");
    } else if (nk_fscan(args, "cat ")) { // Print test file
        int rtn;
        size_t len;
        rtn = nk_checked_read_open(&ymodem_file, &ymodem_file_base, ebuf, sizeof(ebuf));
        if (rtn)
            nk_printf("read_open = %d\n", rtn);
        nk_printf("size = %lu\n", ymodem_file.size);
        nk_printf("crc = %lx\n", ymodem_file.crc);
        len = nk_checked_read(&ymodem_file, 0, ebuf, sizeof(ebuf));
        nk_printf("read = %d\n", len);
        nk_byte_hex_dump(ebuf, 0, 0, len);
    } else {
        nk_printf("Syntax error\n");
    }
    
    return 0;
}

COMMAND(cmd_ymodem,
    ">ymodem                    Transfer files over serial\n"
    "-ymodem                    Receive a file\n"
    "-ymodem send               Send a file\n"
)
