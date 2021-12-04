// SPI-flash and SPI-EEPROM driver

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
#include "nkcli.h"
#include "nkcrclib.h"
#include "nkspiflash.h"

int nk_spiflash_write_enable(struct nk_spiflash_info *info)
{
	info->buffer[0] = NK_FLASH_CMD_WRITE_ENABLE;
	return info->spi_transfer(info->spi_ptr, info->buffer, 1);
}

int nk_spiflash_busy_wait(struct nk_spiflash_info *info)
{
	int x;
	int status = 0;
	for (x = 0; x != info->busy_timeout; ++x) {
		info->buffer[0] = NK_FLASH_CMD_READ_STATUS;
		info->buffer[1] = 0;
		status |= info->spi_transfer(info->spi_ptr, info->buffer, 2);
		if ((info->buffer[0] & 1) == 0)
		{
			// nk_printf("done after %d loops, %x\n", x, info->buffer[1]);
			// 540 loops for a 4K erase
			return status;
		}
	}
	nk_printf("Timeout!\n");
	status = -1;
	return status;
}

int nk_spiflash_erase(struct nk_spiflash_info *info, uint32_t address, uint32_t byte_count)
{
	int status = 0;
	if (info->n_erase_options)
	{
		// As long as we're not done..
		while (byte_count) {
			int work = 0;
			int x;
			for (x = 0; x != info->n_erase_options; ++x)
			{
				if ((address & (info->erase_options[x].erase_size - 1)) == 0 && byte_count >= info->erase_options[x].erase_size)
				{
					int y;
					uint32_t addr = address;
					status |= nk_spiflash_write_enable(info);
					info->buffer[0] = info->erase_options[x].erase_cmd;
					for (y = 0; y != info->addr_size; ++y)
					{
						info->buffer[info->addr_size - y] = addr;
						addr >>= 8;
					}
					nk_printf("  Erase addr=%lx size=%lu\n", (unsigned long)address, (unsigned long)info->erase_options[x].erase_size);
					status |= info->spi_transfer(info->spi_ptr, info->buffer, info->addr_size + 1);
					address += info->erase_options[x].erase_size;
					byte_count -= info->erase_options[x].erase_size;
					work = 1;
					break;
				}
			}
			if (!work)
			{
				nk_printf("ERROR: Invalid erase size\n");
				status = -1;
				break;
			}
		}
	}
	return status;
}

int nk_spiflash_write(struct nk_spiflash_info *info, uint32_t address, uint8_t *data, uint32_t byte_count)
{
	int status = 0; // Assume success
	uint32_t page_size = info->page_size;

	// Write a page at a time
	while (byte_count) {
		uint32_t page_offset = (address & (page_size - 1)); // Starting offset within page
		uint32_t transfer_len = page_size - page_offset; // Up to one page
		uint32_t addr = address;
		int y;

		if (byte_count < transfer_len)
			transfer_len = byte_count;

		status |= nk_spiflash_write_enable(info);

		info->buffer[0] = NK_FLASH_CMD_WRITE;
		for (y = 0; y != info->addr_size; ++y)
		{
			info->buffer[info->addr_size - y] = addr;
			addr >>= 8;
		}

		memcpy(info->buffer + 1 + info->addr_size, data, transfer_len);
		// nk_printf("Write, len = %lu\n", transfer_len + 1 + info->addr_size);
		// nk_byte_hex_dump(xfer, 0, 0, transfer_len + 1 + info->addr_size);
		status |= info->spi_transfer(info->spi_ptr, info->buffer, 1 + info->addr_size + transfer_len);
		status |= nk_spiflash_busy_wait(info);
		// nk_printf("After write\n");

		byte_count -= transfer_len;
		address += transfer_len;
		data += transfer_len;
	}

	return status;
}

