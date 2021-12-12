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

// I2C master interace

#include "nkprintf.h"
#include "nkscan.h"
#include "i2c_master.h"

#define SPEED 10

// Bit bang I2C interface

void i2c_master_init(void)
{
	startup("I2C Master\n");
}

static void scl_low(i2c_handle port)
{
	*port = (I2C_PORT_SCL_BIT << 16);
}

static void scl_high(i2c_handle port)
{
	*port = ((I2C_PORT_SCL_BIT << 16) | I2C_PORT_SCL_BIT);
}

static void sda_low(i2c_handle port)
{
	*port = (I2C_PORT_SDA_BIT << 16);
}

static void sda_high(i2c_handle port)
{
	*port = ((I2C_PORT_SDA_BIT << 16) | I2C_PORT_SDA_BIT);
}

// All of the dual_ ones control two I2C ports at the same time

static void dual_scl_low(i2c_handle port)
{
	*port = (I2C_CAM0_SCL_BIT << 16) | (I2C_CAM3_SCL_BIT << 16);
}

static void dual_scl_high(i2c_handle port)
{
	*port = ((I2C_CAM0_SCL_BIT << 16) | I2C_CAM0_SCL_BIT) | ((I2C_CAM3_SCL_BIT << 16) | I2C_CAM3_SCL_BIT);
}

static void dual_sda_low(i2c_handle port)
{
	*port = (I2C_CAM0_SDA_BIT << 16) | (I2C_CAM3_SDA_BIT << 16);
}

static void dual_sda_high(i2c_handle port)
{
	*port = ((I2C_CAM0_SDA_BIT << 16) | I2C_CAM0_SDA_BIT) | ((I2C_CAM3_SDA_BIT << 16) | I2C_CAM3_SDA_BIT);
}

// Pulse SCL, read along the way

static int sda_read(i2c_handle port)
{
	int rtn;
	int x;
	nk_udelay(SPEED); // Tsu:dat = 250 ns min, Tlow = 4.7 us min
	scl_high(port);
	// should pause here until scl is really high in case slave is stretching
	for (x = 0; x != 100; ++x) {
		nk_udelay(SPEED); // Thigh = 4.0 us min
		rtn = (*port & I2C_PORT_SCL_BIT);
		if (rtn)
			break;
	}
	if (x == 100)
		nk_printf("SCL stuck low?\n");
	rtn = (*port & I2C_PORT_SDA_BIT);
	scl_low(port);
	nk_udelay(SPEED); // huh?
	return rtn;
}

static int dual_sda_read(i2c_handle port)
{
	int rtn;
	int x;
	nk_udelay(SPEED); // Tsu:dat = 250 ns min, Tlow = 4.7 us min
	dual_scl_high(port);
	// should pause here until scl is really high in case slave is stretching
	for (x = 0; x != 100; ++x) {
		nk_udelay(SPEED); // Thigh = 4.0 us min
		rtn = (*port & I2C_CAM0_SCL_BIT);
		if (rtn)
			break;
	}
	if (x == 100)
		nk_printf("SCL stuck low?\n");
	rtn = (*port & I2C_CAM0_SDA_BIT);
	dual_scl_low(port);
	return rtn;
}

static void i2c_start(i2c_handle port)
{
	// SCL must already be high
	sda_low(port);
	nk_udelay(SPEED); // Thd:sta = 4.0 us min
	scl_low(port);
	nk_udelay(SPEED); // Huh?
}

static void dual_i2c_start(i2c_handle port)
{
	// SCL must already be high
	dual_sda_low(port);
	nk_udelay(SPEED); // Thd:sta = 4.0 us min
	dual_scl_low(port);
}

// Normal stop

static void i2c_stop(i2c_handle port)
{
	sda_low(port);
	nk_udelay(SPEED); // Tlow = 4.7 us min
	scl_high(port);
	nk_udelay(SPEED); // Tsu:sto 4.0 us min
	sda_high(port);
	nk_udelay(SPEED); // Tbuf = 4.7 us min
}

static void dual_i2c_stop(i2c_handle port)
{
	dual_sda_low(port);
	nk_udelay(SPEED); // Tlow = 4.7 us min
	dual_scl_high(port);
	nk_udelay(SPEED); // Tsu:sto 4.0 us min
	dual_sda_high(port);
	nk_udelay(SPEED); // Tbuf = 4.7 us min
}

// Stop before a repeated start

static void i2c_stopr(i2c_handle port)
{
	sda_high(port);
	nk_udelay(SPEED); // Tlow = 4.7 us min
	scl_high(port);
	nk_udelay(SPEED); // Tsu:sto 4.0 us min
}

