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

/* Handles are just pointers to FPGA registers */

#ifndef _I2C_MASTER_H
#define _I2C_MASTER_H

typedef uint32_t volatile * const i2c_handle;

void i2c_master_init(void);

/* Write address then write bytes 
 * Return 0 for success or -1 for failure
 */
int i2c_master_write(i2c_handle port, uint8_t addr, size_t len, const uint8_t *data);
int i2c_master_write_nostop(i2c_handle port, uint8_t addr, size_t len, const uint8_t *data);

int dual_i2c_master_write(i2c_handle port, uint8_t addr, size_t len, const uint8_t *data);
int dual_i2c_master_write_nostop(i2c_handle port, uint8_t addr, size_t len, const uint8_t *data);

/* Write address then read bytes
 * Return 0 for success or -1 for failure
 */
int i2c_master_read(i2c_handle port, uint8_t addr, size_t len, uint8_t *data);
int dual_i2c_master_read(i2c_handle port, uint8_t addr, size_t len, uint8_t *data);


/* Write byte to register at offset, where offset is a single byte
 * Return 0 for success or -1 for failure
 */
int i2c_master_put_byte(i2c_handle port, uint8_t addr, uint8_t ofst, uint8_t data);

/* Write byte to register at offset, where offset is two bytes
 * Return 0 for success or -1 for failure
 */
int i2c_master_put2_byte(i2c_handle port, uint8_t addr, uint16_t ofst, uint8_t data);
int dual_i2c_master_put2_byte(i2c_handle port, uint8_t addr, uint16_t ofst, uint8_t data);

/* Read byte from register at offset, where offset is 1 byte
 * Return 0 for success or -1 for failure
 */
int i2c_master_get_byte(i2c_handle port, uint8_t addr, uint8_t ofst, uint8_t *byte);

/* Read byte from register at offset, where offset is 2 bytes
 * Return 0 for success or -1 for failure
 */
int i2c_master_get2_byte(i2c_handle port, uint8_t addr, uint16_t ofst, uint8_t *byte);
int dual_i2c_master_get2_byte(i2c_handle port, uint8_t addr, uint16_t ofst, uint8_t *byte);


/* Write 16-bit short to register at offset, where offset is a single byte
 * Return 0 for success or -1 for failure
 */
int i2c_master_put_leshort(i2c_handle port, uint8_t addr, uint8_t ofst, uint16_t data);

/* Write 16-bit short to register at offset, where offset is two bytes
 * Return 0 for success or -1 for failure
 */
int i2c_master_put2_leshort(i2c_handle port, uint8_t addr, uint16_t ofst, uint16_t data);
int dual_i2c_master_put2_leshort(i2c_handle port, uint8_t addr, uint16_t ofst, uint16_t data);

/* Write 24-bit to register at offset, where offset is two bytes
 * Return 0 for success or -1 for failure
 */
int i2c_master_put2_le24(i2c_handle port, uint8_t addr, uint16_t ofst, uint32_t data);
int dual_i2c_master_put2_le24(i2c_handle port, uint8_t addr, uint16_t ofst, uint32_t data);

/* Read 16-bit short from register at offset, where offset is 1 byte
 * Return 0 for success or -1 for failure
 */
int i2c_master_get_leshort(i2c_handle port, uint8_t addr, uint8_t ofst, uint16_t *byte);

/* Read 16-bit short from register at offset, where offset is 2 bytes
 * Return 0 for success or -1 for failure
 */
int i2c_master_get2_leshort(i2c_handle port, uint8_t addr, uint16_t ofst, uint16_t *byte);
int dual_i2c_master_get2_leshort(i2c_handle port, uint8_t addr, uint16_t ofst, uint16_t *byte);

/* Read 24-bit from register at offset, where offset is 2 bytes
 * Return 0 for success or -1 for failure
 */
int i2c_master_get2_le24(i2c_handle port, uint8_t addr, uint16_t ofst, uint32_t *data);
int dual_i2c_master_get2_le24(i2c_handle port, uint8_t addr, uint16_t ofst, uint32_t *data);

/* Write 16-bit big endian short to register at offset, where offset is a single byte
 * Return 0 for success or -1 for failure
 */
int i2c_master_put_beshort(i2c_handle port, uint8_t addr, uint8_t ofst, uint16_t data);

/* Write 16-bit big endian short to register at offset, where offset is two bytes
 * Return 0 for success or -1 for failure
 */
 int i2c_master_put2_beshort(i2c_handle port, uint8_t addr, uint16_t ofst, uint16_t data);

/* Read 16-bit big endian short from register at offset, where offset is 1 byte
 * Return 0 for success or -1 for failure
 */
int i2c_master_get_beshort(i2c_handle port, uint8_t addr, uint8_t ofst, uint16_t *byte);

/* Read 16-bit big endian short from register at offset, where offset is 2 bytes
 * Return 0 for success or -1 for failure
 */
int i2c_master_get2_beshort(i2c_handle port, uint8_t addr, uint16_t ofst, uint16_t *byte);

/* Write 32-bit mixed endian long to register at offset, where offset is two bytes
 * Return 0 for success or -1 for failure
 */
 int i2c_master_put2_melong(i2c_handle port, uint8_t addr, uint16_t ofst, uint32_t data);


/* Existing handles */
extern i2c_handle cam1_i2c_handle;
extern i2c_handle cam2_i2c_handle;
extern i2c_handle disp1_i2c_handle;
extern i2c_handle disp2_i2c_handle;
extern i2c_handle pmic_i2c_handle;
extern i2c_handle temp_i2c_handle;
extern i2c_handle ar_i2c_handle;

/* Generic i2c command */
int i2c_command(i2c_handle hand, char *args);
int dual_i2c_command(i2c_handle hand, char *args);

#endif    /* _I2C_MASTER_H   */
