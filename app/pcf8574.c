// PCF8574 I2C GPIO expander

// This simple device uses quasi-birdirection pins.  For each bit:
//   Write 1 for input with pull-up
//   Write 0 for output low

#include "nkcli.h"
#include "nki2c.h"

#define PCF8574_I2C_ADDR 0x20 // 0x20 - 0x27 depending on A0-A2 inputs
#define PCF8574_PORT (&ARD_I2C)

// Read GPIO byte

int pcf8574_read(uint8_t *val)
{
    return nk_i2c_read(PCF8574_PORT, PCF8574_I2C_ADDR, 1, val);
}

// Write GPIO byte

int pcf8574_write(uint8_t val)
{
    return nk_i2c_write(PCF8574_PORT, PCF8574_I2C_ADDR, 1, &val);
}

// Command line

int cmd_pcf8574(nkinfile_t *args)
{
    uint8_t val;
    int rtn;
    if (nk_fscan(args, " ")) {
        rtn = pcf8574_read(&val);
        if (rtn)
            nk_printf("Read error\n");
        else
            nk_printf("Read value = %x\n", val);
    } else if (nk_fscan(args, "%hhx ", &val)) {
        rtn = pcf8574_write(val);
        if (rtn)
            nk_printf("Write error\n");
        else
            nk_printf("Wrote %x\n", val);
    } else {
        nk_printf("Syntax error\n");
    }
    return 0;
}

COMMAND(cmd_pcf8574,
    ">pcf8574                   PCF8574 commands\n"
    "-pcf8574 [xx]              Read [write] PCF8574\n"
)