static void dual_i2c_stopr(i2c_handle port)
{
	dual_sda_high(port);
	nk_udelay(SPEED); // Tlow = 4.7 us min
	dual_scl_high(port);
	nk_udelay(SPEED); // Tsu:sto 4.0 us min
}

static void i2c_byte_out(i2c_handle port, uint8_t x)
{
	uint8_t b;
	for (b = 128; b; b >>= 1) {
		if (b & x) {
			sda_high(port);
			sda_read(port);
		} else {
			sda_low(port);
			sda_read(port);
		}
	}
}

static void dual_i2c_byte_out(i2c_handle port, uint8_t x)
{
	uint8_t b;
	for (b = 128; b; b >>= 1) {
		if (b & x) {
			dual_sda_high(port);
			dual_sda_read(port);
		} else {
			dual_sda_low(port);
			dual_sda_read(port);
		}
	}
}

static uint8_t i2c_byte_in(i2c_handle port)
{
	int n;
	uint8_t v = 0;
	sda_high(port);
	for (n = 0; n != 8; n++) {
		if (sda_read(port))
			v = (uint8_t)((v << 1) + 1);
		else
			v = (uint8_t)(v << 1);
	}
	return v;
}

static uint8_t dual_i2c_byte_in(i2c_handle port)
{
	int n;
	uint8_t v = 0;
	dual_sda_high(port);
	for (n = 0; n != 8; n++) {
		if (dual_sda_read(port))
			v = (uint8_t)((v << 1) + 1);
		else
			v = (uint8_t)(v << 1);
	}
	return v;
}

static int i2c_check_ack(i2c_handle port)
{
	sda_high(port);
	if (sda_read(port)) {
		// No ack!
		return -1;
	} else {
		// Ack received
		return 0;
	}
}

static int dual_i2c_check_ack(i2c_handle port)
{
	dual_sda_high(port);
	if (dual_sda_read(port)) {
		// No ack!
		return -1;
	} else {
		// Ack received
		return 0;
	}
}

static void i2c_send_ack(i2c_handle port)
{
	sda_low(port);
	sda_read(port);
}

static void dual_i2c_send_ack(i2c_handle port)
{
	dual_sda_low(port);
	dual_sda_read(port);
}

static void i2c_send_nak(i2c_handle port)
{
	sda_high(port);
	sda_read(port);
}

static void dual_i2c_send_nak(i2c_handle port)
{
	dual_sda_high(port);
	dual_sda_read(port);
}

int i2c_master_write(i2c_handle port, uint8_t addr, size_t len, const uint8_t *buf)
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

int dual_i2c_master_write(i2c_handle port, uint8_t addr, size_t len, const uint8_t *buf)
{
	int status = 0;
	dual_i2c_start(port);
	dual_i2c_byte_out(port, (addr << 1));
	if (dual_i2c_check_ack(port)) {
		status = -1;
		goto done;
	}
	while (len--) {
		dual_i2c_byte_out(port, *buf++);
		if (dual_i2c_check_ack(port)) {
			status = -1;
			goto done;
		}
	}
	done:
	dual_i2c_stop(port);
	return status;
}

int i2c_master_write_nostop(i2c_handle port, uint8_t addr, size_t len, const uint8_t *buf)
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

int dual_i2c_master_write_nostop(i2c_handle port, uint8_t addr, size_t len, const uint8_t *buf)
{
	int status = 0;
	dual_i2c_start(port);
	dual_i2c_byte_out(port, (addr << 1));
	if (dual_i2c_check_ack(port)) {
		status = -1;
		goto done;
	}
	while (len--) {
		dual_i2c_byte_out(port, *buf++);
		if (dual_i2c_check_ack(port)) {
			status = -1;
			goto done;
		}
	}
	done:
	dual_i2c_stopr(port);
	return status;
}

int i2c_master_read(i2c_handle port, uint8_t addr, size_t len, uint8_t *buf)
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

int dual_i2c_master_read(i2c_handle port, uint8_t addr, size_t len, uint8_t *buf)
{
	int status = 0;
	dual_i2c_start(port);
	dual_i2c_byte_out(port, (addr << 1) | 1);
	if (dual_i2c_check_ack(port)) {
		status = -1;
		goto done;
	}
	while (len--) {
		*buf++ = dual_i2c_byte_in(port);
		if (len)
			dual_i2c_send_ack(port);
		else
			dual_i2c_send_nak(port);
	}
	done:
	dual_i2c_stop(port);
	return status;
}

// These all use functions defined above

