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

#ifndef  _SCHEMA_KEYVAL_H
#define _SCHEMA_KEYVAL_H

enum val_type {
	tBOOL,		// true or false

	tINT,		// signed int

	tDOUBLE,	// double

	tFLOAT,

	tSTRING,	// a NUL-terminated C-string in a fixed length array

	tSTRUCT,	// a structure: members has list of members

	tARRAY,		// An array: subtype has type of each item

	tTABLE		// A table (an array of structs): subtype has structure defining columns
};

struct type {
	int what;			// Type code for this type
	size_t size;			// Size of this type in memory in bytes: use sizeof() for structs
	const struct member *members;	// Member list for structures
	const struct type *subtype;	// Array element type, table structure type
};

struct member {
	const char *name;		// Name of this member
	const struct type *type;	// Type of this member
	ptrdiff_t offset;		// Byte offset to it from parent
};

#define member_size(type, member) sizeof(((type *)0)->member)

// This is how we store table and array sizes... this way forces alignment

struct alignme {
	double xxx;
};

union len {
	int len;
	struct alignme alignme;
}; 

extern const struct type tyINT;
extern const struct type tyBOOL;
extern const struct type tyDOUBLE;
extern const struct type tyFLOAT;

int serialize_val(char *org, const struct type *type, void *location);

char *multi_gets();

void keyval_init();

int cmd_cal(char *args);
int cmd_model(char *args);
int cmd_config(char *args);

#endif    /* _SCHEMA_KEYVAL_H */
