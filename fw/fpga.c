// Memory mapped FPGA access

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
#include "fpga.h"
#include "tunnel_fpga.h"

int fpga_wr(uint32_t reg, uint32_t val)
{
        *(volatile uint32_t *)(FPGA_BASE + reg) = val;
        return 0;
}

int fpga_rd(uint32_t reg, uint32_t *val)
{
        *val = *(volatile uint32_t *)(FPGA_BASE + reg);
        return 0;
}

static int cmd_fpga(nkinfile_t *args)
{
        int x;

        uint32_t      reg_addr;
        uint32_t      reg_data;
        int           transferOK;

        if (facmode && nk_fscan(args, "wr %x %x ", &reg_addr, &reg_data))
        {
                transferOK = fpga_wr(reg_addr, reg_data);
                if (!transferOK)
                    nk_printf("Wrote %lx to register %lx\n", reg_data, reg_addr);
                else
                    nk_printf("Error in fpga_spi_write_reg\n");
                return 0;
        }
        else if (facmode && nk_fscan(args, "rd %x %e", &reg_addr)) {
                transferOK = fpga_rd(reg_addr, &reg_data);
                if (transferOK) {
                        nk_printf("Error in fpga_spi_read_reg\n");
                        return -1;
                }
                nk_printf("Read %lx from register %lx\n", reg_data, reg_addr);
                return 0;
        }
        else if (facmode && nk_fscan(args, "")) {
            for (x = 0; fpga_regtable[x].name; ++x) {
                    int y;
                    if (fpga_regtable[x].addr == 0xffff) { // Section header
                            nk_printf("\n%s: %s\n", fpga_regtable[x].name, fpga_regtable[x].comment);
                    } else { // Register
                            uint32_t val;
                            fpga_rd(fpga_regtable[x].addr, &val);
                            nk_printf("\t[0x%x] %s[%d:%d] = 0x%lx %s\n", fpga_regtable[x].addr, fpga_regtable[x].name, fpga_regtable[x].len - 1, 0, val, fpga_regtable[x].comment);
                            for (y = x; fpga_regtable[y + 1].name && fpga_regtable[y + 1].addr == 0x01;) {
                                    ++y;
                                    nk_printf("\t\t%s[%d:%d] = %s.%s = 0x%lx %s\n",
                                            fpga_regtable[x].name,
                                            fpga_regtable[y].len + fpga_regtable[y].ofst - 1,
                                            fpga_regtable[y].ofst,
                                            fpga_regtable[x].name,
                                            fpga_regtable[y].name,
                                            (val >> fpga_regtable[y].ofst) & ((1U << fpga_regtable[y].len) - 1U),
                                            fpga_regtable[y].comment
                                    );
                            }
                            x = y;
                    }
            }
        } else {
            nk_printf("Syntax error\n");
        }
        return 0;
}

COMMAND(cmd_fpga,
    ">fpga                      Access FPGA\n"
    "-fpga rd <xx>              Read FPGA register\n"
    "-fpga wr <xx> <dd>         Write FPGA register\n"
    "-fpga                      Read all FPGA registers\n"
)

void nk_init_fpga()
{
}

static int sdram_rd_wait()
{
    int timeout;
    for (timeout = 0; timeout != 1000; ++timeout)
    {
        uint32_t val;
        fpga_rd(FPGAREG_SDRAM_ADDR, &val);
        if (val & SDRAM_READ_DONE)
            return 0;
    }
    nk_printf("Read timeout!\n");
    return -1;
}

static int sdram_ack_wait()
{
    int timeout;
    for (timeout = 0; timeout != 1000; ++timeout)
    {
        uint32_t val;
        fpga_rd(FPGAREG_SDRAM_ADDR, &val);
        if (val & SDRAM_REQ_DONE)
            return 0;
    }
    nk_printf("Ack timeout!\n");
    return -1;
}

static void sdram_read(uint32_t addr, uint8_t *buf, uint32_t len)
{
    addr >>= 3;
    while (len)
    {
        fpga_wr(FPGAREG_SDRAM_ADDR, addr);
        sdram_rd_wait();
        fpga_rd(FPGAREG_SDRAM_LOW, (uint32_t *)buf);
        fpga_rd(FPGAREG_SDRAM_HIGH, 1 + (uint32_t *)buf);
        len -= 8;
        buf += 8;
        addr += 1;
    }
}

