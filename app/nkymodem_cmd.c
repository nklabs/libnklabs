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
#include "nkymodem.h"

// Test data for transmit
static char sdata_test[]="This is fake data\n";

// Ymodem receive file handler
// Usually set up to write data to flash

static char name_buf[128];
static uint32_t dld_count; // Size of received file
static uint32_t dld_limit; // Space available for this file in local memory
static uint32_t dld_base; // Where file should be stored
static uint32_t dld_exact; // Exact file size from sender
static unsigned int dld_crc; // CRC of received file

static int isopen;

int ymodem_recv_file_open(char *name)
{
    int len;
    isopen = 1;
    strcpy(name_buf, name);
    len = strlen(name);
    dld_exact = 0xffffffff;
    if (name[len+1])
    {
        dld_exact = atoi(name+len+1);
    }

#if 0
    if (name[0] == 'f' && name[1] == 'p' && name[2] == 'g' && name[3] == 'a') {
        dld_base = FLASH_FPGA_BUFFER_MAIN;
        dld_limit = FLASH_FPGA_BUFFER_SIZE;
    } else {
        if (reg_spictrl & 0x40000000)
            dld_base = FLASH_FIRMWARE_BUFFER_0;
        else
            dld_base = FLASH_FIRMWARE_BUFFER_1;
        dld_limit = FLASH_FIRMWARE_BUFFER_SIZE;
    }
#endif
    dld_base = 0;
    dld_limit = 131072;
    dld_crc = 0;
    return 1;
}

void ymodem_recv_file_write(unsigned char *buffer, int len)
{
    int x;

    if (dld_exact != 0xFFFFFFFF && dld_count + len >= dld_exact)
        len = dld_exact - dld_count;

    if (dld_count + len >= dld_limit)
        len = dld_limit - dld_count;


    if (len) { // Ignore extra if data is too large
        for (x = 0; x != len; ++x)
            dld_crc = nk_crc32be_update(dld_crc, buffer[x]);
#if 0
        /* Previous sector we wrote to */
        int prev = ((dld_count - 1) & ~(NK_ERASE_SECTOR_SIZE - 1));

        /* Erase sectors if we need to */
        while (prev != ((dld_count + len - 1) & ~(NK_ERASE_SECTOR_SIZE - 1))) {
            prev += NK_ERASE_SECTOR_SIZE;
            //nk_flash_erase(prev + dld_base, NK_ERASE_SECTOR_SIZE);
        }

        //nk_flash_write(dld_count + dld_base, buffer, len);
#endif

        dld_count += len;
    }
}

void ymodem_recv_file_close()
{
    isopen = 0;
#if 0
    if (dld_count == dld_limit)
        nk_printf("File size too large: extra bytes were ignored!\n");
    nk_printf("All done!  filename = '%s', size = %lu, CRC = %x\n", name_buf, dld_count, dld_crc);
    if (dld_base == FLASH_FIRMWARE_BUFFER_0 || dld_base == FLASH_FIRMWARE_BUFFER_1) {
        nk_printf("It's a software image: setting bootloader flag\n");
        // Switch to new bank
        //nk_flash_erase(FLASH_FIRMWARE_FLAG, NK_ERASE_SECTOR_SIZE);
        //if (reg_spictrl & 0x40000000)
        //    *(int *)(packet_buf) = 0;
        //else
        //    *(int *)(packet_buf) = 1;
        //nk_flash_write(FLASH_FIRMWARE_FLAG, packet_buf, 4);
    } else {
        nk_printf("It's an FPGA image\n");
    }
#endif
}

void ymodem_recv_file_cancel()
{
}


// Command line interface

int cmd_ymodem(nkinfile_t *args)
{
    if (nk_fscan(args, "")) { // Receive a file
        int sta;
        dld_count = 0;
        name_buf[0] = 0;
        sta = nk_yrecv();

        nk_printf("\n");

        switch (sta) {
            case YMODEM_RECV_DONE: {
                nk_printf("Transfer complete: %ld bytes\n", dld_count);
                break;
            } case YMODEM_RECV_REMOTE_CANCEL: {
                nk_printf("Canceled after %ld bytes\n", dld_count);
                break;
            } case YMODEM_RECV_OPEN_CANCEL: {
                nk_printf("Canceled after %ld bytes (couldn't open file)\n", dld_count);
                break;
            } default: {
                nk_printf("YMODEM error code %d after %ld bytes\n", sta, dld_count);
                break;
            }
        }
    } else if (nk_fscan(args, "send ")) { // Send a file
        nk_ysend_buffer("foo", sdata_test, sizeof(sdata_test));
    } else if (nk_fscan(args, "show ")) {
        // State of previous receive
        nk_printf("name = %s\n", name_buf);
        nk_printf("isopen = %d\n", isopen);
        nk_printf("dld_count = %lu\n", dld_count);
        nk_printf("dld_crc = %x\n", dld_crc);
        debug_rcv_status();
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
