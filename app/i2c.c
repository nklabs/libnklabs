#include <string.h>
#include "nkarch.h"
#include "nkcli.h"
#include "nki2c_eeprom.h"
#include "nki2c.h"

uint8_t i2c_buffer[16];

// Raw I2C bus

extern I2C_HandleTypeDef hi2c1;

static int cmd_i2c(nkinfile_t *args)
{
	return nk_i2c_command(&hi2c1, args);
}

COMMAND(i2c,
	"i2c                       I2C access\n",
	"i2c scan                  Scan I2C bus\n"
	"i2c [w AA DD ...] [r AA NN]\n"
	"                          Raw I2C transaction\n",
	""
)

// m24c02 I2C EEPROM on X-NUCLEO-EEPRMA2 board

#define M24C02_I2C_ADDR 0x54

const struct nk_i2c_eeprom_info m24c02 =
{
	.i2c_write = nk_i2c_write,
	.i2c_read = nk_i2c_read,
	.i2c_ptr = &hi2c1,

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

// m24256 I2C EEPROM on X-NUCLEO-EEPRMA2 board

#define M24256_I2C_ADDR 0x55

const struct nk_i2c_eeprom_info m24256 =
{
	.i2c_write = nk_i2c_write,
	.i2c_read = nk_i2c_read,
	.i2c_ptr = &hi2c1,

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

COMMAND(m24256,
    "m24256                    Read/write memory\n",
    "m24256   rd <addr>        Read word\n"
    "m24256   wr <addr> <val>  Write word\n"
    "m24256   hd <addr> <len>  Hex dump memory\n"
    "m24256   hd <addr>        Hex dump 256 bytes\n"
    "m24256   hd               Hex dump next 256 bytes\n"
    "m24256   crc <addr> <len> Calculate CRC of memory\n"
    "m24256   fill <addr> <len>\n"
    "                          Fill memory with pattern\n"
    "m24256   fill <addr> <len> <val>\n"
    "                          Fill memory with constant\n",
    ""
)

// m24m01 I2C EEPROM on X-NUCLEO-EEPRMA2 board

#define M24M01_I2C_ADDR 0x56

const struct nk_i2c_eeprom_info m24m01 =
{
	.i2c_write = nk_i2c_write,
	.i2c_read = nk_i2c_read,
	.i2c_ptr = &hi2c1,

	.i2c_addr = M24M01_I2C_ADDR,

	.buffer = i2c_buffer,

	.page_size = 16,
	.busy_timeout = 100,
	.addr_size = 2
};

uint32_t m24m01_old_addr;

static int cmd_m24m01(nkinfile_t *args)
{
    return nk_i2c_eeprom_command(&m24m01, args, &m24m01_old_addr);
}

COMMAND(m24m01,
    "m24m01                    Read/write memory\n",
    "m24m01   rd <addr>        Read word\n"
    "m24m01   wr <addr> <val>  Write word\n"
    "m24m01   hd <addr> <len>  Hex dump memory\n"
    "m24m01   hd <addr>        Hex dump 256 bytes\n"
    "m24m01   hd               Hex dump next 256 bytes\n"
    "m24m01   crc <addr> <len> Calculate CRC of memory\n"
    "m24m01   fill <addr> <len>\n"
    "                          Fill memory with pattern\n"
    "m24m01   fill <addr> <len> <val>\n"
    "                          Fill memory with constant\n",
    ""
)
