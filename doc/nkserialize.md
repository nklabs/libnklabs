# NK Labs Serialization Library and Database

nkserialize is a schema-driven serialization / deserialization library
similar to Google's Protocol Buffers, but suitable for embedded systems with
limited RAM.  Heap memory is not required and malloc is not used.

Serialization means that we convert a C native data structure into a string. 
The string can be read or edited by humans, transferred over a network or
saved in a file or flash memory.  Deserialization means we parse a string
back into a native C data structure.

The serialized format looks very much like JSON.  Here is an example:

    {
      connector: {
        sp: {
          scale: 0.00251427,
          offset: 0.0
        },
        f: {
          scale: 0.00265934,
          offset: -0.416
        },
      },
      my_array: [
        1, 2, 3, 4, 5
      ],
      temp: {
        in: {
          scale: 0.0414627,
          offset: 0.0
        },
        ambient: {
          scale: 0.0414627,
          offset: 0.0
        }
      },
      voltage: {
        scale: 0.0163324,
        offset: 0.0
      },
      tinfo: {
        tempco: 1314,
        nom: 0,
        f_table:
          (	bias	limit	tempco	enable	driver
          :	2.0	4.0	0	0	0
          :	2.0	4.0	0	0	0
          :	2.0	4.0	1314	1	0
          :	2.0	4.0	1314	1	0
          :	2.0	4.0	1314	1	0
          :	2.0	4.0	1314	1	0
          :	2.0	4.0	0	0	0
          ),
      d_quiescent: 62.0,
      pwd: "6D7FD05041051F080F766F94C591C4B8397FA66CF5FC8AB9854050978A",
      salt: "00006C84F3A",
      serno: "1234"
    }

The following types are supported:

* Integers, such as -123
* Booleans, true or false.
* Floating point numbers, such as -1.3e4.
* Strings, such as "abc".  C language escape sequences are allowed within strings.
* Structures, such as { a:1, b:2, c:3 }.  The member names follow the rules for C identifiers.
* Arrays, such as [ "first", "second", "third" ].  The types of all the members must be the same, but they can be complex types such as structures.
* Tables, such as ( name, ssno : "joe", "123-45-6890" : "bill", "111-22-3333" ).  There is a header row specifying the column names followed by 0 or more data rows.  The deserialized format of a table is an array of structures.  Tables are printed so that they can be imported into spreadsheets.

nkserialize is schema driven.  This means that unlike JSON, a schema
determines which types and fields are expected in the serialized string.  If
you add an extra field to a structure for example, it will be ignored during
deserialization.

The schema is defined in C (in the future there may be a dedicated schema
language, but not yet).  For example, suppose the C data structure that you
want to serialize looks like this:

```c
struct top {
	float a;
	int b;
	char c[50];
};
```

The schema that describes this structure is as follows: 


```c

const struct type tyNAME = {
	.what = tSTRING,
	.size = member_size(struct top, c),
	.members = NULL,
	.subtype = NULL
};

const struct member top_members[] = {
	{ "a", &tyFLOAT, offsetof(struct top, a) },
	{ "b", &tyINT, offsetof(struct top, b) },
	{ "c", &tyNAME, offsetof(struct top, c) },
	{ NULL, NULL, 0 }
}

const struct type tyTOP = {
	.what = tSTRUCT,
	.size = sizeof(struct top),
	.members = top_members,
	.subtype = NULL
};
```

tyTOP indicates that the "struct top" is a C structure.  top_members define
the members of the structure: a, b and c.  Note that they supply the byte
offset to each member in the structure.  tyNAME is used for member c,
because it is a complex type.  tyNAME indicates that the type is a string
and that its maximum length is 50.

Notice that the schema is all marked as const.  This keeps the schema in
(the typically more expansive) flash memory of your microcontroller, so as
not to use up the limited RAM.

Default values can supplied to an instance of the structure in the normal
C-language way:

```c
struct top fred = {
	.a = 1.0,
	.b = 10,
	.c = "Hello"
}
```

If any of the members are missing from the string during deserialization,
the default values will remain in place.

The structure to be serialized can be arbitrarily complex- it may be made of
any number of levels of sub-structures and arrays.  The only limit is that
it must describe a fixed length contiguous region of memory, which means
that strings are stored within preallocated fixed length arrays of
characters (as with member c above) instead of as malloced strings on the
heap.

Strings have length which can be less than the maximum length.  This is
determined by the terminating NUL as usual.  But what about arrays and
tables?  For these, a separate integer containing the actual length always
precedes the array or table, for example for array 'd' and table 'e' below:

```c
struct my_table_entry {
	int x;
	int y;
};

struct top {
	float a;
	int b;
	char c[50];
	union len d_len;
	int d[7];
	union len e_len;
	struct my_table_entry e[20];
} foo;
```

A union is used to force a fixed memory offset between the length and the
following array (it contains a member which the C compiler is forced to
align).  To access the integer containing the length, use
foo.d_len.len.

d_len.len can have any of the values 0 through 7 inclusive. 
e_len.len can have of the values 0 through 20 inclusive.

The schema for arrays and tables looks like this:

```c
const struct member my_table_entry_members[] = {
	{ "x", &tyINT, offsetof(struct my_table_entry, x) },
	{ "y", &tyINT, offsetof(struct my_table_entry, y) },
	{ NULL, NULL, 0 }
};

const struct type tyMY_TABLE_ENTRY = {
	.what = tSTRUCT,
	.size = sizeof(struct my_table_entry),
	.members = my_table_entry_members,
	.subtype = NULL
};

const struct type tyMY_TABLE = {
	.what = tTABLE,
	.size = member_size(struct top, e),
	.members = NULL,
	.subtype = &tyMY_TABLE_ENTRY
};

const struc type tyMY_ARRAY = {
	.what = tARRAY,
	.size = member_size(struct top, d),
	.members = NULL,
	.subtype = &tyINT
};

const struct type tyNAME = {
	.what = tSTRING,
	.size = member_size(struct top, c),
	.members = NULL,
	.subtype = NULL
};

const struct member top_members[] = {
	{ "a", &tyFLOAT, offsetof(struct top, a) },
	{ "b", &tyINT, offsetof(struct top, b) },
	{ "c", &tyNAME, offsetof(struct top, c) },
	{ "d", &tyMY_ARRAY, offsetof(struct top, d_len) },
	{ "e", &tyMY_TABLE, offsetof(struct top, e_len) },
	{ NULL, NULL, 0 }
}

const struct type tyTOP = {
	.what = tSTRUCT,
	.size = sizeof(struct top),
	.members = top_members,
	.subtype = NULL
};

```

Notice that "d" and "e" of "top_members" refer to the length element, not
the array or table directly.

The subtype for a table must be tSTRUCT.  The subtype for an array can be
any type.

## Built-in types

There are a number of built-in types that can appear in the .subtype field
of "struct type" or in the .type field of struct member:

	tyINT		"int"
	tyUINT		"unsigned int"
	tyINT8          "char"
	tyUINT8		"unsigned char"
	tyINT16		"short"
	tyUINT16	"unsigned short"
	tyFLOAT		"float"
	tyDOUBLE	"double"
	tyBOOL		"int" (true is 1, false is 0)

## nk_dbase_serialize()

```c
int nk_dbase_serialize(nkoutfile_t *f, const struct type *type, void *location);
```

nk_dbase_serialize() is used to serialize a structure.  For example,

```c
char buffer[100];
nkoutfile_t f;

nkoutfile_open_mem(&f, buffer, sizeof(buffer)-1);
int status = nk_dbase_serialize(&f, &tyTOP, &fred);
nk_fputc(f, 0); // Terminating NUL
```

nk_dbase_serialize() requires an output file for the resulting string, the
schema for the structure (&tyTOP) and the structure's address (&fred).

nk_dbase_serialize() returns the output status (bit-wise OR of all calls to
nk_fputc).  0 means no errors.

## nk_scan()

```c
int nk_scan(char **ptr, const char *fmt, ...);
```

nk_scan is used to deserialize a string- to load data into a C structure. 
nk_scan is a generic string parser similar to sscanf (see the [nk_scan documentation](nkscan.md)).
It takes a format string, and includes a conversion
specifier for nkserialized data.  For example:

```c
nkinfile_t f;
nkinfile_open_string(&f, buffer);

if (nk_scan(&ptr, "%v ", &tyTOP, &fred))
	nk_printf("Deserialization OK!\n");
```

%v means that serialized data appears at that point in the input string. 
You must supply the schema (&tyTOP) and an address to where you want the
deserialized data to go (&fred in this case).

## nk_xpath()

```c
const struct type *nk_xpath(char *key, const struct type *type, void **location);

```

nk_xpath can be used to navigate deserialized data and its schema using an
expression syntax.  It is just like [XML XPath](https://www.w3schools.com/xml/xml_xpath.asp).
You can use it to focus to any subset of your structure.  For example:

```c
void *ptr = &fred;
struct type *b_type = nk_xpath("b", &tyTOP, &ptr);
```

"b" is the expression.  nk_xpath finds the schema of the "b" member (left in
b_type above).  It also finds the memory address of the "b" member of a
specific instance (left in ptr above, to find the address of the "b" member
of fred).

Return value is NULL if the member can not be found.

With ptr and b_type, you can use nk_serialize or nk_scan to serialize or
deserialize data to just the "b" member of your structure.

More elaborate expressions can be used to access any member of your
structure.  For example:

	a.b.c		Access a member in a sub-structure
	a.ary[3]	Access an array element
	a.ary[3].x	Access a member of an array of structures

## Database

Functions are provided to create a database located in flash memory.  This
just means that we save a serialized form of a structure into flash memory. 
The serialized form is useful because it tends to preserve data across
schema changes.

Two areas of memory (banks) are allocated so that an older version of the
database is always preserved in case of a power outage during a write.   The
data in each bank is marked with a revision number and a 32-bit CRC.

An nk_dbase structure should be filled out with information about the database:

~~~c
	const struct nk_dbase test_dbase =
	{
	    .ty = &tyTESTTOP,			// Schema for database
	    .bank0 = 65536+32768+0,		// Location of bank0 in flash memory
	    .bank1 = 65536+32768+8192,		// Location of bank1 in flash memory
	    .bank_size = 8192,			// Size of each flash memory bank
	    .bigbuf = big_buf,			// Transfer buffer
	    .bigbuf_size = sizeof(big_buf),	// Sizeof transfer buffer
	    .flash_read = nk_mcuflash_read,	// Flash read access function
	    .flash_erase = nk_mcuflash_erase,	// Flash erase function
	    .flash_write = nk_mcuflash_write,	// Flash write function
	    .flash_granularity = 8		// Write granularity- writes are padded so that they always a multiple of this size
						// 1 is allowed for granularity
	};
~~~

## nk_dbase_load()

~~~c
int nk_dbase_load(
	const struct nk_dbase *dbase,
	char *rev, // Address of version number
	void *ram // Address of database in RAM
);
~~~

Load a database into RAM.  The revision number of the loaded database is
saved in *rev.

## nk_dbase_save()

~~~c
int nk_dbase_save(
	const struct nk_dbase *dbase,
	char *rev, // Address of version number
	void *ram // Address of database in RAM
);
~~~

Save a database from RAM into flash.  This bumps the revision number if the
database was successfully saved.

## Configuration or calibration database

A template CLI command is provided which shows how a user interface for a
database might work, see (../app/database.c)[../app/database.c]

The command allows you to query or set elements of the database.  The
advantage of this method is that it avoids the need for writing a user
interface for each configuration variable.

A typical CLI command looks like this:

	config show			Print entire configuration database in serialized format
	config replace			Replace the entire database with following multi-line input
	config get a.b			Print a particular element
	config set a.b 7		Set a particular element
	config set a { b:7, c:6 }	Set an entire sub-structure
	config set a			Set an entire sub-structure with following multi-line input
	config flush			Save database to flash
	config load			Load database from flash
	config reset			Reset database to default values

