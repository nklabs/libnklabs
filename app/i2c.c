#include <string.h>
#include "nkarch.h"
#include "nkcli.h"
#include "nki2c.h"

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
