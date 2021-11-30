#include <string.h>
#include "nkarch.h"
#include "nkcli.h"
#include "nkcrclib.h"
#include "nkspiflash.h"

#include "main.h" // For pin definitions

// Proto board has SPI-flash
// Dev board has EEPROM

// SPI-transfer

static void flashio(uint8_t *data, uint32_t len)
{
#ifdef DEV_BOARD
        HAL_GPIO_WritePin(S2LP_SDN_GPIO_Port, S2LP_SDN_Pin, 1); // Bug on older S2LPs- they only tristate MISO when shutdown
#endif
        HAL_GPIO_WritePin(S2LP_CSN_GPIO_Port, S2LP_CSN_Pin, 1); // CS_L
        HAL_GPIO_WritePin(EEPROM_CSN_GPIO_Port, EEPROM_CSN_Pin, 0); // CS_L

	HAL_SPI_TransmitReceive(&s2lp_spi_port, data, data, len, 1000);

        HAL_GPIO_WritePin(EEPROM_CSN_GPIO_Port, EEPROM_CSN_Pin, 1); // CS_L
}

int nk_init_spiflash()
{
	return 0;
}

int spiflash_enabled;

static void write_enable()
{
	uint8_t xfer[1];
	xfer[0] = 0x06;
	flashio(xfer, sizeof(xfer));
	spiflash_enabled = 1;
}

static void busy_wait()
{
	int x;
	for (x = 0; x != 10000; ++x) {
		uint8_t xfer[2];
		xfer[0] = 0x05;
		xfer[1] = 0x00;
		flashio(xfer, sizeof(xfer));
		if ((xfer[1] & 1) == 0) {
			// nk_printf("done after %d loops, %x\n", x, xfer[1]);
			// 540 loops for a 4K erase
			return;
		}
	}
	nk_printf("Timeout!\n");
}

int nk_spiflash_erase(uint32_t address, uint32_t byte_count)
{
#ifdef PROTO_BOARD
	// As long as we're not done..
	while (byte_count) {
		// Try to use largest erase that works..
		if ((address & 65535) == 0 && byte_count >= 65536) {
			uint8_t xfer[4];
			xfer[0] = 0xd8;
			xfer[1] = (uint8_t)(address >> 16);
			xfer[2] = (uint8_t)(address >> 8);
			xfer[3] = (uint8_t)(address);
			nk_printf("  Erase 64K\n");
			write_enable();
			flashio(xfer, sizeof(xfer));
			busy_wait();
			nk_printf("  Erase done\n");
			address += 65536;
			byte_count -= 65536;
		} else if ((address & 32767) == 0 && byte_count >= 32768) {
			uint8_t xfer[4];
			xfer[0] = 0x52;
			xfer[1] = (uint8_t)(address >> 16);
			xfer[2] = (uint8_t)(address >> 8);
			xfer[3] = (uint8_t)(address);
			nk_printf("  Erase 32K\n");
			write_enable();
			flashio(xfer, sizeof(xfer));
			busy_wait();
			nk_printf("  Erase done\n");
			address += 32768;
			byte_count -= 32768;
		} else if ((address & 4095) == 0 && byte_count >= 4096) {
			uint8_t xfer[4];
			xfer[0] = 0x20;
			xfer[1] = (uint8_t)(address >> 16);
			xfer[2] = (uint8_t)(address >> 8);
			xfer[3] = (uint8_t)(address);
			nk_printf("  Erase 4K at %lx\n", address);
			write_enable();
			flashio(xfer, sizeof(xfer));
			busy_wait();
			nk_printf("  done.\n");
			address += 4096;
			byte_count -= 4096;
		} else {
			// None work? Fail..
			break;
		}
	}

	if (byte_count) {
		nk_printf("ERROR: Invalid erase size\n");
		return -1;
	}
#endif
	return 0;
}

int nk_spiflash_write(uint32_t address, uint8_t *data, uint32_t byte_count)
{
	uint8_t xfer[256+4];
	int rtn = 0; // Assume success
	int hdrlen;

	uint32_t page_size = 128;

	// Write a page at a time
	while (byte_count) {
		uint32_t page_offset = (address & (page_size - 1)); // Starting offset within page
		uint32_t page_len = page_size - page_offset; // Up to one page

		if (byte_count < page_len)
			page_len = byte_count;


		xfer[0] = 0x02;
#ifdef DEV_BOARD
		xfer[1] = (uint8_t)(address >> 8);
		xfer[2] = (uint8_t)(address);
		hdrlen = 3;
#else
		xfer[1] = (uint8_t)(address >> 16);
		xfer[2] = (uint8_t)(address >> 8);
		xfer[3] = (uint8_t)(address);
		hdrlen = 4;
#endif
		memcpy(xfer + hdrlen, data, page_len);
		// nk_printf("Write, len = %lu\n", page_len + 4);
		// byte_hex_dump((unsigned long)(xfer), 0, page_len + 4, NULL);
		write_enable();
		flashio(xfer, page_len + hdrlen);
		busy_wait();
		// nk_printf("After write\n");

		byte_count -= page_len;
		address += page_len;
		data += page_len;
	}

	return rtn;
}

