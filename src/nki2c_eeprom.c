// I2C EEPROM driver

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
#include "nki2c_eeprom.h"

int nk_i2c_eeprom_busy_wait(const struct nk_i2c_eeprom_info *info)
{
	int x;
	for (x = 0; x != info->busy_timeout; ++x) {
		if (!nk_i2c_write(info->dev, 0, info->buffer))
			return 0;
	}
	// nk_printf("Timeout!\n");
	return -1;
}

int nk_i2c_eeprom_write(const struct nk_i2c_eeprom_info *info, uint32_t address, uint8_t *data, uint32_t byte_count)
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

		for (y = 0; y != info->addr_size; ++y)
		{
			info->buffer[info->addr_size - y - 1] = addr;
			addr >>= 8;
		}

		memcpy(info->buffer + info->addr_size, data, transfer_len);
		// nk_printf("Write, len = %lu\n", transfer_len + info->addr_size);
		// nk_byte_hex_dump(info->buffer, 0, 0, transfer_len + info->addr_size);
		status |= nk_i2c_write(info->dev, info->addr_size + transfer_len, info->buffer);
		if (status)
			break;
		status |= nk_i2c_eeprom_busy_wait(info);
		if (status)
			break;
		// nk_printf("After write\n");

		byte_count -= transfer_len;
		address += transfer_len;
		data += transfer_len;
	}

	return status;
}

int nk_i2c_eeprom_read(const struct nk_i2c_eeprom_info *info, uint32_t address, uint8_t *data, uint32_t byte_count)
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

		for (y = 0; y != info->addr_size; ++y)
		{
			info->buffer[info->addr_size - y - 1] = addr;
			addr >>= 8;
		}

		status |= nk_i2c_write(info->dev, info->addr_size, info->buffer);
		if (status)
			break;
		status |= nk_i2c_read(info->dev, transfer_len, data);
		if (status)
			break;

		byte_count -= transfer_len;
		address += transfer_len;
		data += transfer_len;
	}

	return status;
}

void nk_i2c_eeprom_hex_dump(const struct nk_i2c_eeprom_info *info, uint32_t addr, uint32_t len)
{
    unsigned char buf[256];
    while(len) {
        uint32_t this_page = (addr & ~255U);
        uint32_t this_ofst = (addr & 255U);
        uint32_t this_len = 256U - this_ofst;
        if (this_len > len)
            this_len = len;

        if (nk_i2c_eeprom_read(info, this_page + this_ofst, buf + this_ofst, this_len))
	{
		nk_printf("I2C error\n");
		break;
	}
        nk_byte_hex_dump(buf, this_page, this_ofst, this_len);

        addr += this_len;
        len -= this_len;
    }
}

uint32_t nk_i2c_eeprom_crc(const struct nk_i2c_eeprom_info *info, uint32_t addr, uint32_t len)
{
    unsigned char buf[256];
    uint32_t x, crc = 0;
    while(len) {
        uint32_t this_page = (addr & ~255U);
        uint32_t this_ofst = (addr & 255U);
        uint32_t this_len = 256U - this_ofst;
        if (this_len > len)
            this_len = len;

        if (nk_i2c_eeprom_read(info, this_page, buf, this_len))
	{
		nk_printf("I2C error\n");
		break;
	}

        for (x = 0; x != this_len; ++x)
            crc = nk_crc32be_update(crc, buf[x]);

        addr += this_len;
        len -= this_len;
    }
    return crc;
}

int nk_i2c_eeprom_command(const struct nk_i2c_eeprom_info *info, nkinfile_t *args)
{
    int status = 0;
    uint32_t addr;
    uint32_t len;
    uint32_t val;
    if (facmode && nk_fscan(args, "rd %lx ", &addr))
    {
        status |= nk_i2c_eeprom_read(info, addr, (uint8_t *)&val, 4);
        nk_printf("[%lx] has %lx\n", addr, val);
    }
    else if (facmode && nk_fscan(args, "wr %lx %lx ", &addr, &val))
    {
        status |= nk_i2c_eeprom_write(info, addr, (uint8_t *)&val, 4);
        nk_printf("Wrote %lx to [%lx]\n", val, addr);
    }
    else if (facmode && nk_fscan(args, "hd %lx %x ", &addr, &len))
    {
        nk_i2c_eeprom_hex_dump(info, addr, len);
    }
    else if (facmode && nk_fscan(args, "hd %lx ", &addr))
    {
    	len = 0x100;
        nk_i2c_eeprom_hex_dump(info, addr, len);
    }
    else if (facmode && nk_fscan(args, "crc %lx %lu ", &addr, &len))
    {
        nk_printf("Calculate CRC of %lx - %lx\n", addr, addr + len);
        val = nk_i2c_eeprom_crc(info, addr, len);
        nk_printf("CRC is %lx\n", val);
    }
    else if (facmode && nk_fscan(args, "fill %lx %x ", &addr, &len))
    {
    	uint8_t buf[16];
    	uint8_t x = 0x10;
        nk_printf("Writing %lu bytes...\n", len);
    	while (len)
    	{
    		int th;
    		int n;
    		for (n = 0; n != 16; ++n)
    			buf[n] = x++;
    		if (len >= 16)
    			th = 16;
		else
			th = len;
		status |= nk_i2c_eeprom_write(info, addr, buf, th);
		if (status)
			break;
		len -= th;
		addr += th;
    	}
        nk_printf("done.\n");
    }
    else if (facmode && nk_fscan(args, "fill %lx %x %x ", &addr, &len, &val))
    {
    	uint8_t buf[16];
    	memset(buf, val, sizeof(buf));
        nk_printf("Writing %lu bytes...\n", len);
    	while (len)
    	{
    		int th;
    		if (len >= 16)
    			th = 16;
		else
			th = len;
		status |= nk_i2c_eeprom_write(info, addr, buf, th);
		if (status)
			break;
		len -= th;
		addr += th;
    	}
        nk_printf("done.\n");
    }
    else
    {
        nk_printf("Syntax error\n");
    }
    if (status)
    	nk_printf("I2C error\n");
    return 0;
}
