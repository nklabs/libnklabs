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

json_formatter_t json_top = {
    .level = 0,
    .type = NK_JSON_LEVEL_OBJ,
    .first = false
};

/* If we are not already emitting JSON, start it and return true.
 * If we are already emitting JSON, do nothing and return false.
 */

static bool _nk_pjson_start()
{
    if (!json_top.level)
    {
        nk_fprintf(json_top.file, "{");
        json_top.level = 1;
        json_top.first = false;
        json_top.type = NK_JSON_LEVEL_OBJ;
        return 1;
    }
    else
    {
        return 0;
    }
}

/* End current JSON outermost object (if any), and start a new one.
   Use this to start emitting JSON
*/

void nk_pjson_begin(nkoutfile_t *file)
{
    nk_pjson_end();
    json_top.file = file;
    _nk_pjson_start();
}

/* Called before printing a value: this emits the proper indentation and separation */

static void _nk_pjson_prefix(bool close)
{
    int32_t i;

    // Auto-Start JSON
    if (!close && !json_top.level)
    {
        _nk_pjson_start();
    }

    // Finish previous item

    if (!close && json_top.first)
    {
        nk_fprintf(json_top.file, ",\n");
    }
    else
    {
        nk_fprintf(json_top.file, "\n");
    }

    // Start new item

    for (i = 0; i != json_top.level; ++i)
    {
        nk_fprintf(json_top.file, "    ");
    }

    // Note that we printed something in the level

    json_top.first = 1;
}

/* Start emitting a JSON object.  Return old level (for use with nk_pjson_popto) */

int32_t nk_pjson_obj(const char *name)
{
    int32_t old = json_top.level;
    if (json_top.level != NK_JSON_STACK_SIZE)
    {
        _nk_pjson_prefix(false);
        if (json_top.type == NK_JSON_LEVEL_OBJ)
        {
            nk_fprintf(json_top.file, "\"%s\" : {", name);
        }
        else
        {
            nk_fprintf(json_top.file, "{");
        }
        json_top.stack[json_top.level] = json_top.type;
        json_top.type = NK_JSON_LEVEL_OBJ;
        json_top.first = false;
        json_top.level++;
    }
    else
    {
        nk_pjson_string("oops", "JSON stack");
    }
    return old;
}

/* Start emitting a JSON array.  Return old level (for use with nk_pjson_popto) */

int32_t nk_pjson_array(const char *name)
{
    int32_t old = json_top.level;
    if (json_top.level != NK_JSON_STACK_SIZE)
    {
        _nk_pjson_prefix(false);
        if (json_top.type == NK_JSON_LEVEL_OBJ)
        {
            nk_fprintf(json_top.file, "\"%s\" : [", name);
        }
        else
        {
            nk_fprintf(json_top.file, "[");
        }
        json_top.stack[json_top.level] = json_top.type;
        json_top.type = NK_JSON_LEVEL_ARRAY;
        json_top.first = false;
        json_top.level++;
    }
    else
    {
        nk_pjson_string("oops", "JSON stack");
    }
    return old;
}

/* Emit an integer */

void nk_pjson_int32(const char *name, int32_t value)
{
    _nk_pjson_prefix(false);
    if (json_top.type == NK_JSON_LEVEL_OBJ)
    {
        nk_fprintf(json_top.file, "\"%s\" : %" PRId32, name, value);
    }
    else
    {
        nk_fprintf(json_top.file, "%" PRId32, value);
    }
}

/* Emit a null */

void nk_pjson_null(const char *name)
{
    _nk_pjson_prefix(false);
    if (json_top.type == NK_JSON_LEVEL_OBJ)
    {
        nk_fprintf(json_top.file, "\"%s\" : null", name);
    }
    else
    {
        nk_fprintf(json_top.file, "null");
    }
}

/* Emit an unsigned integer */

void nk_pjson_uint32(const char *name, uint32_t value)
{
    _nk_pjson_prefix(false);
    if (json_top.type == NK_JSON_LEVEL_OBJ)
    {
        nk_fprintf(json_top.file, "\"%s\" : %" PRIu32, name, value);
    }
    else
    {
        nk_fprintf(json_top.file, "%" PRIu32, value);
    }
}

/* Emit a 64-bit integer */

void nk_pjson_int64(const char *name, int64_t value)
{
    _nk_pjson_prefix(false);
    if (json_top.type == NK_JSON_LEVEL_OBJ)
    {
        nk_fprintf(json_top.file, "\"%s\" : %" PRId64, name, value);
    }
    else
    {
        nk_fprintf(json_top.file, "%" PRId64, value);
    }
}

/* Emit an unsigned 64-bit integer */

void nk_pjson_uint64(const char *name, uint64_t value)
{
    _nk_pjson_prefix(false);
    if (json_top.type == NK_JSON_LEVEL_OBJ)
    {
        nk_fprintf(json_top.file, "\"%s\" : %" PRIu64, name, value);
    }
    else
    {
        nk_fprintf(json_top.file, "%" PRIu64, value);
    }
}

/* Emit a boolean */

void nk_pjson_bool(const char *name, bool value)
{
    _nk_pjson_prefix(false);
    if (json_top.type == NK_JSON_LEVEL_OBJ)
    {
        nk_fprintf(json_top.file, "\"%s\" : %s", name, value ? "true" : "false");
    }
    else
    {
        nk_fprintf(json_top.file, "%s", value ? "true" : "false");
    }
}

/* Emit a string */

void nk_pjson_string(const char *name, const char *value)
{
    _nk_pjson_prefix(false);
    if (json_top.type == NK_JSON_LEVEL_OBJ)
    {
        nk_fprintf(json_top.file, "\"%s\" : \"%s\"", name, value);
    }
    else
    {
        nk_fprintf(json_top.file, "\"%s\"", value);
    }
}

/* Emit a double */

void nk_pjson_double(const char *name, double value)
{
    _nk_pjson_prefix(false);
    if (json_top.type == NK_JSON_LEVEL_OBJ)
    {
        nk_fprintf(json_top.file, "\"%s\" : %g", name, value);
    }
    else
    {
        nk_fprintf(json_top.file, "%g", value);
    }
}

/* Mark end of JSON object or array */

void nk_pjson_pop()
{
    if (json_top.level)
    {
        --json_top.level;
        _nk_pjson_prefix(true);
        if (json_top.type == NK_JSON_LEVEL_OBJ)
        {
            nk_fprintf(json_top.file, "}");
        }
        else if (json_top.type == NK_JSON_LEVEL_ARRAY)
        {
            nk_fprintf(json_top.file, "]");
        }
        json_top.type = json_top.stack[json_top.level];
    }
}

/* End JSON objects/arrays until we are at the desired level */

void nk_pjson_popto(int32_t level)
{
    while (json_top.level > level)
    {
        nk_pjson_pop();
    }
}

/* End all objects/array including outermost JSON object */

void nk_pjson_end()
{
    if (json_top.level)
    {
        while (json_top.level)
        {
            nk_pjson_pop();
        }
        nk_fprintf(json_top.file, "\n");
    }
}
