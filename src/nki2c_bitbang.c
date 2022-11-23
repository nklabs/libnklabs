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

// Bitbang I2C master interace

#include "nkarch.h"
#include "nkpin.h"
#include "nki2c.h"

#define SPEED 10

static void nk_i2c_scl_low(nk_bitbang_i2c_bus_t *port)
{
	nk_pin_write(port->scl, 0);
}

static void nk_i2c_scl_high(nk_bitbang_i2c_bus_t *port)
{
	nk_pin_write(port->scl, 1);
}

static void nk_i2c_sda_low(nk_bitbang_i2c_bus_t *port)
{
	nk_pin_write(port->sda, 0);
}

static void nk_i2c_sda_high(nk_bitbang_i2c_bus_t *port)
{
	nk_pin_write(port->sda, 1);
}

static int nk_i2c_scl_peek(nk_bitbang_i2c_bus_t *port)
{
	bool rtn;
	nk_pin_read(port->scl, &rtn);
	return rtn;
}

static int nk_i2c_sda_peek(nk_bitbang_i2c_bus_t *port)
{
	bool rtn;
	nk_pin_read(port->sda, &rtn);
	return rtn;
}

// Pulse SCL, read along the way

static int sda_read(nk_bitbang_i2c_bus_t *port)
{
	int rtn;
	int x;
	nk_udelay(SPEED); // Tsu:dat = 250 ns min, Tlow = 4.7 us min
	nk_i2c_scl_high(port);
	// should pause here until scl is really high in case slave is stretching
	for (x = 0; x != 100; ++x) {
		nk_udelay(SPEED); // Thigh = 4.0 us min
		rtn = nk_i2c_scl_peek(port);
		if (rtn)
			break;
	}
	if (x == 100)
		nk_printf("SCL stuck low?\n");
	rtn = nk_i2c_sda_peek(port);
	nk_i2c_scl_low(port);
	nk_udelay(SPEED); // huh?
	return rtn;
}

static void i2c_start(nk_bitbang_i2c_bus_t *port)
{
	// SCL must already be high
	nk_i2c_sda_low(port);
	nk_udelay(SPEED); // Thd:sta = 4.0 us min
	nk_i2c_scl_low(port);
	nk_udelay(SPEED); // Huh?
}

// Normal stop

static void i2c_stop(nk_bitbang_i2c_bus_t *port)
{
	nk_i2c_sda_low(port);
	nk_udelay(SPEED); // Tlow = 4.7 us min
	nk_i2c_scl_high(port);
	nk_udelay(SPEED); // Tsu:sto 4.0 us min
	nk_i2c_sda_high(port);
	nk_udelay(SPEED); // Tbuf = 4.7 us min
}

// Stop before a repeated start

static void i2c_stopr(nk_bitbang_i2c_bus_t *port)
{
	nk_i2c_sda_high(port);
	nk_udelay(SPEED); // Tlow = 4.7 us min
	nk_i2c_scl_high(port);
	nk_udelay(SPEED); // Tsu:sto 4.0 us min
}

static void i2c_byte_out(nk_bitbang_i2c_bus_t *port, uint8_t x)
{
	uint8_t b;
	for (b = 128; b; b >>= 1) {
		if (b & x) {
			nk_i2c_sda_high(port);
			sda_read(port);
		} else {
			nk_i2c_sda_low(port);
			sda_read(port);
		}
	}
}

static uint8_t i2c_byte_in(nk_bitbang_i2c_bus_t *port)
{
	int n;
	uint8_t v = 0;
	nk_i2c_sda_high(port);
	for (n = 0; n != 8; n++) {
		if (sda_read(port))
			v = (uint8_t)((v << 1) + 1);
		else
			v = (uint8_t)(v << 1);
	}
	return v;
}

static int i2c_check_ack(nk_bitbang_i2c_bus_t *port)
{
	nk_i2c_sda_high(port);
	if (sda_read(port)) {
		// No ack!
		return -1;
	} else {
		// Ack received
		return 0;
	}
}

static void i2c_send_ack(nk_bitbang_i2c_bus_t *port)
{
	nk_i2c_sda_low(port);
	sda_read(port);
}

static void i2c_send_nak(nk_bitbang_i2c_bus_t *port)
{
	nk_i2c_sda_high(port);
	sda_read(port);
}

int nk_bitbang_i2c_write(nk_bitbang_i2c_bus_t *port, uint8_t addr, size_t len, const uint8_t *buf)
{
	int status = 0;
	i2c_start(port);
	i2c_byte_out(port, (addr << 1));
	if (i2c_check_ack(port)) {
		status = -1;
		goto done;
	}
	while (len--) {
		i2c_byte_out(port, *buf++);
		if (i2c_check_ack(port)) {
			status = -1;
			goto done;
		}
	}
	done:
	i2c_stop(port);
	return status;
}

int nk_bitbang_i2c_write_nostop(nk_bitbang_i2c_bus_t *port, uint8_t addr, size_t len, const uint8_t *buf)
{
	int status = 0;
	i2c_start(port);
	i2c_byte_out(port, (addr << 1));
	if (i2c_check_ack(port)) {
		status = -1;
		goto done;
	}
	while (len--) {
		i2c_byte_out(port, *buf++);
		if (i2c_check_ack(port)) {
			status = -1;
			goto done;
		}
	}
	done:
	i2c_stopr(port);
	return status;
}

int nk_bitbang_i2c_read(nk_bitbang_i2c_bus_t *port, uint8_t addr, size_t len, uint8_t *buf)
{
	int status = 0;
	i2c_start(port);
	i2c_byte_out(port, (addr << 1) | 1);
	if (i2c_check_ack(port)) {
		status = -1;
		goto done;
	}
	while (len--) {
		*buf++ = i2c_byte_in(port);
		if (len)
			i2c_send_ack(port);
		else
			i2c_send_nak(port);
	}
	done:
	i2c_stop(port);
	return status;
}
