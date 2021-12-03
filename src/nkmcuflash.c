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
        uint32_t this_len = 256U - this_ofst;
        if (this_len > len)
            this_len = len;

        nk_mcuflash_read(this_page + this_ofst, buf + this_ofst, this_len);
        nkinfile_t f[1];
        nkinfile_open_mem(f, buf, 256);
        nk_byte_hex_dump(this_page, this_ofst, this_len, f);

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
        uint32_t this_len = 256U - this_ofst;
        if (this_len > len)
            this_len = len;

        nk_mcuflash_read(this_page, buf, this_len);

        for (x = 0; x != this_len; ++x)
            crc = nk_crc32be_update(crc, buf[x]);

        addr += this_len;
        len -= this_len;
    }
    return crc;
}

static int cmd_mcuflash(nkinfile_t *args)
{
    uint32_t addr;
    uint32_t len;
    uint64_t val;
    if (facmode && nk_fscan(args, "rd %lx ", &addr)) {
        nk_mcuflash_read(addr, (uint8_t *)&val, 8);
        nk_printf("[%lx] has %llx\n", addr, val);
    } else if (facmode && nk_fscan(args, "wr %lx %llx ", &addr, &val)) {
        nk_mcuflash_write(addr, (uint8_t *)&val, 8);
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
        nk_mcuflash_erase(addr, len);
        nk_printf("done.\n");
    } else if (facmode && nk_fscan(args, "erase %lx ", &addr)) {
    	len = FLASH_PAGE_SIZE;
    	nk_printf("Erasing %lu bytes...\n", len);
        nk_mcuflash_erase(addr, len);
        nk_printf("done.\n");
    } else if (facmode && nk_fscan(args, "fill %lx %x ", &addr, &len)) {
        nk_printf("Writing %lu bytes...\n", len);
        nk_mcuflash_write(addr, (uint8_t *)0x100, len);
        nk_printf("done.\n");
    } else {
        nk_printf("Syntax error\n");
    }
    return 0;
}

COMMAND(mcuflash,
    "mcuflash                  Read/write flash memory\n",
    "mcuflash rd <addr>        Read word\n"
    "mcuflash wr <addr> <val>  Write word\n"
    "mcuflash hd <addr> <len>  Hex dump flash memory\n"
    "mcuflash crc <addr> <len> Calculate CRC of flash memory\n"
    "mcuflash erase <addr> <len>\n"
    "                          Erase flash memory\n"
    "mcuflash fill <addr> <len>\n"
    "                          Write flash memory\n",
    ""
)
