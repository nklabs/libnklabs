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

#include "nkcli.h"
#include "nkflash.h"

static uint32_t old_flash_addr;

void flash_hex_dump(uint32_t addr, uint32_t len)
{
    unsigned char buf[256];
    while (len) {
        uint32_t this_page = (addr & ~255);
        uint32_t this_ofst = (addr & 255);
        uint32_t this_len = 256 - this_ofst;
        if (this_len > len)
            this_len = len;

        nk_flash_read(this_page, buf, this_len);
        nk_byte_hex_dump(buf, this_page, this_ofst, this_len);

        addr += this_len;
        len -= this_len;
    }
}

int cmd_flash(nkinfile_t *args)
{
    uint32_t addr;
    uint32_t len;
    uint32_t val;
    if (facmode && nk_fscan(args, "rd %lx ", &addr)) {
        nk_flash_read(addr, &val, 4);
        nk_printf("[%lx] has %x\n", addr, val);
    } else if (facmode && nk_fscan(args, "wr %lx %x ", &addr, &val)) {
        nk_flash_write(addr, &val, 4);
        nk_printf("Wrote %x to [%lx]\n", val, addr);
    } else if (facmode && nk_fscan(args, "hd %lx %x ", &old_flash_addr, &len)) {
        flash_hex_dump(old_flash_addr, len);
	old_flash_addr += len;
    } else if (facmode && nk_fscan(args, "hd %lx ", &old_flash_addr)) {
    	len = 0x100;
        flash_hex_dump(old_flash_addr, len);
	old_flash_addr += len;
    } else if (facmode && nk_fscan(args, "erase %lx %x ", &addr, &len)) {
        nk_flash_erase(addr, len);
    } else if (facmode && nk_fscan(args, "erase %lx ", &addr)) {
    	len = 0x10000;
        nk_flash_erase(addr, len);
    } else {
        nk_printf("Syntax error\n");
    }
    return 0;
}

COMMAND(flash, "flash                     Read/write flash memory")
HELP("flash rd <addr>           Read word")
HELP("flash wr <addr> <val>     Write word")
HELP("flash hd <addr> <len>     Hex dump flash memory")
HELP("flash erase <addr> <len>  Erase flash memory")
