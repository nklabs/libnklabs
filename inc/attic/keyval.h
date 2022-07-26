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

#ifndef _KEYVAL_H
#define  _KEYVAL_H

// Value types

enum val_type {
    tVOID, // Nothing
    tNUM, // Integer
    tFP, // Double precision floating point
    tSTR, // String
    tHASH, // A hash table
    tARRAY, // An array
    tBOOL, // True or false
    tTABLE // A table
};

// Generic data

struct data {
    uint8_t *data; // Address of data
    size_t len; // Length of data
};

// Value

struct value {
    enum val_type what; // Value type
    union {
        struct keyval_hash *hash; // tHASH
        struct keyval_array *array; // tARRAY
        struct keyval_table *table; // tTABLE
        struct data data; // tDATA
        double fp; // tFP
        int num; // tNUM
        char *str; // tSTR
        int truth; // tBOOL
    } u;
};

// A table

struct keyval_table {
    int ncols; // How many columns
    int nrows; // How many rows
    char **colname; // Column headers
    struct value **row; // Rows
    int col_idx; // Index to most recently added field
};

// A key/value pair

struct keyval_entry {
    char *name;
    struct value value;
};

// A hash table

struct keyval_hash {
    struct keyval_entry *str_tab; // Hash table (size is always a power of 2)
    int str_tab_mask; // Hash table size - 1
    int str_tab_count; // Number of items in table
};

// An array

struct keyval_array {
    struct value *ary; // Array of values
    int ary_size; // Size of malloc block array is in
    int ary_len; // One plus highest numbered member in array
};

struct keyval_table *alloc_table();
struct keyval_table *alloc_table_sized(int nrows, int ncols);
void free_table(struct keyval_table *a);
void table_set_colname(struct keyval_table *t, int x, char *name);
void table_add_col(struct keyval_table *a, char  *name);
void table_add_row(struct keyval_table *a);
void table_add_field(struct keyval_table *a, struct value v);
void table_set_field(struct keyval_table *t, int row, int col, struct value v);

unsigned hash(const char *s);

struct value *get_by_string(struct keyval_hash *obj, const char *name);
struct value *set_by_string(struct keyval_hash *obj, const char *name);
struct keyval_hash *alloc_hash();
void free_hash(struct keyval_hash *obj);

struct value *get_by_number(struct keyval_array *t, int num);
struct value *set_by_number(struct keyval_array *t, int num);
struct keyval_array *alloc_array();
void free_array(struct keyval_array *o);

void free_val(struct value *v);

#endif      /* _KEYVAL_H   */
