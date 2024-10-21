# JSON formatter

## Files

[nk_pjson.h](../inc/nk_pjson.h),
[nk_pjson.c](../src/nk_pjson.c),

## Description

```c
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
```

Provides functions for printing JSON-formatted data.

This was original designed so that CLI-command results can be emitted on the
serial console so that scripts running on some host computer can easily
parse the results.

But it can also be used to write JSON to any nkoutfile_t.

The code is not reentrant.  Only one target nkoutfile_t can be written to at
a time.

Example use:

```c
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

You can leave out the nk_pjson_begin()- all of the printing functions automatically
call it.

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
```

Output:

```json
    {
        "test" : "Hello, world"
        "foo" : {
            "first", : 1,
            "second", : 2
        }
       "bar" : [
           1,
           2,
           3
       ]
    }
```

As stated above, we did not call nk_pjson_begin().  Likewise, we did not
call nk_pjson_end().

When you print values form within an array, the name is ignored.

Normally you should provide a nk_pjson_pop() to terminate each object or array.  But
it's OK if you leave out the last one(s).  nk_pjson_end() automatically closes the whole
stack of open arrays or objects.
