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

// Locate database in MCU flash memory

// FPGA attemps the load FLASH_PFGA_BUFFER_MAIN first.  If it has bad CRC, it then loads
// FLASH_GOLDEN_JUMP which commands it to load from FLASH_FPGA_BUFFER_GOLDEN.

// There are two copies of the RISC-V firmware: FLASH_FIRMWARE_BUFFER_0 and FLASH_FIRMWARE_BUFFER_1.
// Bit 0 of first byte of FLASH_FIRMWARE_FLAG indicates which of these will be used for
// execute in place on next boot up.

#define FLASH_FPGA_BUFFER_MAIN (1024*1024*0)
#define FLASH_FPGA_BUFFER_GOLDEN (1024*1024*8)
#define FLASH_FPGA_BUFFER_SIZE (1024*1024*6)

#define FLASH_FIRMWARE_BUFFER_0 (1024*1024*6)
#define FLASH_FIRMWARE_BUFFER_1 (1024*1024*14)
#define FLASH_FIRMWARE_BUFFER_SIZE (1024*1024)

#define FLASH_KEYVAL_SIZE 8192
#define FLASH_CAL_KEYVAL_ADDR_1 (1024*1024*15 + 12*65536)
#define FLASH_CAL_KEYVAL_ADDR_2 (1024*1024*15 + 13*65536)
// Bit zero of first byte indicates which firmware to use: 0 or 1
#define FLASH_FIRMWARE_FLAG (1024*1024*15 + 14*65536)

// Jump command is here for FPGA golden image
#define FLASH_GOLDEN_JUMP (1024*1024*15 + 15*65536 + 255*256)
