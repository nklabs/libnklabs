#include <string.h>
#include "nkarch.h"
#include "nkcli.h"
#include "nki2c_eeprom.h"
#include "nki2c.h"

extern uint8_t i2c_buffer[16];

// m24c02 I2C EEPROM on X-NUCLEO-EEPRMA2 board

#define M24C02_I2C_ADDR 0x54

const struct nk_i2c_eeprom_info m24c02 =
{
	.i2c_write = nk_i2c_write,
	.i2c_read = nk_i2c_read,
	.i2c_ptr = &ARD_I2C,

	.i2c_addr = M24C02_I2C_ADDR,

	.buffer = i2c_buffer,

	.page_size = 16,
	.busy_timeout = 100,
	.addr_size = 1
};

uint32_t m24c02_old_addr;

static int cmd_m24c02(nkinfile_t *args)
{
    return nk_i2c_eeprom_command(&m24c02, args, &m24c02_old_addr);
}

COMMAND(m24c02,
    "m24c02                    Read/write memory\n",
    "m24c02   rd <addr>        Read word\n"
    "m24c02   wr <addr> <val>  Write word\n"
    "m24c02   hd <addr> <len>  Hex dump memory\n"
    "m24c02   hd <addr>        Hex dump 256 bytes\n"
    "m24c02   hd               Hex dump next 256 bytes\n"
    "m24c02   crc <addr> <len> Calculate CRC of memory\n"
    "m24c02   fill <addr> <len>\n"
    "                          Fill memory with pattern\n"
    "m24c02   fill <addr> <len> <val>\n"
    "                          Fill memory with constant\n",
    ""
)
