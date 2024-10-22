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

// nk_pjson - Print JSON formatted text

#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "nk_pjson.h"

json_formatter_t __nk_std_json = {
    .level = 0,
    .type = NK_JSON_LEVEL_OBJ,
    .first = false,
    .file = &__nkstdout /* nkstdout is a variable initialized to this, libc uses a macro for stdout */
};

json_formatter_t *nk_std_json = &__nk_std_json;

/* If we are not already emitting JSON, start it and return true.
 * If we are already emitting JSON, do nothing and return false.
 */

static bool _nk_fpjson_start(json_formatter_t *json)
{
    if (!json->level)
    {
        nk_fprintf(json->file, "{");
        json->level = 1;
        json->first = false;
        json->type = NK_JSON_LEVEL_OBJ;
        return 1;
    }
    else
    {
        return 0;
    }
}

void nk_fpjson_init(json_formatter_t *json, nkoutfile_t *file)
{
    json->level = 0;
    json->type = NK_JSON_LEVEL_OBJ;
    json->first = false;
    json->file = file;
}

/* End current JSON outermost object (if any), and start a new one.
   Use this to start emitting JSON
*/

void nk_fpjson_begin(json_formatter_t *json)
{
    nk_fpjson_end(json);
    _nk_fpjson_start(json);
}

/* Called before printing a value: this emits the proper indentation and separation */

static void _nk_fpjson_prefix(json_formatter_t *json, bool close)
{
    int32_t i;

    // Auto-Start JSON
    if (!close && !json->level)
    {
        _nk_fpjson_start(json);
    }

    // Finish previous item

    if (!close && json->first)
    {
        nk_fprintf(json->file, ",\n");
    }
    else
    {
        nk_fprintf(json->file, "\n");
    }

    // Start new item

    for (i = 0; i != json->level; ++i)
    {
        nk_fprintf(json->file, "    ");
    }

    // Note that we printed something in the level

    json->first = 1;
}

/* Start emitting a JSON object.  Return old level (for use with nk_pjson_popto) */

int32_t nk_fpjson_obj(json_formatter_t *json, const char *name)
{
    int32_t old = json->level;
    if (json->level != NK_JSON_STACK_SIZE)
    {
        _nk_fpjson_prefix(json, false);
        if (json->type == NK_JSON_LEVEL_OBJ)
        {
            nk_fprintf(json->file, "\"%s\" : {", name);
        }
        else
        {
            nk_fprintf(json->file, "{");
        }
        json->stack[json->level] = json->type;
        json->type = NK_JSON_LEVEL_OBJ;
        json->first = false;
        json->level++;
    }
    else
    {
        nk_fpjson_string(json, "oops", "JSON stack");
    }
    return old;
}

/* Start emitting a JSON array.  Return old level (for use with nk_pjson_popto) */

int32_t nk_fpjson_array(json_formatter_t *json, const char *name)
{
    int32_t old = json->level;
    if (json->level != NK_JSON_STACK_SIZE)
    {
        _nk_fpjson_prefix(json, false);
        if (json->type == NK_JSON_LEVEL_OBJ)
        {
            nk_fprintf(json->file, "\"%s\" : [", name);
        }
        else
        {
            nk_fprintf(json->file, "[");
        }
        json->stack[json->level] = json->type;
        json->type = NK_JSON_LEVEL_ARRAY;
        json->first = false;
        json->level++;
    }
    else
    {
        nk_pjson_string("oops", "JSON stack");
    }
    return old;
}

/* Emit an integer */

void nk_fpjson_int32(json_formatter_t *json, const char *name, int32_t value)
{
    _nk_fpjson_prefix(json, false);
    if (json->type == NK_JSON_LEVEL_OBJ)
    {
        nk_fprintf(json->file, "\"%s\" : %" PRId32, name, value);
    }
    else
    {
        nk_fprintf(json->file, "%" PRId32, value);
    }
}