int i2c_master_put_byte(i2c_handle port, uint8_t addr, uint8_t ofst, uint8_t data)
{
	uint8_t buf[2];
	buf[0] = ofst;
	buf[1] = data;
	return i2c_master_write(port, addr, 2, buf);
}

int i2c_master_put2_byte(i2c_handle port, uint8_t addr, uint16_t ofst, uint8_t data)
{
	uint8_t buf[3];
	buf[0] = (uint8_t)(ofst >> 8);
	buf[1] = (uint8_t)ofst;
	buf[2] = data;
	return i2c_master_write(port, addr, 3, buf);
}

int dual_i2c_master_put2_byte(i2c_handle port, uint8_t addr, uint16_t ofst, uint8_t data)
{
	uint8_t buf[3];
	buf[0] = (uint8_t)(ofst >> 8);
	buf[1] = (uint8_t)ofst;
	buf[2] = data;
	return dual_i2c_master_write(port, addr, 3, buf);
}

int i2c_master_get_byte(i2c_handle port, uint8_t addr, uint8_t ofst, uint8_t *data)
{
	int status;
	uint8_t buf[1];
	buf[0] = ofst;
	status = i2c_master_write_nostop(port, addr, 1, buf);
	if (status)
		return status;
	return i2c_master_read(port, addr, 1, data);
}

int i2c_master_get2_byte(i2c_handle port, uint8_t addr, uint16_t ofst, uint8_t *data)
{
	int status;
	uint8_t buf[2];
	buf[0] = (uint8_t)(ofst >> 8);
	buf[1] = (uint8_t)ofst;
	status = i2c_master_write_nostop(port, addr, 2, buf);
	if (status)
		return status;
	return i2c_master_read(port, addr, 1, data);
}

int dual_i2c_master_get2_byte(i2c_handle port, uint8_t addr, uint16_t ofst, uint8_t *data)
{
	int status;
	uint8_t buf[2];
	buf[0] = (uint8_t)(ofst >> 8);
	buf[1] = (uint8_t)ofst;
	status = dual_i2c_master_write_nostop(port, addr, 2, buf);
	if (status)
		return status;
	return dual_i2c_master_read(port, addr, 1, data);
}

int i2c_master_put_leshort(i2c_handle port, uint8_t addr, uint8_t ofst, uint16_t data)
{
	uint8_t buf[3];
	buf[0] = ofst;
	buf[1] = (uint8_t)data;
	buf[2] = (uint8_t)(data >> 8);
	return i2c_master_write(port, addr, 3, buf);
}

int i2c_master_put2_leshort(i2c_handle port, uint8_t addr, uint16_t ofst, uint16_t data)
{
	uint8_t buf[4];
	buf[0] = (uint8_t)(ofst >> 8);
	buf[1] = (uint8_t)ofst;
	buf[2] = (uint8_t)data;
	buf[3] = (uint8_t)(data >> 8);
	return i2c_master_write(port, addr, 4, buf);
}

int dual_i2c_master_put2_leshort(i2c_handle port, uint8_t addr, uint16_t ofst, uint16_t data)
{
	uint8_t buf[4];
	buf[0] = (uint8_t)(ofst >> 8);
	buf[1] = (uint8_t)ofst;
	buf[2] = (uint8_t)data;
	buf[3] = (uint8_t)(data >> 8);
	return dual_i2c_master_write(port, addr, 4, buf);
}

int i2c_master_put2_le24(i2c_handle port, uint8_t addr, uint16_t ofst, uint32_t data)
{
	uint8_t buf[5];
	buf[0] = (uint8_t)(ofst >> 8);
	buf[1] = (uint8_t)ofst;
	buf[2] = (uint8_t)data;
	buf[3] = (uint8_t)(data >> 8);
	buf[4] = (uint8_t)(data >> 16);
	return i2c_master_write(port, addr, 5, buf);
}

int dual_i2c_master_put2_le24(i2c_handle port, uint8_t addr, uint16_t ofst, uint32_t data)
{
	uint8_t buf[5];
	buf[0] = (uint8_t)(ofst >> 8);
	buf[1] = (uint8_t)ofst;
	buf[2] = (uint8_t)data;
	buf[3] = (uint8_t)(data >> 8);
	buf[4] = (uint8_t)(data >> 16);
	return dual_i2c_master_write(port, addr, 5, buf);
}

int i2c_master_get_leshort(i2c_handle port, uint8_t addr, uint8_t ofst, uint16_t *data)
{
	int status;
	uint8_t buf[2];
	buf[0] = ofst;
	status = i2c_master_write_nostop(port, addr, 1, buf);
	if (status)
		return status;
	status = i2c_master_read(port, addr, 2, buf);
	if (status)
		return status;
	*data = (buf[1] << 8) + (buf[0]);
	return 0;
}

