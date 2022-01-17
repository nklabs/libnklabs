#include <string.h>
#include "nkarch.h"
#include "nkcli.h"
#include "i2c.h"

uint8_t i2c_buffer[16];

// Arduino pin I2C bus

const nk_i2c_bus_t ard_i2c_bus =
{
	.i2c_write = nk_hal_i2c_write,
	.i2c_write_nostop = nk_hal_i2c_write_nostop,
	.i2c_read = nk_hal_i2c_read,
	.i2c_ptr = &ARD_I2C
};

// I2C bus CLI command

static int cmd_i2c(nkinfile_t *args)
{
	return nk_i2c_command(&ard_i2c_bus, args);
}

COMMAND(cmd_i2c,
	">i2c                       I2C access\n"
	"-i2c scan                  Scan I2C bus\n"
	"-i2c [w AA DD ...] [r AA NN]\n"
	"-                          Raw I2C transaction\n"
)