/* Emit a null */

void nk_fpjson_null(json_formatter_t *json, const char *name)
{
    _nk_fpjson_prefix(json, false);
    if (json->type == NK_JSON_LEVEL_OBJ)
    {
        nk_fprintf(json->file, "\"%s\" : null", name);
    }
    else
    {
        nk_fprintf(json->file, "null");
    }
}

/* Emit an unsigned integer */

void nk_fpjson_uint32(json_formatter_t *json, const char *name, uint32_t value)
{
    _nk_fpjson_prefix(json, false);
    if (json->type == NK_JSON_LEVEL_OBJ)
    {
        nk_fprintf(json->file, "\"%s\" : %" PRIu32, name, value);
    }
    else
    {
        nk_fprintf(json->file, "%" PRIu32, value);
    }
}

/* Emit a 64-bit integer */

void nk_fpjson_int64(json_formatter_t *json, const char *name, int64_t value)
{
    _nk_fpjson_prefix(json, false);
    if (json->type == NK_JSON_LEVEL_OBJ)
    {
        nk_fprintf(json->file, "\"%s\" : %" PRId64, name, value);
    }
    else
    {
        nk_fprintf(json->file, "%" PRId64, value);
    }
}

/* Emit an unsigned 64-bit integer */

void nk_fpjson_uint64(json_formatter_t *json, const char *name, uint64_t value)
{
    _nk_fpjson_prefix(json, false);
    if (json->type == NK_JSON_LEVEL_OBJ)
    {
        nk_fprintf(json->file, "\"%s\" : %" PRIu64, name, value);
    }
    else
    {
        nk_fprintf(json->file, "%" PRIu64, value);
    }
}

/* Emit a boolean */

void nk_fpjson_bool(json_formatter_t *json, const char *name, bool value)
{
    _nk_fpjson_prefix(json, false);
    if (json->type == NK_JSON_LEVEL_OBJ)
    {
        nk_fprintf(json->file, "\"%s\" : %s", name, value ? "true" : "false");
    }
    else
    {
        nk_fprintf(json->file, "%s", value ? "true" : "false");
    }
}

/* Emit a string */

void nk_fpjson_string(json_formatter_t *json, const char *name, const char *value)
{
    _nk_fpjson_prefix(json, false);
    if (json->type == NK_JSON_LEVEL_OBJ)
    {
        nk_fprintf(json->file, "\"%s\" : \"%s\"", name, value);
    }
    else
    {
        nk_fprintf(json->file, "\"%s\"", value);
    }
}

/* Emit a double */

void nk_fpjson_double(json_formatter_t *json, const char *name, double value)
{
    _nk_fpjson_prefix(json, false);
    if (json->type == NK_JSON_LEVEL_OBJ)
    {
        nk_fprintf(json->file, "\"%s\" : %g", name, value);
    }
    else
    {
        nk_fprintf(json->file, "%g", value);
    }
}

/* Mark end of JSON object or array */

void nk_fpjson_pop(json_formatter_t *json)
{
    if (json->level)
    {
        --json->level;
        _nk_fpjson_prefix(json, true);
        if (json->type == NK_JSON_LEVEL_OBJ)
        {
            nk_fprintf(json->file, "}");
        }
        else if (json->type == NK_JSON_LEVEL_ARRAY)
        {
            nk_fprintf(json->file, "]");
        }
        json->type = json->stack[json->level];
    }
}

/* End JSON objects/arrays until we are at the desired level */

void nk_fpjson_popto(json_formatter_t *json, int32_t level)
{
    while (json->level > level)
    {
        nk_fpjson_pop(json);
    }
}

/* End all objects/array including outermost JSON object */

void nk_fpjson_end(json_formatter_t *json)
{
    if (json->level)
    {
        while (json->level)
        {
            nk_fpjson_pop(json);
        }
        nk_fprintf(json->file, "\n");
    }
}
