// Copyright 2021 NK Labs, LLC

// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:

// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
// OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
// THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef _Imcp23017
#define _Imcp23017

// MCP23017 GPIO expander

#include "nki2c.h"

#define MCP23017_I2C_ADDR 0x20 // 0x20 - 0x27 depending on A0,A1,A2

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

int mcp23017_set_dira(const nk_i2c_device_t *dev, uint8_t dir);
int mcp23017_get_dira(const nk_i2c_device_t *dev, uint8_t *dir);

// Set/Get PORT B direction bits
//   1 means input, 0 means output

int mcp23017_set_dirb(const nk_i2c_device_t *dev, uint8_t dir);
int mcp23017_get_dirb(const nk_i2c_device_t *dev, uint8_t *dir);

// Set/Get PORT A pull-up bits
//   1 means enable pull-up

int mcp23017_set_pupa(const nk_i2c_device_t *dev, uint8_t pup);
int mcp23017_get_pupa(const nk_i2c_device_t *dev, uint8_t *pup);

// Set/Get PORT B pull-up bits
//   1 means enable pull-up

int mcp23017_set_pupb(const nk_i2c_device_t *dev, uint8_t pup);
int mcp23017_get_pupb(const nk_i2c_device_t *dev, uint8_t *pup);

// Set/Get PORT A pins

int mcp23017_set_gpioa(const nk_i2c_device_t *dev, uint8_t val);
int mcp23017_get_gpioa(const nk_i2c_device_t *dev, uint8_t *val);

// Set/Get PORT B pins

int mcp23017_set_gpiob(const nk_i2c_device_t *dev, uint8_t val);
int mcp23017_get_gpiob(const nk_i2c_device_t *dev, uint8_t *val);

#endif
