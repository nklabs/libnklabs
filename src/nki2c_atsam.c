// Copyright 2020 NK Labs, LLC

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

// ATSAM I2C

// It does not support writing 0-length (address only) transactions. You must write at
// least one byte.

#include "nkarch.h"
#include "nki2c.h"

int nk_hal_i2c_write(void *port, uint8_t addr, size_t len, const uint8_t *buf)
{
        struct i2c_m_sync_desc *i2c = (struct i2c_m_sync_desc *)port;

        struct _i2c_m_msg msg;

	i2c_m_sync_enable(i2c);

        msg.addr = addr;
        msg.flags = I2C_M_STOP;
        msg.len = len;
        msg.buffer = buf;

        int rtn = i2c_m_sync_transfer(i2c, &msg);

        return rtn;
}

int nk_hal_i2c_write_nostop(void *port, uint8_t addr, size_t len, const uint8_t *buf)
{
        return nk_hal_i2c_write(port, addr, len, buf);

#if 0
        struct i2c_m_sync_desc *i2c = (struct i2c_m_sync_desc *)port;

        struct _i2c_m_msg msg;

	i2c_m_sync_enable(i2c);

        msg.addr = addr;
        msg.flags = 0;
        msg.len = len;
        msg.buffer = buf;

        int rtn = i2c_m_sync_transfer(i2c, &msg);

        return rtn;
#endif
}


int nk_hal_i2c_read(void *port, uint8_t addr, size_t len, uint8_t *buf)
{
        struct i2c_m_sync_desc *i2c = (struct i2c_m_sync_desc *)port;

        struct _i2c_m_msg msg;

	i2c_m_sync_enable(i2c);

        msg.addr = addr;
        msg.flags = I2C_M_STOP | I2C_M_RD;
        msg.len = len;
        msg.buffer = buf;

        int rtn = i2c_m_sync_transfer(i2c, &msg);

        return rtn;
}