int i2c_master_get2_leshort(i2c_handle port, uint8_t addr, uint16_t ofst, uint16_t *data)
{
	int status;
	uint8_t buf[2];
	buf[0] = (uint8_t)(ofst >> 8);
	buf[1] = (uint8_t)ofst;
	status = i2c_master_write_nostop(port, addr, 2, buf);
	if (status)
		return status;
	status = i2c_master_read(port, addr, 2, buf);
	if (status)
		return status;
	*data = (buf[1] << 8) + (buf[0]);
	return 0;
}

int dual_i2c_master_get2_leshort(i2c_handle port, uint8_t addr, uint16_t ofst, uint16_t *data)
{
	int status;
	uint8_t buf[2];
	buf[0] = (uint8_t)(ofst >> 8);
	buf[1] = (uint8_t)ofst;
	status = dual_i2c_master_write_nostop(port, addr, 2, buf);
	if (status)
		return status;
	status = dual_i2c_master_read(port, addr, 2, buf);
	if (status)
		return status;
	*data = (buf[1] << 8) + (buf[0]);
	return 0;
}

int i2c_master_get2_le24(i2c_handle port, uint8_t addr, uint16_t ofst, uint32_t *data)
{
	int status;
	uint8_t buf[3];
	buf[0] = (uint8_t)(ofst >> 8);
	buf[1] = (uint8_t)ofst;
	status = i2c_master_write_nostop(port, addr, 2, buf);
	if (status)
		return status;
	status = i2c_master_read(port, addr, 3, buf);
	if (status)
		return status;
	*data = (buf[2] << 16) + (buf[1] << 8) + (buf[0]);
	return 0;
}

int dual_i2c_master_get2_le24(i2c_handle port, uint8_t addr, uint16_t ofst, uint32_t *data)
{
	int status;
	uint8_t buf[3];
	buf[0] = (uint8_t)(ofst >> 8);
	buf[1] = (uint8_t)ofst;
	status = dual_i2c_master_write_nostop(port, addr, 2, buf);
	if (status)
		return status;
	status = dual_i2c_master_read(port, addr, 3, buf);
	if (status)
		return status;
	*data = (buf[2] << 16) + (buf[1] << 8) + (buf[0]);
	return 0;
}

int i2c_master_put_beshort(i2c_handle port, uint8_t addr, uint8_t ofst, uint16_t data)
{
	uint8_t buf[3];
	buf[0] = ofst;
	buf[1] = (uint8_t)(data >> 8);
	buf[2] = (uint8_t)(data);
	return i2c_master_write(port, addr, 3, buf);
}

int i2c_master_put2_beshort(i2c_handle port, uint8_t addr, uint16_t ofst, uint16_t data)
{
	uint8_t buf[4];
	buf[0] = (uint8_t)(ofst >> 8);
	buf[1] = (uint8_t)ofst;
	buf[2] = (uint8_t)(data >> 8);
	buf[3] = (uint8_t)(data);
	return i2c_master_write(port, addr, 4, buf);
}

int i2c_master_put2_melong(i2c_handle port, uint8_t addr, uint16_t ofst, uint32_t data)
{
	uint8_t buf[6];
	buf[0] = (uint8_t)(ofst >> 8);
	buf[1] = (uint8_t)ofst;
	buf[2] = (uint8_t)(data >> 8);
	buf[3] = (uint8_t)(data);
	buf[4] = (uint8_t)(data >> 24);
	buf[5] = (uint8_t)(data >> 16);
	return i2c_master_write(port, addr, 6, buf);
}

int i2c_master_get_beshort(i2c_handle port, uint8_t addr, uint8_t ofst, uint16_t *data)
{
	int status;
	uint8_t buf[2];
	buf[0] = ofst;
	status = i2c_master_write_nostop(port, addr, 1, buf);
	if (status)
		return status;
	status = i2c_master_read(port, addr, 2, buf);
	if (status)
		return status;
	*data = (buf[0] << 8) + (buf[1]);
	return 0;
}

int i2c_master_get2_beshort(i2c_handle port, uint8_t addr, uint16_t ofst, uint16_t *data)
{
	int status;
	uint8_t buf[2];
	buf[0] = (uint8_t)(ofst >> 8);
	buf[1] = (uint8_t)ofst;
	status = i2c_master_write_nostop(port, addr, 2, buf);
	if (status)
		return status;
	status = i2c_master_read(port, addr, 2, buf);
	*data = (buf[0] << 8) + (buf[1]);
	return 0;
}

