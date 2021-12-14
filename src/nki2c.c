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

#include "nkcli.h"
#include "nki2c.h"

int nk_i2c_put_byte(void *port, uint8_t addr, uint8_t ofst, uint8_t data)
{
	uint8_t buf[2];
	buf[0] = ofst;
	buf[1] = data;
	return nk_i2c_write(port, addr, 2, buf);
}

int nk_i2c_put2_byte(void *port, uint8_t addr, uint16_t ofst, uint8_t data)
{
	uint8_t buf[3];
	buf[0] = (uint8_t)(ofst >> 8);
	buf[1] = (uint8_t)ofst;
	buf[2] = data;
	return nk_i2c_write(port, addr, 3, buf);
}

int nk_i2c_get_byte(void *port, uint8_t addr, uint8_t ofst, uint8_t *data)
{
	int status;
	uint8_t buf[1];
	buf[0] = ofst;
	status = nk_i2c_write_nostop(port, addr, 1, buf);
	if (status)
		return status;
	return nk_i2c_read(port, addr, 1, data);
}

int nk_i2c_get2_byte(void *port, uint8_t addr, uint16_t ofst, uint8_t *data)
{
	int status;
	uint8_t buf[2];
	buf[0] = (uint8_t)(ofst >> 8);
	buf[1] = (uint8_t)ofst;
	status = nk_i2c_write_nostop(port, addr, 2, buf);
	if (status)
		return status;
	return nk_i2c_read(port, addr, 1, data);
}

int nk_i2c_put_leshort(void *port, uint8_t addr, uint8_t ofst, uint16_t data)
{
	uint8_t buf[3];
	buf[0] = ofst;
	buf[1] = (uint8_t)data;
	buf[2] = (uint8_t)(data >> 8);
	return nk_i2c_write(port, addr, 3, buf);
}

int nk_i2c_put2_leshort(void *port, uint8_t addr, uint16_t ofst, uint16_t data)
{
	uint8_t buf[4];
	buf[0] = (uint8_t)(ofst >> 8);
	buf[1] = (uint8_t)ofst;
	buf[2] = (uint8_t)data;
	buf[3] = (uint8_t)(data >> 8);
	return nk_i2c_write(port, addr, 4, buf);
}

int nk_i2c_put2_le24(void *port, uint8_t addr, uint16_t ofst, uint32_t data)
{
	uint8_t buf[5];
	buf[0] = (uint8_t)(ofst >> 8);
	buf[1] = (uint8_t)ofst;
	buf[2] = (uint8_t)data;
	buf[3] = (uint8_t)(data >> 8);
	buf[4] = (uint8_t)(data >> 16);
	return nk_i2c_write(port, addr, 5, buf);
}

int nk_i2c_get_leshort(void *port, uint8_t addr, uint8_t ofst, uint16_t *data)
{
	int status;
	uint8_t buf[2];
	buf[0] = ofst;
	status = nk_i2c_write_nostop(port, addr, 1, buf);
	if (status)
		return status;
	status = nk_i2c_read(port, addr, 2, buf);
	if (status)
		return status;
	*data = (buf[1] << 8) + (buf[0]);
	return 0;
}

int nk_i2c_get2_leshort(void *port, uint8_t addr, uint16_t ofst, uint16_t *data)
{
	int status;
	uint8_t buf[2];
	buf[0] = (uint8_t)(ofst >> 8);
	buf[1] = (uint8_t)ofst;
	status = nk_i2c_write_nostop(port, addr, 2, buf);
	if (status)
		return status;
	status = nk_i2c_read(port, addr, 2, buf);
	if (status)
		return status;
	*data = (buf[1] << 8) + (buf[0]);
	return 0;
}

int nk_i2c_get2_le24(void *port, uint8_t addr, uint16_t ofst, uint32_t *data)
{
	int status;
	uint8_t buf[3];
	buf[0] = (uint8_t)(ofst >> 8);
	buf[1] = (uint8_t)ofst;
	status = nk_i2c_write_nostop(port, addr, 2, buf);
	if (status)
		return status;
	status = nk_i2c_read(port, addr, 3, buf);
	if (status)
		return status;
	*data = (buf[2] << 16) + (buf[1] << 8) + (buf[0]);
	return 0;
}

