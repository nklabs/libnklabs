// author: jimmy cheung
// email: jcheung@nklabs.com
// CLI Commands for TCAL6408 
// Part of Galvion Project

#include "nkcli.h"
#include "i2c.h"

#include "nkdriver_tcal6408.h"

const nk_i2c_device_t tcal6408 =
{ 
    .i2c_bus = &ard_i2c_bus,
    .i2c_addr = TCAL6408_ADDR
};

int cmd_tcal6408(nkinfile_t *args)
{
    uint8_t pin;
    uint8_t val;
    uint8_t addr;
    uint8_t dir;
    char addr_bits[9];
    char val_bits[9];
    int rtn;


    // command parsing using only 8 bit binary inputs for 
    if (nk_fscan(args, "rd pin %hhu", &pin)) {
        rtn = tcal6408_get_input(&tcal6408, pin, &val);
        if (rtn == -1) nk_printf("I2C error: could not read value at pin\n");
        else if (rtn == -2) nk_printf("User error: requested invalid pin\n");
        else nk_printf("Read value %x at pin %x\n", val, pin);
    }
    else if (nk_fscan(args, "rd reg %w", addr_bits, sizeof(addr_bits))) {
        if (bin2dec(addr_bits, &addr)) {
            nk_printf("User error: invalid binary input\n");
        }
        else {
            rtn = tcal6408_read(&tcal6408, addr, &val);
            if (rtn) nk_printf("I2C error: could not read value at address\n"); 
            else nk_printf("Read value %x at address %x\n", val, addr);
        }
    }
    else if (nk_fscan(args, "wr pin %hhu %hhu", &pin, &dir)) {
        if (pin > 7 || pin < 0 || dir > 1 || dir < 0) nk_printf("User error: requested invalid pin or direction\n");
        else {
            rtn = tcal6408_set_output(&tcal6408, pin, dir);
            if (rtn == -2) nk_printf("User error: requested invalid pin\n");
            else if (rtn == -1) nk_printf("I2C error: could not write value at pin\n");
            else nk_printf("Wrote value = %x at bit %x\n", dir, pin);
        }
    }
    else if (nk_fscan(args, "set %w %w", addr_bits, sizeof(addr_bits), val_bits, sizeof(val_bits))) {    //binary overwrite
        if (bin2dec(addr_bits, &addr) || bin2dec(val_bits, &val)) {
            nk_printf("User error: invalid binary input\n");
        }
        else {
            rtn = tcal6408_write(&tcal6408, addr, val);
            if (rtn) nk_printf("I2C error: could not set value at address\n"); 
            else nk_printf("Wrote value %x at register %x\n", val, addr);
        } 
    }
    else if (nk_fscan(args, "rd reg hex %w", addr_bits, sizeof(addr_bits))) {
        if (hex2dec(addr_bits, &addr)) {
            nk_printf("User error: invalid hex input\n");
        }
        else {
            rtn = tcal6408_read(&tcal6408, addr, &val);
            if (rtn) nk_printf("I2C error: could not read value at address\n"); 
            else nk_printf("Read value %x at address 0x%x\n", val, addr);
        }
    }
    else if (nk_fscan(args, "set hex %w %w", addr_bits, sizeof(addr_bits), val_bits, sizeof(val_bits))) {    //binary overwrite
        if (hex2dec(addr_bits, &addr) || hex2dec(val_bits, &val)) {
            nk_printf("User error: invalid hex input\n");
        }
        else {
            rtn = tcal6408_write(&tcal6408, addr, val);
            if (rtn) nk_printf("I2C error: could not set value at address\n"); 
            else nk_printf("Wrote value %x at register %x\n", val, addr);
        } 
    }
    else {
        nk_printf("Syntax error\n");
    }
    return 0;
}

COMMAND(cmd_tcal6408, 
    ">tcal6408                          TCAL6408 commands\n"
    "-tcal6408 rd pin [pin]             Read value of pin\n"
    "-tcal6408 rd reg [addr]            Read value of register\n"
    "-tcal6408 wr pin [pin] [dir]       Write value to pin\n"
    "-tcal6408 set [addr] [val]         Write value to register\n"
    "-tcal6408 rd reg hex [addr]        Read value of register with hex input\n"
    "-tcal6408 set hex [addr] hex [val] Write value to register with hex input\n"
)