/* [w AA DD ....] [r AA NN]
 *   optional write followed by optional read
 */


int i2c_command(i2c_handle port, nkinfile_t *args)
{
	int status;
	uint8_t write_array[20];
	uint8_t read_array[20];
	uint32_t write_len = 0;
	uint8_t read_addr;
	uint32_t read_len = 0;
	int z;
	for (z = 0; z != 20; ++z)
		read_array[z] = 0xee;

	if (nk_fscan(args, "scan ")) {
		int addr;
		// Reserved 7-bit addresses:
		//   0 = general call or start byte
		//   1 = CBUS addresses
		//   2 = reserved for different bus formats
		//   3 = reserved for future purposes
		//   4 - 7 = high speed master code
		//   78 - 7B = 10-bit slave addressing
		//   7C - 7F = reserved for future purposes
		for (addr = 8; addr != 0x78; ++addr) {
			int status  = i2c_master_write(port, addr, 0, write_array);
			if (!status) {
				nk_printf("Found device %x\n", addr);
			}
		}
		return 0;
	}


	if (nk_fscan(args, "w %e")) {
		// Write
		while (nk_fscan(args, "%hhx %e", &write_array[write_len]))
			++write_len;
	}
	
	if (!nk_fscan(args, "r %hhx %x ", &read_addr, &read_len)) {
		read_len = 0;
	}

	if (write_len) {
		size_t y;
		nk_puts(" write");
		for (y = 0; y != write_len; ++y) {
			nk_printf(" %x", write_array[y]);
		}
		if (read_len)
			status = i2c_master_write_nostop(port, write_array[0], write_len - 1, write_array + 1);
		else
			status = i2c_master_write(port, write_array[0], write_len - 1, write_array + 1);
		if (status) {
			nk_printf(" I2C write failed");
			read_len = 0;
		}
	}
	if (read_len) {
                size_t y;
		nk_printf(" read addr=%x len=%x", read_addr, read_len);
		status = i2c_master_read(port, read_addr, read_len, read_array);
		if (status)
			nk_printf(" I2C read failed");
		else {
			nk_puts(" got back:");
			for (y = 0; y != read_len; ++y) {
				nk_printf(" %x", read_array[y]);
	                }
		}
	}
	nk_puts("\n");
	return 0;
}

int dual_i2c_command(i2c_handle port, nkinfile_t *args)
{
	int status;
	uint8_t write_array[20];
	uint8_t read_array[20];
	uint32_t write_len = 0;
	uint8_t read_addr;
	uint32_t read_len = 0;
	int z;
	for (z = 0; z != 20; ++z)
		read_array[z] = 0xee;

	if (nk_fscan(args, "scan ")) {
		int addr;
		// Reserved 7-bit addresses:
		//   0 = general call or start byte
		//   1 = CBUS addresses
		//   2 = reserved for different bus formats
		//   3 = reserved for future purposes
		//   4 - 7 = high speed master code
		//   78 - 7B = 10-bit slave addressing
		//   7C - 7F = reserved for future purposes
		for (addr = 8; addr != 0x78; ++addr) {
			int status  = dual_i2c_master_write(port, addr, 0, write_array);
			if (!status) {
				nk_printf("Found device %x\n", addr);
			}
		}
		return 0;
	}


	if (nk_fscan(args, "w %e")) {
		// Write
		while (nk_fscan(args, "%hhx %e", &write_array[write_len]))
			++write_len;
	}
	
	if (!nk_fscan(args, "r %hhx %x ", &read_addr, &read_len)) {
		read_len = 0;
	}

	if (write_len) {
		size_t y;
		nk_puts(" write");
		for (y = 0; y != write_len; ++y) {
			nk_printf(" %x", write_array[y]);
		}
		if (read_len)
			status = dual_i2c_master_write_nostop(port, write_array[0], write_len - 1, write_array + 1);
		else
			status = dual_i2c_master_write(port, write_array[0], write_len - 1, write_array + 1);
		if (status) {
			nk_printf(" I2C write failed");
			read_len = 0;
		}
	}
	if (read_len) {
                size_t y;
		nk_printf(" read addr=%x len=%x", read_addr, read_len);
		status = dual_i2c_master_read(port, read_addr, read_len, read_array);
		if (status)
			nk_printf(" I2C read failed");
		else {
			nk_puts(" got back:");
			for (y = 0; y != read_len; ++y) {
				nk_printf(" %x", read_array[y]);
	                }
		}
	}
	nk_puts("\n");
	return 0;
}
