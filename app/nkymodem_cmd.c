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
#include "nkcrclib.h"
#include "nkcli.h"
#include "nkchecked.h"
#include "nkspiflash.h"
#include "nkymodem.h"

// Test data for transmit
static char sdata_test[]="This is fake data\n";

// Ymodem receive file handler
// Usually set up to write data to flash

static char name_buf[128];
static uint32_t dld_exact; // Exact file size from sender

extern const struct nk_spiflash_info m95m04;

const nk_checked_base_t ymodem_file_base =
{
    .area_size = 131072,
    .area_base = 0,
    .erase_size = 256,
    .buf_size = 256,
    .info = &m95m04,
    .flash_read = nk_spiflash_read, // info, address, data, byte_count
    .flash_erase = 0,
    .flash_write = nk_spiflash_write // info, address, data, byte_count
};

nk_checked_t ymodem_file;

int ymodem_recv_file_open(char *name)
{
    int len;
    strcpy(name_buf, name);
    len = strlen(name);
    dld_exact = 0xffffffff;
    if (name[len+1])
    {
        dld_exact = atoi(name+len+1);
    }

    nk_checked_write_open(&ymodem_file, &ymodem_file_base);

    return 1;
}

void ymodem_recv_file_write(unsigned char *buffer, int len)
{
    nk_checked_write(&ymodem_file, buffer, len);
}

void ymodem_recv_file_close()
{
    nk_checked_write_close(&ymodem_file);
}

void ymodem_recv_file_cancel()
{
}


// Command line interface

int cmd_ymodem(nkinfile_t *args)
{
    unsigned char ebuf[256];
    if (nk_fscan(args, "")) { // Receive a file
        int sta;
        name_buf[0] = 0;

        sta = nk_yrecv();

        nk_printf("\n");

        switch (sta) {
            case YMODEM_RECV_DONE: {
                nk_printf("Transfer complete: %ld bytes\n", ymodem_file.size);
                break;
            } case YMODEM_RECV_REMOTE_CANCEL: {
                nk_printf("Canceled after %ld bytes\n", ymodem_file.size);
                break;
            } case YMODEM_RECV_OPEN_CANCEL: {
                nk_printf("Canceled after %ld bytes (couldn't open file)\n", ymodem_file.size);
                break;
            } default: {
                nk_printf("YMODEM error code %d after %ld bytes\n", sta, ymodem_file.size);
                break;
            }
        }
    } else if (nk_fscan(args, "send ")) { // Send a file
        nk_ysend_buffer("foo", sdata_test, sizeof(sdata_test));
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
        len = nk_checked_read(&ymodem_file, ebuf, 0);
        nk_printf("read = %d\n", len);
        nk_byte_hex_dump(ebuf, 0, 0, 128);
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
