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

extern json_formatter_t *nk_std_json;

/* Initialize json_formatter_t structure */
void nk_fpjson_init(json_formatter_t *json, nkoutfile_t *file);
#define nk_pjson_init(file) nk_fpjson_init(nk_std_json, (file))

/* Pop all level and begin printing a new JSON object */
void nk_fpjson_begin(json_formatter_t *json);
#define nk_pjson_begin() nk_fpjson_begin(nk_std_json)

/* Get current level.  Right after nk_pjson_begin(), the level is 1. */
int32_t nk_fpjson_getlevel(json_formatter_t *json);
#define nk_pjson_getlevel() nk_fpjson_getlevel(nk_std_json)

/* Begin a new named object, return the old level for use with */
int32_t nk_fpjson_obj(json_formatter_t *json, const char *name);
#define nk_pjson_obj(name) nk_fpjson_obj(nk_std_json, (name))

/* Begin a new named array, return the old level for use with nk_pjson_popto() */
int32_t nk_fpjson_array(json_formatter_t *json, const char *name);
#define nk_pjson_array(name) nk_fpjson_array(nk_std_json, (name))

/* Print various simple values */
/* If you are in the body of an array, the names are suppressed, ok to use NULL for names */
void nk_fpjson_null(json_formatter_t *json, const char *name);
#define nk_pjson_null(name) nk_fpjson_null(nk_std_json, (name))

void nk_fpjson_bool(json_formatter_t *json, const char *name, bool value);
#define nk_pjson_bool(name, value) nk_fpjson_bool(nk_std_json, (name), (value))

void nk_fpjson_string(json_formatter_t *json, const char *name, const char *value);
#define nk_pjson_string(name, value) nk_fpjson_string(nk_std_json, (name), (value))

void nk_fpjson_double(json_formatter_t *json, const char *name, double value);
#define nk_pjson_double(name, value) nk_fpjson_double(nk_std_json, (name), (value))

void nk_fpjson_int32(json_formatter_t *json, const char *name, int32_t value);
#define nk_pjson_int32(name, value) nk_fpjson_int32(nk_std_json, (name), (value))

void nk_fpjson_uint32(json_formatter_t *json, const char *name, uint32_t value);
#define nk_pjson_uint32(name, value) nk_fpjson_uint32(nk_std_json, (name), (value))

void nk_fpjson_int64(json_formatter_t *json, const char *name, int64_t value);
#define nk_pjson_int64(name, value) nk_fpjson_int64(nk_std_json, (name), (value))

void nk_fpjson_uint64(json_formatter_t *json, const char *name, uint64_t value);
#define nk_pjson_uint64(name, value) nk_fpjson_uint64(nk_std_json, (name), (value))

/* Pop one level */
void nk_fpjson_pop(json_formatter_t *json);
#define nk_pjson_pop() nk_fpjson_pop(nk_std_json)

/* Pop to specified level */
void nk_fpjson_popto(json_formatter_t *json, int32_t level);
#define nk_pjson_popto(level) nk_fpjson_popto(nk_std_json, (level))

/* Pop all levels */
void nk_fpjson_end(json_formatter_t *json);
#define nk_pjson_end() nk_fpjson_end(nk_std_json)
