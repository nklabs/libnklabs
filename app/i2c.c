#include <string.h>
#include "nkarch.h"
#include "nkcli.h"
#include "nki2c_eeprom.h"
#include "nki2c.h"

uint8_t i2c_buffer[16];

// Raw I2C bus

static int cmd_i2c(nkinfile_t *args)
{
	return nk_i2c_command(&ARD_I2C, args);
}

COMMAND(cmd_i2c,
	">i2c                       I2C access\n"
	"-i2c scan                  Scan I2C bus\n"
	"-i2c [w AA DD ...] [r AA NN]\n"
	"-                          Raw I2C transaction\n"
)