static void sdram_hex_dump(uint32_t addr, uint32_t len)
{
    unsigned char buf[256];
    while(len) {
        uint32_t this_page = (addr & ~255U);
        uint32_t this_ofst = (addr & 255U);
        uint32_t this_len = 256U - this_ofst;
        if (this_len > len)
            this_len = len;

        sdram_read(this_page + this_ofst, buf + this_ofst, this_len);

        nk_byte_hex_dump(buf, this_page, this_ofst, this_len);

        addr += this_len;
        len -= this_len;
    }
}

static int cmd_sdram(nkinfile_t *args)
{
    uint32_t addr;
    uint32_t len;
    uint64_t data;
    if (nk_fscan(args, "rd %x ", &addr)) {
        addr >>= 3;
        fpga_wr(FPGAREG_SDRAM_ADDR, addr);
        sdram_rd_wait();
        fpga_rd(FPGAREG_SDRAM_LOW, (uint32_t *)&data);
        fpga_rd(FPGAREG_SDRAM_HIGH, 1 + (uint32_t *)&data);
        nk_printf("Read %llx from %lx\n", data, addr);
    } else if (nk_fscan(args, "wr %x %llx ", &addr, &data)) {
        addr >>= 3;
        fpga_wr(FPGAREG_SDRAM_LOW, (uint32_t)data);
        fpga_wr(FPGAREG_SDRAM_HIGH, (uint32_t)(data >> 32));
        fpga_wr(FPGAREG_SDRAM_ADDR, addr | SDRAM_WRITE);
        sdram_ack_wait();
        nk_printf("Wrote %llx to %lx\n", data, addr);
    } else if (nk_fscan(args, "hd %x ", &addr)) {
        sdram_hex_dump(addr, 0x100);
    } else if (nk_fscan(args, "hd %x %x ", &addr, &len)) {
        sdram_hex_dump(addr, len);
    } else if (nk_fscan(args, "test ")) {
        uint32_t limit = 64*1024*64; // 64*1024*1024
        uint64_t val;
        uint32_t x;
        nk_printf("Filling...\n");
        data = 0x10;
        for (x = 0; x != limit; x += 8) {
            fpga_wr(FPGAREG_SDRAM_LOW, (uint32_t)data);
            fpga_wr(FPGAREG_SDRAM_HIGH, (uint32_t)(data >> 32));
            fpga_wr(FPGAREG_SDRAM_ADDR, (x >> 3) | SDRAM_WRITE);
            sdram_ack_wait();
            ++data;
        }
        nk_printf("Done.\n");
        nk_printf("Checking...\n");
        data = 0x10;
        for (x = 0; x != limit; x += 8) {
            fpga_wr(FPGAREG_SDRAM_ADDR, x >> 3);
            sdram_rd_wait();
            fpga_rd(FPGAREG_SDRAM_LOW, (uint32_t *)&val);
            fpga_rd(FPGAREG_SDRAM_HIGH, 1 + (uint32_t *)&val);
            if (val != data) {
                nk_printf("%lx: expected %llx, got %llx\n", x, data, val);
            }
            ++data;
        }
        nk_printf("Done.\n");
    } else if (nk_fscan(args, "testrd ")) {
        uint32_t limit = 64*1024*64; // 64*1024*1024
        uint64_t val;
        uint32_t x;
        nk_printf("Checking...\n");
        data = 0x10;
        for (x = 0; x != limit; x += 8) {
            fpga_wr(FPGAREG_SDRAM_ADDR, x >> 3);
            sdram_rd_wait();
            fpga_rd(FPGAREG_SDRAM_LOW, (uint32_t *)&val);
            fpga_rd(FPGAREG_SDRAM_HIGH, 1 + (uint32_t *)&val);
            if (val != data) {
                nk_printf("%lx: expected %llx, got %llx\n", x, data, val);
            }
            ++data;
        }
        nk_printf("Done.\n");
    } else {
        nk_printf("Syntax error\r\n");
    }
    return 0;
}

COMMAND(cmd_sdram,
    ">sdram                     Access SDRAM\n"
    "-sdram rd <xx>             Read FPGA register\n"
    "-sdram wr <xx> <dd>        Write FPGA register\n"
    "-sdram hd <xx>             Read all FPGA registers\n"
    "-sdram test                Memory test\n"
)