int nk_i2c_put_beshort(void *port, uint8_t addr, uint8_t ofst, uint16_t data)
{
	uint8_t buf[3];
	buf[0] = ofst;
	buf[1] = (uint8_t)(data >> 8);
	buf[2] = (uint8_t)(data);
	return nk_i2c_write(port, addr, 3, buf);
}

int nk_i2c_put2_beshort(void *port, uint8_t addr, uint16_t ofst, uint16_t data)
{
	uint8_t buf[4];
	buf[0] = (uint8_t)(ofst >> 8);
	buf[1] = (uint8_t)ofst;
	buf[2] = (uint8_t)(data >> 8);
	buf[3] = (uint8_t)(data);
	return nk_i2c_write(port, addr, 4, buf);
}

int nk_i2c_put2_melong(void *port, uint8_t addr, uint16_t ofst, uint32_t data)
{
	uint8_t buf[6];
	buf[0] = (uint8_t)(ofst >> 8);
	buf[1] = (uint8_t)ofst;
	buf[2] = (uint8_t)(data >> 8);
	buf[3] = (uint8_t)(data);
	buf[4] = (uint8_t)(data >> 24);
	buf[5] = (uint8_t)(data >> 16);
	return nk_i2c_write(port, addr, 6, buf);
}

int nk_i2c_get_beshort(void *port, uint8_t addr, uint8_t ofst, uint16_t *data)
{
	int status;
	uint8_t buf[2];
	buf[0] = ofst;
	status = nk_i2c_write_nostop(port, addr, 1, buf);
	if (status)
		return status;
	status = nk_i2c_read(port, addr, 2, buf);
	if (status)
		return status;
	*data = (buf[0] << 8) + (buf[1]);
	return 0;
}

int nk_i2c_get2_beshort(void *port, uint8_t addr, uint16_t ofst, uint16_t *data)
{
	int status;
	uint8_t buf[2];
	buf[0] = (uint8_t)(ofst >> 8);
	buf[1] = (uint8_t)ofst;
	status = nk_i2c_write_nostop(port, addr, 2, buf);
	if (status)
		return status;
	status = nk_i2c_read(port, addr, 2, buf);
	*data = (buf[0] << 8) + (buf[1]);
	return 0;
}

/* [w AA DD ....] [r AA NN]
 *   optional write followed by optional read
 */


int nk_i2c_command(void *port, nkinfile_t *args)
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

#ifdef __SAME70Q21__

		// atsame70 at least does not support 0 length (address only) writes, so trying reading one byte from
		// each possible slave instead

		for (addr = 8; addr != 0x78; ++addr) {
			// atsame70 at least does not support 0 length (address only) writes
			int status;
			nk_printf("try %x\n", addr);
			write_array[0] = 0;
			write_array[1] = 0;
#if 1
			status = nk_i2c_write(port, addr, 2, write_array);
			nk_printf("write status = %x\n", status);
#endif
#if 0
			status  = nk_i2c_read(port, addr, 1, read_array);
			nk_printf("read status = %x\n", status);
#endif
			if (!status) {
				nk_printf("Found device %x\n", addr);
			}
		}
#else
		for (addr = 8; addr != 0x78; ++addr) {
			int status  = nk_i2c_write(port, addr, 0, write_array);
			if (!status) {
				nk_printf("Found device %x\n", addr);
			}
		}
#endif
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
			status = nk_i2c_write_nostop(port, write_array[0], write_len - 1, write_array + 1);
		else
			status = nk_i2c_write(port, write_array[0], write_len - 1, write_array + 1);
		if (status) {
			nk_printf(" I2C write failed");
			read_len = 0;
		}
	}
	if (read_len) {
                size_t y;
		nk_printf(" read addr=%x len=%lx", read_addr, (unsigned long)read_len);
		status = nk_i2c_read(port, read_addr, read_len, read_array);
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
