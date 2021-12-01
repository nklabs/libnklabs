// Simple flash-based schema-defined database

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

#ifndef _nkdbase
#define _nkdbase

#include <stdint.h>
#include "nkserialize.h"

// Database definition
// These are all constants

struct nk_dbase {
	const struct type *ty; // Schema describing the data layout
	const uint32_t bank0; // Flash address of bank0
	const uint32_t bank1; // Flash address of bank1
	const uint32_t bank_size; // Size of each bank
	unsigned char * const bigbuf; // Buffer large enough to hold serialized database: this can be shared
	const uint32_t bigbuf_size; // Size of above buffer: this size is used for flash_erase and flash_read
	// Flash access functions
	// These should all return 0 for success
	int (* const flash_read)(uint32_t addr, uint8_t *buf, uint32_t size);
	int (* const flash_erase)(uint32_t addr, uint32_t size);
	int (* const flash_write)(uint32_t addr, uint8_t *buf, uint32_t size);
	// Flash write granularity
	//  flash_writes are padded to a multiple of this size
	//  flash_granularity must be a power of 2 (including 2^0 == 1)
	const uint32_t flash_granularity;
};

// Bump version number and save a database from RAM to flash
// Returns zero for success

int nk_dbase_save(
	const struct nk_dbase *dbase,
	char *rev, // Address of version number
	void *ram // Address of database in RAM
);

// Load a database from flash to RAM
// Returns zero for success

int nk_dbase_load(
	const struct nk_dbase *dbase,
	char *rev, // Address of version number
	void *ram // Address of database in RAM
);

#endif
