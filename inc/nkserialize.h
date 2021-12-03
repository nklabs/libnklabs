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

#ifndef _nkserialize
#define _nkserialize

#include <stdint.h>
#include <stddef.h>
#include "nkoutfile.h"
#include "nkmacros.h"
#include "nkserialize_config.h"

enum val_type {
	tBOOL,		// true or false

	tINT,		// signed int
	tUINT,		// unsigned int
	tINT8,
	tUINT8,
	tINT16,
	tUINT16,

	tDOUBLE,	// double

	tFLOAT,

	tSTRING,	// a NUL-terminated C-string in a fixed length array

	tSTRUCT,	// a structure: members has list of members

	tARRAY,		// A fixed array: subtype has type of each item
	tVARRAY,	// A variable length array: subtype has type of each item

	tTABLE		// A table (an array of structs): subtype has structure defining columns
};

struct type {
	int what;			// Type code for this type
	size_t size;			// Size of this type in memory in bytes: use sizeof() for structs
	const struct member *members;	// Member list for structures
	const struct type *subtype;	// Array element type, table structure type
	int (*check)(size_t location);	// Check validity after parsing (return 0 for good)
};

struct member {
	const char *name;		// Name of this member
	const struct type *type;	// Type of this member
	size_t offset;			// Byte offset to it from parent
	uint32_t triggers;		// Trigger on modification bitmap
};

// This is how we store table and array sizes... this way forces alignment

struct alignme {
	double xxx;
};

union len {
	size_t len;
	struct alignme alignme;
}; 

extern const struct type tyINT;
extern const struct type tyUINT;
extern const struct type tyINT8;
extern const struct type tyUINT8;
extern const struct type tyINT16;
extern const struct type tyUINT16;
extern const struct type tyBOOL;
extern const struct type tyDOUBLE;
extern const struct type tyFLOAT;

// Serialize a data structure at the specified address (location) with the specified type.
// The serialized output is sent to the nkoutfile_t.
int nk_dbase_serialize(nkoutfile_t *f, const struct type *type, void *location);

// Same as above, but in a more human readable format
//  ind is number of space to indent the entire output
//  ed is string to print at line ends, usually "\n"
int nk_dbase_fprint(nkoutfile_t *f, const struct type *type, void *location, int ind, const char *ed);

// Locate a subset of a data structure by following an expression
const struct type *nk_xpath(char *key, const struct type *type, void **location_loc, uint32_t *triggers);

// Parse a serialized database- used by nkscan
int nk_fscan_keyval(nkinfile_t *f, const struct type *type, size_t location);

#endif
