#include <string.h>
#include "nkarch.h"
#include "nkcli.h"
#include "nki2c_eeprom.h"
#include "nki2c.h"

extern uint8_t i2c_buffer[16];

// m24256 I2C EEPROM on X-NUCLEO-EEPRMA2 board

#define M24256_I2C_ADDR 0x55

const struct nk_i2c_eeprom_info m24256 =
{
	.i2c_write = nk_i2c_write,
	.i2c_read = nk_i2c_read,
	.i2c_ptr = &ARD_I2C,

	.i2c_addr = M24256_I2C_ADDR,

	.buffer = i2c_buffer,

	.page_size = 16,
	.busy_timeout = 100,
	.addr_size = 2
};

uint32_t m24256_old_addr;

static int cmd_m24256(nkinfile_t *args)
{
    return nk_i2c_eeprom_command(&m24256, args, &m24256_old_addr);
}

COMMAND(cmd_m24256,
    ">m24256                    Read/write memory\n"
    "-m24256   rd <addr>        Read word\n"
    "-m24256   wr <addr> <val>  Write word\n"
    "-m24256   hd <addr> <len>  Hex dump memory\n"
    "-m24256   hd <addr>        Hex dump 256 bytes\n"
    "-m24256   hd               Hex dump next 256 bytes\n"
    "-m24256   crc <addr> <len> Calculate CRC of memory\n"
    "-m24256   fill <addr> <len>\n"
    "-                          Fill memory with pattern\n"
    "-m24256   fill <addr> <len> <val>\n"
    "-                          Fill memory with constant\n"
)

