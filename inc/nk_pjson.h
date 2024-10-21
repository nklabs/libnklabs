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

// nk_pjson - Print JSON formatted text.

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "nkprintf.h"
#include "nk_pjson_config.h"

typedef enum
{
        NK_JSON_LEVEL_OBJ = 0,
        NK_JSON_LEVEL_ARRAY
} json_level_type_t;

typedef struct
{
    nkoutfile_t *file;
    int32_t level; // Current level
    json_level_type_t type; // Type of current level
    bool first; // Set after we have a single item
    json_level_type_t stack[NK_JSON_STACK_SIZE];
} json_formatter_t;

/* Pop all level and begin printing a new JSON object */
void nk_pjson_begin(nkoutfile_t *file);

/* Get current level.  Right after nk_pjson_begin(), the level is 1. */
int32_t nk_pjson_getLevel();

/* Begin a new named object, return the old level for use with */
int32_t nk_pjson_obj(const char *name);

/* Begin a new named array, return the old level for use with nk_pjson_popto() */
int32_t nk_pjson_array(const char *name);

/* Print various simple values */
/* If you are in the body of an array, the names are suppressed, ok to use NULL for names */
void nk_pjson_null(const char *name);

void nk_pjson_bool(const char *name, bool value);

void nk_pjson_string(const char *name, const char *value);

void nk_pjson_double(const char *name, double value);

void nk_pjson_int32(const char *name, int32_t value);

void nk_pjson_uint32(const char *name, uint32_t value);

void nk_pjson_int64(const char *name, int64_t value);

void nk_pjson_uint64(const char *name, uint64_t value);

/* Pop one level */
void nk_pjson_pop();

/* Pop to specified level */
void nk_pjson_popto(int32_t level);

/* Pop all levels */
void nk_pjson_end();
