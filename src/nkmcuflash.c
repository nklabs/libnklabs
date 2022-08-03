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
#include "nkcli.h"
#include "nkcrclib.h"
#include "nkmcuflash.h"

static uint32_t old_mcuflash_addr;

static void flash_hex_dump(uint32_t addr, uint32_t len)
{
    unsigned char buf[256];
    while(len) {
        uint32_t this_page = (addr & ~255U);
        uint32_t this_ofst = (addr & 255U);
        size_t this_len = (size_t)(256U - this_ofst);
        if (this_len > len)
            this_len = (size_t)len;

        nk_mcuflash_read(NULL, this_page + this_ofst, buf + this_ofst, this_len);
        nk_byte_hex_dump(buf, this_page, this_ofst, this_len);

        addr += this_len;
        len -= this_len;
    }
}

static uint32_t flash_crc(uint32_t addr, uint32_t len)
{
    unsigned char buf[256];
    uint32_t x, crc = 0;
    while(len) {
        uint32_t this_page = (addr & ~255U);
        uint32_t this_ofst = (addr & 255U);
        size_t this_len = (size_t)(256U - this_ofst);
        if (this_len > len)
            this_len = (size_t)len;

        nk_mcuflash_read(NULL, this_page, buf, this_len);

        for (x = 0; x != this_len; ++x)
            crc = nk_crc32be_update(crc, buf[x]);

        addr += this_len;
        len -= this_len;
    }
    return crc;
}

static int cmd_mcuflash(nkinfile_t *args)
{
    int status = 0;
    uint32_t addr;
    uint32_t len;
    uint64_t val;
    uint8_t val8;
    if (facmode && nk_fscan(args, "rd %lx ", &addr)) {
        nk_mcuflash_read(NULL, addr, (uint8_t *)&val, 8);
        nk_printf("[%lx] has %llx\n", addr, val);
    } else if (facmode && nk_fscan(args, "wr %lx %llx ", &addr, &val)) {
        nk_mcuflash_write(NULL, addr, (uint8_t *)&val, 8);
        nk_printf("Wrote %llx to [%lx]\n", val, addr);
    } else if (facmode && nk_fscan(args, "hd %lx %x ", &old_mcuflash_addr, &len)) {
        flash_hex_dump(old_mcuflash_addr, len);
	old_mcuflash_addr += len;
    } else if (facmode && nk_fscan(args, "hd %lx ", &old_mcuflash_addr)) {
    	len = 0x100;
        flash_hex_dump(old_mcuflash_addr, len);
	old_mcuflash_addr += len;
    } else if (facmode && nk_fscan(args, "crc %lx %lu ", &addr, &len)) {
        nk_printf("Calculate CRC of %lx - %lx\n", addr, addr + len);
        addr = flash_crc(addr, len);
        nk_printf("CRC is %lx\n", addr);
    } else if (facmode && nk_fscan(args, "erase %lx %x ", &addr, &len)) {
    	nk_printf("Erasing %lu bytes...\n", len);
        nk_mcuflash_erase(NULL, addr, len);
        nk_printf("done.\n");
    } else if (facmode && nk_fscan(args, "erase %lx ", &addr)) {
#ifdef FLASH_PAGE_SIZE
    	len = FLASH_PAGE_SIZE;
    	nk_printf("Erasing %lu bytes...\n", len);
        nk_mcuflash_erase(NULL, addr, len);
        nk_printf("done.\n");
#else
        nk_printf("Ooops.\n");
#endif
    } else if (facmode && nk_fscan(args, "fill %lx %x ", &addr, &len)) {
    	uint8_t buf[16];
    	uint8_t x = 0x10;
        nk_printf("Writing %lu bytes...\n", len);
    	while (len)
    	{
    		size_t th;
    		int n;
    		for (n = 0; n != 16; ++n)
    			buf[n] = x++;
    		if (len >= 16)
    			th = 16;
		else
			th = (size_t)len;
		status |= nk_mcuflash_write(NULL, addr, buf, th);
		if (status)
			break;
		len -= th;
		addr += th;
    	}
        nk_printf("done.\n");
    } else if (facmode && nk_fscan(args, "fill %lx %x %hhx ", &addr, &len, &val8)) {
    	uint8_t buf[16];
    	memset(buf, val8, sizeof(buf));
        nk_printf("Writing %lu bytes...\n", len);
    	while (len)
    	{
    		size_t th;
    		if (len >= 16)
    			th = 16;
		else
			th = (size_t)len;
		status |= nk_mcuflash_write(NULL, addr, buf, th);
		if (status)
			break;
		len -= th;
		addr += th;
    	}
        nk_printf("done.\n");
    } else {
        nk_printf("Syntax error\n");
    }
    if (status)
    	nk_printf("Flash error\n");
    return 0;
}

COMMAND(cmd_mcuflash,
    ">mcuflash                  Read/write flash memory\n"
    "-mcuflash rd <addr>        Read word\n"
    "-mcuflash wr <addr> <val>  Write word\n"
    "-mcuflash hd <addr> <len>  Hex dump flash memory\n"
    "-mcuflash crc <addr> <len> Calculate CRC of flash memory\n"
    "-mcuflash erase <addr> <len>\n"
    "-                          Erase flash memory\n"
    "-mcuflash fill <addr> <len>\n"
    "-                          Fill flash memory with pattern\n"
    "-mcuflash fill <addr> <len> <val>\n"
    "-                          Fill flash memory with byte\n"
)