int nk_spiflash_read(uint32_t address, uint8_t *data, uint32_t byte_count)
{
	uint8_t xfer[256+4];
	int rtn = 0; // Assume success
	int hdrlen;

	uint32_t page_size = 256;

	// nk_printf("addr=%x, byte_count=%d\n", address, byte_count);

	// Read a page at a time
	while (byte_count) {
		uint32_t page_offset = (address & (page_size - 1)); // Starting offset within page
		uint32_t page_len = page_size - page_offset; // Up to one page

		if (byte_count < page_len)
			page_len = byte_count;

		xfer[0] = 0x03;
#ifdef DEV_BOARD
		xfer[1] = (uint8_t)(address >> 8);
		xfer[2] = (uint8_t)(address);
		hdrlen = 3;
#else
		xfer[1] = (uint8_t)(address >> 16);
		xfer[2] = (uint8_t)(address >> 8);
		xfer[3] = (uint8_t)(address);
		hdrlen = 4;
#endif
//		nk_printf("hdrlen = %d pagelen = %d\n", hdrlen, page_len);
		flashio(xfer, page_len + hdrlen);
		memcpy(data, xfer + hdrlen, page_len);

		byte_count -= page_len;
		address += page_len;
		data += page_len;
	}

	return rtn;
}

static uint32_t old_spiflash_addr;

static void spiflash_hex_dump(uint32_t addr, uint32_t len)
{
    unsigned char buf[256];
    while(len) {
        uint32_t this_page = (addr & ~255U);
        uint32_t this_ofst = (addr & 255U);
        uint32_t this_len = 256U - this_ofst;
        if (this_len > len)
            this_len = len;

        nk_spiflash_read(this_page + this_ofst, buf + this_ofst, this_len);
        nkinfile_t f[1];
        nkinfile_open_mem(f, buf - this_page, 256);
        nk_byte_hex_dump(addr, this_len, f);

        addr += this_len;
        len -= this_len;
    }
}

static uint32_t spiflash_crc(uint32_t addr, uint32_t len)
{
    unsigned char buf[256];
    uint32_t x, crc = 0;
    while(len) {
        uint32_t this_page = (addr & ~255U);
        uint32_t this_ofst = (addr & 255U);
        uint32_t this_len = 256U - this_ofst;
        if (this_len > len)
            this_len = len;

        nk_spiflash_read(this_page, buf, this_len);

        for (x = 0; x != this_len; ++x)
            crc = nk_crc32be_update(crc, buf[x]);

        addr += this_len;
        len -= this_len;
    }
    return crc;
}

static int cmd_spiflash(nkinfile_t *args)
{
    uint32_t addr;
    uint32_t len;
    uint32_t val;
    if (facmode && nk_fscan(args, "rd %lx ", &addr)) {
        nk_spiflash_read(addr, (uint8_t *)&val, 4);
        nk_printf("[%lx] has %lx\n", addr, val);
    } else if (facmode && nk_fscan(args, "wr %lx %lx ", &addr, &val)) {
        nk_spiflash_write(addr, (uint8_t *)&val, 4);
        nk_printf("Wrote %lx to [%lx]\n", val, addr);
    } else if (facmode && nk_fscan(args, "hd %lx %x ", &old_spiflash_addr, &len)) {
        spiflash_hex_dump(old_spiflash_addr, len);
	old_spiflash_addr += len;
    } else if (facmode && nk_fscan(args, "hd %lx ", &old_spiflash_addr)) {
    	len = 0x100;
        spiflash_hex_dump(old_spiflash_addr, len);
	old_spiflash_addr += len;
    } else if (facmode && nk_fscan(args, "crc %lx %lu ", &addr, &len)) {
        nk_printf("Calculate CRC of %lx - %lx\n", addr, addr + len);
        val = spiflash_crc(addr, len);
        nk_printf("CRC is %lx\n", val);
    } else if (facmode && nk_fscan(args, "erase %lx %x ", &addr, &len)) {
    	nk_printf("Erasing %lu bytes...\n", len);
        nk_spiflash_erase(addr, len);
        nk_printf("done.\n");
    } else if (facmode && nk_fscan(args, "erase %lx ", &addr)) {
    	len = 4096;
    	nk_printf("Erasing %lu bytes...\n", len);
        nk_spiflash_erase(addr, len);
        nk_printf("done.\n");
    } else if (facmode && nk_fscan(args, "fill %lx %x ", &addr, &len)) {
        nk_printf("Writing %lu bytes...\n", len);
        nk_spiflash_write(addr, (uint8_t *)0x100, len);
        nk_printf("done.\n");
    } else {
        nk_printf("Syntax error\n");
    }
    return 0;
}

COMMAND(spiflash,
    "spiflash                  Read/write flash memory\n",
    "spiflash rd <addr>        Read word\n"
    "spiflash wr <addr> <val>  Write word\n"
    "spiflash hd <addr> <len>  Hex dump flash memory\n"
    "spiflash crc <addr> <len> Calculate CRC of flash memory\n"
    "spiflash erase <addr> <len>\n"
    "                          Erase flash memory\n"
    "spiflash fill <addr> <len>\n"
    "                          Write flash memory\n",
    ""
)
