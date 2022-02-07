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

#include <string.h>
#include "nkreadline.h"
#include "nkcli.h"
 
// Control echoing

static int cmd_echo(nkinfile_t *args)
{
    if (nk_fscan(args, "on ")) {
        nk_set_echo(1);
        nk_printf("Echoing is enabled\n");
    } else if (nk_fscan(args, "off ")) {
        nk_set_echo(0);
        nk_printf("Echoing is disabled\n");
    } else if (nk_fscan(args, "")) {
        if (nk_get_echo()) 
            nk_printf("Echoing is enabled\n");
        else
            nk_printf("Echoing is disabled\n");
    } else {
        nk_printf("Syntax error\n");
    }
    return 0;
}

COMMAND(cmd_echo,
    ">echo                      Control command echoing\n"
    "-echo on                   Enable echoing\n"
    "-echo off                  Disable echoing\n"
    "-echo                      Show current echo mode\n"
)

// Read/Write/hex-dump memory

static int cmd_mem(nkinfile_t *args)
{
    uint32_t addr;
    uint32_t len;
    uint32_t val;
    if (facmode && nk_fscan(args, "rd %lx ", &addr))
    {
        nk_printf("[%lx] has %lx\n", addr, *(uint32_t *)addr);
    }
    else if (facmode && nk_fscan(args, "wr %lx %x ", &addr, &val))
    {
        *(uint32_t *)addr = val;
        nk_printf("Wrote %lx to [%lx]\n", val, addr);
    }
    else if (facmode && nk_fscan(args, "hd %lx %x ", &addr, &len))
    {
	nk_byte_hex_dump((unsigned char *)0, 0, addr, len);
    }
    else if (facmode && nk_fscan(args, "hd %lx ", &addr))
    {
    	len = 0x100;
	nk_byte_hex_dump((unsigned char *)0, 0, addr, len);
    }
    else
    {
        nk_printf("Syntax error\n");
    }
    return 0;
}

COMMAND(cmd_mem,
    ">mem                       Read/write memory\n"
    "-mem rd <addr>             Read 32-bit word from address\n"
    "-mem wr <addr> <data>      Write 32-bit word to address\n"
    "-mem hd <addr> <len>       Hex dump memory\n"
    "-mem hd <addr>             Hex dump 256 bytes\n"
)

// Reboot the system

static int cmd_reboot(nkinfile_t *args)
{
        if (nk_fscan(args, "")) {
            nk_printf("Rebooting...\n");
            nk_udelay(2000);
            reboot();
        } else {
            nk_printf("Syntax error\n");
        }
        return 0;
}


COMMAND(cmd_reboot,
    ">reboot                    Reboot system\n"
    "-reboot                    Reboot system\n"
)