int nk_spiflash_read(struct nk_spiflash_info *info, uint32_t address, uint8_t *data, uint32_t byte_count)
{
	int status = 0; // Assume success
	uint32_t page_size = info->page_size;

	// Read a page at a time
	while (byte_count) {
		uint32_t page_offset = (address & (page_size - 1)); // Starting offset within page
		uint32_t transfer_len = page_size - page_offset; // Up to one page
		uint32_t addr = address;
		int y;

		if (byte_count < transfer_len)
			transfer_len = byte_count;

		info->buffer[0] = NK_FLASH_CMD_READ;
		for (y = 0; y != info->addr_size; ++y)
		{
			info->buffer[info->addr_size - y] = addr;
			addr >>= 8;
		}

		// nk_printf("Read, len = %lu\n", transfer_len + 1 + info->addr_size);
		// nk_byte_hex_dump(xfer, 0, 0, transfer_len + 1 + info->addr_size);
		status |= info->spi_transfer(info->spi_ptr, info->buffer, 1 + info->addr_size + transfer_len);
		// nk_printf("After read\n");
		memcpy(data, info->buffer + 1 + info->addr_size, transfer_len);

		byte_count -= transfer_len;
		address += transfer_len;
		data += transfer_len;
	}

	return status;
}

void nk_spiflash_hex_dump(struct nk_spiflash_info *info, uint32_t addr, uint32_t len)
{
    unsigned char buf[256];
    while(len) {
        uint32_t this_page = (addr & ~255U);
        uint32_t this_ofst = (addr & 255U);
        uint32_t this_len = 256U - this_ofst;
        if (this_len > len)
            this_len = len;

        nk_spiflash_read(info, this_page + this_ofst, buf + this_ofst, this_len);
        nk_byte_hex_dump(buf, this_page, this_ofst, this_len);

        addr += this_len;
        len -= this_len;
    }
}

uint32_t nk_spiflash_crc(struct nk_spiflash_info *info, uint32_t addr, uint32_t len)
{
    unsigned char buf[256];
    uint32_t x, crc = 0;
    while(len) {
        uint32_t this_page = (addr & ~255U);
        uint32_t this_ofst = (addr & 255U);
        uint32_t this_len = 256U - this_ofst;
        if (this_len > len)
            this_len = len;

        nk_spiflash_read(info, this_page, buf, this_len);

        for (x = 0; x != this_len; ++x)
            crc = nk_crc32be_update(crc, buf[x]);

        addr += this_len;
        len -= this_len;
    }
    return crc;
}

int nk_spiflash_command(struct nk_spiflash_info *info, nkinfile_t *args, uint32_t *old_spiflash_addr)
{
    uint32_t addr;
    uint32_t len;
    uint32_t val;
    if (facmode && nk_fscan(args, "rd %lx ", &addr)) {
        nk_spiflash_read(info, addr, (uint8_t *)&val, 4);
        nk_printf("[%lx] has %lx\n", addr, val);
    } else if (facmode && nk_fscan(args, "wr %lx %lx ", &addr, &val)) {
        nk_spiflash_write(info, addr, (uint8_t *)&val, 4);
        nk_printf("Wrote %lx to [%lx]\n", val, addr);
    } else if (facmode && nk_fscan(args, "hd %lx %x ", old_spiflash_addr, &len)) {
        nk_spiflash_hex_dump(info, *old_spiflash_addr, len);
	*old_spiflash_addr += len;
    } else if (facmode && nk_fscan(args, "hd %lx ", old_spiflash_addr)) {
    	len = 0x100;
        nk_spiflash_hex_dump(info, *old_spiflash_addr, len);
	*old_spiflash_addr += len;
    } else if (facmode && nk_fscan(args, "crc %lx %lu ", &addr, &len)) {
        nk_printf("Calculate CRC of %lx - %lx\n", addr, addr + len);
        val = nk_spiflash_crc(info, addr, len);
        nk_printf("CRC is %lx\n", val);
    } else if (facmode && nk_fscan(args, "erase %lx %x ", &addr, &len)) {
    	nk_printf("Erasing %lu bytes...\n", len);
        nk_spiflash_erase(info, addr, len);
        nk_printf("done.\n");
    } else if (facmode && nk_fscan(args, "erase %lx ", &addr)) {
    	len = 4096;
    	nk_printf("Erasing %lu bytes...\n", len);
        nk_spiflash_erase(info, addr, len);
        nk_printf("done.\n");
    } else if (facmode && nk_fscan(args, "fill %lx %x ", &addr, &len)) {
        nk_printf("Writing %lu bytes...\n", len);
        nk_spiflash_write(info, addr, (uint8_t *)0x100, len);
        nk_printf("done.\n");
    } else {
        nk_printf("Syntax error\n");
    }
    return 0;
}
