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

// MCP23017 GPIO expander

#include "nkdriver_mcp23017.h"

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
