// MCP23017 GPIO expander

#include "nkcli.h"
#include "nki2c.h"
#include "i2c.h"

#define MCP23017_I2C_ADDR 0x27 // 0x20 - 0x27 depending on A0,A1,A2

const nk_i2c_device_t mcp23017 =
{
    .i2c_bus = &ard_i2c_bus,
    .i2c_addr = MCP23017_I2C_ADDR
};

// IOCON.BANK = 0 register map

#define MCP23017_REG_IODIRA 0x00
#define MCP23017_REG_IODIRB 0x01
#define MCP23017_REG_IPOLA 0x02
#define MCP23017_REG_IPOLB 0x03
#define MCP23017_REG_GPINTENA 0x04
#define MCP23017_REG_GPINTENB 0x05
#define MCP23017_REG_DEFVALA 0x06
#define MCP23017_REG_DEFVALB 0x07
#define MCP23017_REG_INTCONA 0x08
#define MCP23017_REG_INTCONB 0x09
#define MCP23017_REG_IOCONA 0x0A
#define MCP23017_REG_IOCONB 0x0B
#define MCP23017_REG_GPPUA 0x0C
#define MCP23017_REG_GPPUB 0x0D
#define MCP23017_REG_INTFA 0x0E
#define MCP23017_REG_INTFB 0x0F
#define MCP23017_REG_INTCAPA 0x10
#define MCP23017_REG_INTCAPB 0x11
#define MCP23017_REG_GPIOA 0x12
#define MCP23017_REG_GPIOB 0x13
#define MCP23017_REG_OLATA 0x14
#define MCP23017_REG_OLATB 0x15

// Set/Get PORT A direction bits
//   1 means input, 0 means output

int mcp23017_set_dira(const nk_i2c_device_t *dev, uint8_t dir)
{
    return nk_i2c_put_byte(dev, MCP23017_REG_IODIRA, dir);
}

int mcp23017_get_dira(const nk_i2c_device_t *dev, uint8_t *dir)
{
    return nk_i2c_get_byte(dev, MCP23017_REG_IODIRA, dir);
}

// Set/Get PORT B direction bits
//   1 means input, 0 means output

int mcp23017_set_dirb(const nk_i2c_device_t *dev, uint8_t dir)
{
    return nk_i2c_put_byte(dev, MCP23017_REG_IODIRB, dir);
}

int mcp23017_get_dirb(const nk_i2c_device_t *dev, uint8_t *dir)
{
    return nk_i2c_get_byte(dev, MCP23017_REG_IODIRB, dir);
}

// Set/Get PORT A pull-up bits
//   1 means enable pull-up

int mcp23017_set_pupa(const nk_i2c_device_t *dev, uint8_t pup)
{
    return nk_i2c_put_byte(dev, MCP23017_REG_GPPUA, pup);
}

int mcp23017_get_pupa(const nk_i2c_device_t *dev, uint8_t *pup)
{
    return nk_i2c_get_byte(dev, MCP23017_REG_GPPUA, pup);
}

// Set/Get PORT B pull-up bits
//   1 means enable pull-up

int mcp23017_set_pupb(const nk_i2c_device_t *dev, uint8_t pup)
{
    return nk_i2c_put_byte(dev, MCP23017_REG_GPPUB, pup);
}

int mcp23017_get_pupb(const nk_i2c_device_t *dev, uint8_t *pup)
{
    return nk_i2c_get_byte(dev, MCP23017_REG_GPPUB, pup);
}

// Set/Get PORT A pins

int mcp23017_set_gpioa(const nk_i2c_device_t *dev, uint8_t val)
{
    return nk_i2c_put_byte(dev, MCP23017_REG_GPIOA, val);
}

int mcp23017_get_gpioa(const nk_i2c_device_t *dev, uint8_t *val)
{
    return nk_i2c_get_byte(dev, MCP23017_REG_GPIOA, val);
}

// Set/Get PORT B pins

int mcp23017_set_gpiob(const nk_i2c_device_t *dev, uint8_t val)
{
    return nk_i2c_put_byte(dev, MCP23017_REG_GPIOB, val);
}

int mcp23017_get_gpiob(const nk_i2c_device_t *dev, uint8_t *val)
{
    return nk_i2c_get_byte(dev, MCP23017_REG_GPIOB, val);
}

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
