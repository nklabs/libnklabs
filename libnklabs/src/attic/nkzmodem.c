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

//#include "lib.h"
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include "lib.h"
#include "zmodem.h"
//#include "crclib.h"

ZModem zinfo[1];
unsigned char inbuf[1024];

int ZXmitStr(unsigned char *s, int len, ZModem *info)
{
    nk_uart_write((char *)s, len);
    return 0;
}

void ZIFlush(ZModem *info)
{
    unsigned char buf[80];
    unsigned len;
    do {
        len = nk_uart_read((char *)buf, sizeof(buf), 100);
    } while (len > 0);
}

void ZOFlush(ZModem *info)
{
    // fflush(stdout);
}

int ZAttn(ZModem *info)
{
    if (info->attn) {
        nk_puts(info->attn);
        nk_puts("\r\n");
    }
    return 0;
}

void ZFlowControl(int onoff, ZModem *info)
{
}

void ZStatus(int type, int value, char *status)
{
}

static FILE fake;

static char name_buf[128];
static int dld_count;
static int dld_limit;
static int dld_base;
static unsigned int dld_crc;

FILE *ZOpenFile(char *name, unsigned long crc, ZModem *info)
{
    strcpy(name_buf, name);
    if (name[0] == 'f' && name[1] == 'p' && name[2] == 'g' && name[3] == 'a') {
        dld_base = FLASH_FPGA_BUFFER;
        dld_limit = FLASH_FPGA_BUFFER_SIZE;
    } else {
        dld_base = FLASH_FIRMWARE_BUFFER;
        dld_limit = FLASH_FIRMWARE_BUFFER_SIZE;
    }
    dld_crc = 0;
    return &fake;
}

int ZWriteFile(unsigned char *buffer, int len, FILE *file, ZModem *info)
{
    int x;

    if (dld_count + len >= dld_limit)
        len = dld_limit - dld_count;

    if (len) { // Ignore extra if data is too large
        for (x = 0; x != len; ++x)
            dld_crc = crc_update(dld_crc, buffer[x]);
        /* Previous sector we wrote to */
        int prev = ((dld_count - 1) & ~(NK_ERASE_SECTOR_SIZE - 1));

        /* Erase sectors if we need to */
        while (prev != ((dld_count + len - 1) & ~(NK_ERASE_SECTOR_SIZE - 1))) {
            prev += NK_ERASE_SECTOR_SIZE;
            nk_flash_erase(prev + dld_base, NK_ERASE_SECTOR_SIZE);
        }

        nk_flash_write(dld_count + dld_base, buffer, len);


        dld_count += len;
    }
    return 0;
}

int ZCloseFile(ZModem *info)
{
    return 0;
}

void ZIdleStr(unsigned char *buffer, int len, ZModem *info)
{
}

// Fake stdin functions for sending

static int (*zgetc)(FILE *f);
int jzgetc(FILE *f) { return (*zgetc)(f); }

static void (*zseek)(FILE *f, size_t offset, int ign);
void jzseek(FILE *f, size_t offset, int ign) { (*zseek)(f, offset, ign); }

static int (*zsize)(FILE *f);
int jzsize(FILE *f) { return (*zsize)(f); }

static void (*zclose)(FILE *f);
void jzclose(FILE *f) { (*zclose)(f); }

static FILE *(*zopen)(const char *name, const char *mode);
FILE *jzopen(const char *name, const char *mode) { return (*zopen)(name, mode); }

int jzread(unsigned char *buffer, int ign, int size, FILE *f) // Used for Ymodem only
{
    return -1;
}

int jzeof(FILE *f) // Used for Ymodem only
{
    return 1;
}

// Test functions

static char sdata_test[]="This is fake data\n";

static char *sdata;
static size_t sdata_offset;
static size_t sdata_size;
static int sdata_eof;

static FILE *tzopen(const char *name, const char *mode)
{
    sdata_offset = 0;
    sdata_eof = 0;
    return &fake;
}

static void tzclose(FILE *f)
{
}

static int tzgetc(FILE *f)
{
    if (sdata_offset == sdata_size) {
        sdata_eof = 1;
        return -1;
    } else {
        unsigned char c;
        c = sdata[sdata_offset];
        sdata_offset++;
        return c;
    }
}

static void tzseek(FILE *f, size_t offset, int ign)
{
    if (offset >= sdata_size)
        sdata_offset = sdata_size;
    else
        sdata_offset = offset;
}

static int tzsize(FILE *f)
{
    return (int)sdata_size;
}

// Send a file using zmodem protocol

