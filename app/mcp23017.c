// MCP23017 GPIO expander

#include "nkcli.h"
#include "nkdriver_mcp23017.h"
#include "i2c.h"

const nk_i2c_device_t mcp23017 =
{
    .i2c_bus = &ard_i2c_bus,
    .i2c_addr = MCP23017_I2C_ADDR + 7
};

int cmd_mcp23017(nkinfile_t *args)
{
    uint8_t val;
    int rtn;
    if (nk_fscan(args, "gpioa ")) {
        rtn = mcp23017_get_gpioa(&mcp23017, &val);
        if (rtn) {
            nk_printf("I2C error\n");
        } else {
            nk_printf("GPIOA = %x\n", val);
        }
    } else if (nk_fscan(args, "gpioa %hhx", &val)) {
        rtn = mcp23017_set_gpioa(&mcp23017, val);
        if (rtn) {
            nk_printf("I2C error\n");
        } else {
            nk_printf("Wrote %x to GPIOA\n", val);
        }
    } else if (nk_fscan(args, "gpiob ")) {
        rtn = mcp23017_get_gpiob(&mcp23017, &val);
        if (rtn) {
            nk_printf("I2C error\n");
        } else {
            nk_printf("GPIOB = %x\n", val);
        }
    } else if (nk_fscan(args, "gpiob %hhx", &val)) {
        rtn = mcp23017_set_gpiob(&mcp23017, val);
        if (rtn) {
            nk_printf("I2C error\n");
        } else {
            nk_printf("Wrote %x to GPIOB\n", val);
        }
    } else if (nk_fscan(args, "dira ")) {
        rtn = mcp23017_get_dira(&mcp23017, &val);
        if (rtn) {
            nk_printf("I2C error\n");
        } else {
            nk_printf("DIRA = %x\n", val);
        }
    } else if (nk_fscan(args, "dira %hhx", &val)) {
        rtn = mcp23017_set_dira(&mcp23017, val);
        if (rtn) {
            nk_printf("I2C error\n");
        } else {
            nk_printf("Wrote %x to DIRA\n", val);
        }
    } else if (nk_fscan(args, "dirb ")) {
        rtn = mcp23017_get_dirb(&mcp23017, &val);
        if (rtn) {
            nk_printf("I2C error\n");
        } else {
            nk_printf("DIRB = %x\n", val);
        }
    } else if (nk_fscan(args, "dirb %hhx", &val)) {
        rtn = mcp23017_set_dirb(&mcp23017, val);
        if (rtn) {
            nk_printf("I2C error\n");
        } else {
            nk_printf("Wrote %x to DIRB\n", val);
        }
    } else if (nk_fscan(args, "pupa ")) {
        rtn = mcp23017_get_pupa(&mcp23017, &val);
        if (rtn) {
            nk_printf("I2C error\n");
        } else {
            nk_printf("PUPA = %x\n", val);
        }
    } else if (nk_fscan(args, "pupa %hhx", &val)) {
        rtn = mcp23017_set_pupa(&mcp23017, val);
        if (rtn) {
            nk_printf("I2C error\n");
        } else {
            nk_printf("Wrote %x to PUPA\n", val);
        }
    } else if (nk_fscan(args, "pupb ")) {
        rtn = mcp23017_get_pupb(&mcp23017, &val);
        if (rtn) {
            nk_printf("I2C error\n");
        } else {
            nk_printf("PUPB = %x\n", val);
        }
    } else if (nk_fscan(args, "pupb %hhx", &val)) {
        rtn = mcp23017_set_pupb(&mcp23017, val);
        if (rtn) {
            nk_printf("I2C error\n");
        } else {
            nk_printf("Wrote %x to PUPB\n", val);
        }
    } else {
        nk_printf("Syntax error\n");
    }
    return 0;
}

COMMAND(cmd_mcp23017,
    ">mcp23017                  MCP23017 commands\n"
    "-mcp23017 gpioa [xx]       Read [Write] port a\n"
    "-mcp23017 gpiob [xx]       Read [Write] port b\n"
    "-mcp23017 dira [xx]        Read [Write] port a direction (1 = output)\n"
    "-mcp23017 dirb [xx]        Read [Write] port b direction (1 = input)(\n"
    "-mcp23017 pupa [xx]        Read [Write] port a pull-ups (1 = enable)\n"
    "-mcp23017 pupb [xx]        Read [Write] port b pull-ups (1 = enable)\n"
)
