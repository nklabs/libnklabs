// PCF8575 I2C GPIO expander

// This simple device uses quasi-birdirection pins.  For each bit:
//   Write 1 for input with pull-up
//   Write 0 for output low

// This is just like the PCF8574, except 16-bits instead of 8

#include "nkcli.h"
#include "nki2c.h"
#include "i2c.h"

#define PCF8575_I2C_ADDR 0x20 // 0x20 - 0x27 depending on A0-A2 inputs

const nk_i2c_device_t pcf8575 =
{
    .i2c_bus = &ard_i2c_bus, // Which bus it's on
    .i2c_addr = PCF8575_I2C_ADDR // I2C address of device: 0x20 - 0x27 depending on A0 - A2 inputs
};

// Read GPIO short

int pcf8575_read(const nk_i2c_device_t *dev, uint16_t *val)
{
    return nk_i2c_read(dev, 2, (uint8_t *)val);
}

// Write GPIO short

int pcf8575_write(const nk_i2c_device_t *dev, uint16_t val)
{
    return nk_i2c_write(dev, 2, (uint8_t *)&val);
}

// Command line

int cmd_pcf8575(nkinfile_t *args)
{
    uint16_t val;
    int rtn;
    if (nk_fscan(args, " ")) {
        rtn = pcf8575_read(&pcf8575, &val);
        if (rtn)
            nk_printf("I2C error\n");
        else
            nk_printf("Read value = %x\n", val);
    } else if (nk_fscan(args, "%hx ", &val)) {
        rtn = pcf8575_write(&pcf8575, val);
        if (rtn)
            nk_printf("I2C error\n");
        else
            nk_printf("Wrote %x\n", val);
    } else {
        nk_printf("Syntax error\n");
    }
    return 0;
}

COMMAND(cmd_pcf8575,
    ">pcf8575                   PCF8575 commands\n"
    "-pcf8575 [xx]              Read [write] PCF8575\n"
)