void nk_zsend_file(
    const char *name, 
    FILE *(*topen)(const char *name, const char *mode),
    void (*tclose)(FILE *f),
    int (*tgetc)(FILE *f),
    void (*tseek)(FILE *f, size_t offset, int ign),
    int (tsize)(FILE *f)
) {
    // int async;
    // async = async_log_set(0);
    nk_set_uart_mode(1);

    // Install file access functions
    zopen = topen;
    zclose = tclose;
    zgetc = tgetc;
    zseek = tseek;
    zsize = tsize;

    nk_printf("Start ZMODEM receive transfer...\r\n");
    nk_printf("Hit Ctrl-X five times to cancel.\r\n");
    fflush(stdout);

    memset(zinfo, 0, sizeof(zinfo));
    zinfo->packetsize = 1024;
    ZmodemTInit(zinfo);

    ZmodemTFile(name, name, 0, 0, 0, 0, /* Files remaining */ 0, /* Bytes remaining */ 0, zinfo);
    for (;;) {
        int sta = 0;
        int to = 0; // Timeout counter
        do {
            unsigned len = nk_uart_read((char *)inbuf, sizeof(inbuf), 10);
            if (len > 0) {
                sta = ZmodemRcv(inbuf, len, zinfo); // Call whenever we have some data...
                to = 0;
            } else {
                ++to;
                if (to >= zinfo->timeout*100) {
                    sta = ZmodemTimeout(zinfo);
                    to = 0;
                }
            }
        } while (!sta);

        switch (sta) {
            case ZmDone: {
                ZmodemTFinish(zinfo);
                nk_printf("1.. \r\n"); fflush(stdout); // Not sure why this is needed, but it suppresses extra printed junk
                ZIFlush(zinfo);
                nk_printf("2.. \r\n"); fflush(stdout);
                ZIFlush(zinfo);
                nk_printf("3.. \r\n"); fflush(stdout);
                ZIFlush(zinfo);
                nk_printf("4.. \r\n"); fflush(stdout);
                ZIFlush(zinfo);
                nk_printf("All done!\r\n");
                goto done;
            } case ZmErrCancel: {
                ZmodemTFinish(zinfo);
                ZOFlush(zinfo);
                ZIFlush(zinfo);
                nk_printf("Canceled\r\n");
                goto done;
            } default: {
                ZmodemTFinish(zinfo);
                ZOFlush(zinfo);
                ZIFlush(zinfo);
                nk_printf("ZMODEM error code %d\r\n", sta);
                goto done;
            }
        }
    }
    done:
    fflush(stdout);
    nk_set_uart_mode(0);
    // async_log_set(async);
}

void nk_zsend_buffer(const char *name, char *buffer, size_t len)
{
    sdata = buffer;
    sdata_offset = 0;
    sdata_size = len;
    sdata_eof = 0;
    return nk_zsend_file(name, tzopen, tzclose, tzgetc, tzseek, tzsize);
}

int cmd_zmodem(nkinfile_t *args)
{
    if (nk_fscan(args, "")) { // Receive a file
        int sta = 0;
        int to = 0;
        // int async;
        dld_count = 0;
        name_buf[0] = 0;

        // async = async_log_set(0);

        nk_set_uart_mode(1);

        nk_printf("Start ZMODEM send transfer...\r\n");
        nk_printf("Hit Ctrl-X five times to cancel.\r\n");
        fflush(stdout);

        memset(zinfo, 0, sizeof(zinfo));
        zinfo->zrinitflags = (CANFDX | CANOVIO | CANFC32);
        zinfo->packetsize = 128;
        zinfo->bufsize = 0;

        ZmodemRInit(zinfo);

        do {
            unsigned len = nk_uart_read((char *)inbuf, sizeof(inbuf), 100);
            if (len > 0) {
                sta = ZmodemRcv(inbuf, len, zinfo); // Call whenever we have some data...
                to = 0;
            } else {
                ++to;
                if (to >= zinfo->timeout*10) {
                    sta = ZmodemTimeout(zinfo);
                    to = 0;
                }
            }
        } while (!sta);

        nk_set_uart_mode(0);
        // char bf[8];
        // sprintf(bf, "%d", sta);
        // lcd_field(3, 0, 7, bf); lcd_update();

        switch (sta) {
            case ZmDone: {
                if (dld_count == dld_limit)
            nk_printf("File size too large: extra bytes were ignored!\n");
                nk_printf("All done!  filename = '%s', size = %d, CRC = %x\n", name_buf, dld_count, dld_crc);
                if (dld_base == FLASH_FIRMWARE_BUFFER) {
                    nk_printf("It's a software image: setting bootloader flag\n");
                    // Indicate to the bootloader that we have an image
                    nk_flash_erase(FLASH_FIRMWARE_FLAG, NK_ERASE_SECTOR_SIZE);
                    strcpy((char *)inbuf, "ABCD");
                    *(int *)(inbuf + 4) = dld_count;
                    nk_flash_write(FLASH_FIRMWARE_FLAG, inbuf, 8);
                } else {
                    nk_printf("It's an FPGA image\n");
                }
                break;
            } case ZmErrCancel: {
                nk_printf("Canceled after %d bytes %p\n", dld_count, zinfo->buffer);
                break;
            } default: {
                nk_printf("ZMODEM error code %d after %d bytes\n", sta, dld_count);
                break;
            }
        }
        if (zinfo->buffer) {
            free(zinfo->buffer);
            zinfo->buffer = 0;
        }

        // async_log_set(async);
    } else if (nk_fscan(args, "send ")) { // Send a file
        nk_zsend_buffer("foo", sdata_test, sizeof(sdata_test));
    } else {
        nk_printf("Syntax error\n");
    }
    
    return 0;
}

COMMAND(zmodem, "zmodem                    Transfer files over serial")
HELP("zmodem                    Receive a file")
HELP("zmodem send               Send a file")
