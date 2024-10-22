# JSON formatter

## Files

[nk_pjson.h](../inc/nk_pjson.h),
[nk_pjson.c](../src/nk_pjson.c),
[nk_pjson_config.h](../config/nk_pjson_config.h),

## Description

```c
/* json_formatter_t set up to output to nkstdout */
extern json_formatter_t *nk_std_json;

/* Initialize json_formatter_t structure */
void nk_fpjson_init(json_formatter_t *json, nkoutfile_t *file);

/* Same as above, but initialize nk_std_json */
void nk_pjson_init(nkoutfile_t *file);

/* Pop all level and begin printing a new JSON object */
void nk_fpjson_begin(json_formatter_t *json);
void nk_pjson_begin();

/* Get current level.  Right after nk_pjson_begin(), the level is 1. */
int32_t nk_fpjson_getlevel(json_formatter_t *json);
int32_t nk_pjson_getlevel();

/* Begin a new named object, return the old level for use with */
int32_t nk_fpjson_obj(json_formatter_t *json, const char *name);
int32_t nk_pjson_obj(const char *name);

/* Begin a new named array, return the old level for use with nk_pjson_popto() */
int32_t nk_fpjson_array(json_formatter_t *json, const char *name);
int32_t nk_pjson_array(const char *name);

/* Print various simple values */
/* If you are in the body of an array, the names are suppressed, ok to use NULL for names */
void nk_fpjson_null(json_formatter_t *json, const char *name);
void nk_pjson_null(const char *name);

void nk_fpjson_bool(json_formatter_t *json, const char *name, bool value);
void nk_pjson_bool(const char *name, bool value);

void nk_fpjson_string(json_formatter_t *json, const char *name, const char *value);
void nk_pjson_string(const char *name, const char *value);

void nk_fpjson_double(json_formatter_t *json, const char *name, double value);
void nk_pjson_double(const char *name, double value);

void nk_fpjson_int32(json_formatter_t *json, const char *name, int32_t value);
void nk_pjson_int32(const char *name, int32_t value);

void nk_fpjson_uint32(json_formatter_t *json, const char *name, uint32_t value);
void nk_pjson_uint32(const char *name, uint32_t value);

void nk_fpjson_int64(json_formatter_t *json, const char *name, int64_t value);
void nk_pjson_int64(const char *name, int64_t value);

void nk_fpjson_uint64(json_formatter_t *json, const char *name, uint64_t value);
void nk_pjson_uint64(const char *name, uint64_t value);

/* Pop one level */
void nk_fpjson_pop(json_formatter_t *json);
void nk_pjson_pop();

/* Pop to specified level */
void nk_fpjson_popto(json_formatter_t *json, int32_t level);
void nk_pjson_popto(int32_t level);

/* Pop all levels */
void nk_fpjson_end(json_formatter_t *json);
void nk_pjson_end();
```

## Description

Provides functions for printing JSON-formatted data.

This was original designed so that CLI-command results can be emitted on the
serial console so that scripts running on some host computer can easily
parse the results.

But it can also be used to write JSON to any nkoutfile_t.

Two versions of each function are provided: one targetting a specific
json_formatter_t as the output target, and another targetting nk_std_json.

nk_pjson_config.h contains NK_JSON_STACK_SIZE, which sets the maximum object
or array depth of the produced JSON.

Example use:

```c
    nk_pjson_init(nkstdout);

    nk_pjson_begin();
    nk_pjson_string("test", "Hello, world!");
    nk_pjson_end();
```

Output:

```json
    {
        "test" : "Hello, world!"
    }
```

nk_pjson_init() sets up the nk_std_json, and is only needed if the output is
not nkstdout.

nk_pjson_begin() prints the opening '{', but this call is optional. 
Nk_pjson_begin() is automatically called by all of the printing functions
after nk_pjson_end() had been called or after initialization.

It's a good idea for your common CLI code to call nk_pjson_end() after CLI
functions return.  This way the user doesn't have to remember to do it.

Example with objects and arrays:

```c
     nk_pjson_string("test", "Hello, world!");
     nk_pjson_obj("foo");
     nk_pjson_int32("first", 1);
     nk_pjson_int32("second", 2);
     nk_pjson_pop(); // End of Object
     nk_pjson_array("bar");
     nk_pjson_int32("", 1);
     nk_pjson_int32("", 2); 
     nk_pjson_int32("", 3);
     // nk_pjson_pop(); // End of Array
     // nk_pjson_end(); // End of JSON
```

Output:

```json
    {
        "test" : "Hello, world"
        "foo" : {
            "first" : 1,
            "second" : 2
        }
       "bar" : [
           1,
           2,
           3
       ]
    }
```

As stated above, we did not call nk_pjson_begin(), relying instead on the
automatic call to it by nk_pjson_string().  Likewise, we did not call
nk_pjson_end(), assuming that it is called by common CLI code.

When you print values from within an array, the name is ignored.

Normally you should provide a nk_pjson_pop() to terminate each object or
array.  But it's OK to leave out the last one(s).  nk_pjson_end()
automatically closes the whole stack of open arrays or objects.
