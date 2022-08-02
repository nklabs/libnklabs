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

#ifndef _Inkcrclib
#define _Inkcrclib

#include <inttypes.h>
#include <stddef.h>

/* Poly is 0x04c11db7 (this is used for Ethernet) */
/* Big endian, MSB first version */
/* When you append the calculated CRC to a file (MSByte first), the CRC of the result will be 0 */
uint32_t nk_crc32be_update(uint32_t accu, uint8_t byte);

// Compute CRC of an array.  Result is 0 if CRC is good.
uint32_t nk_crc32be_check(uint8_t *start, size_t size);


/* Poly is 0xedb88320 (this is used for ZMODEM) */
/* Little endian, LSB first version */
/* When you append the calculated CRC to a file (LSByte first), the CRC of the result will be 0 */
uint32_t nk_crc32le_update(uint32_t accu, uint8_t byte);

// Compute CRC of an array.  Result is 0 if CRC is good.
uint32_t nk_crc32le_check(uint8_t *start, size_t size);


// CRC-16-CCITT (poly is 0x1021) (this is used for XMODEM / YMODEM)
// Big endian version: append crc to data, most significant byte first.  CRC of result will be zero.
uint16_t nk_crc16be_update(uint16_t crc, uint8_t ch);

// Compute CRC of an array.  Result is 0 if CRC is good.
// (initial crc is 0x0000)
uint16_t nk_crc16be_check(const uint8_t *data_p, size_t length);


// CRC-16-CCITT (poly is 0x8408)
// Little endian version: append crc to data, least significant byte first.  CRC of result will be zero.
uint16_t nk_crc16le_update(uint16_t crc, uint8_t ch);

// Compute CRC of an array.  Result is 0 if CRC is good.
// (initial crc is 0x0000)
uint16_t nk_crc16le_check(const uint8_t* data_p, size_t length);


// Poly is 0x07 (as used in ST's energy monitor ICs)
// Compute CRC of an array.
uint8_t nk_crc8(const uint8_t *data, size_t length);

#endif
