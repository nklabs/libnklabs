# Schema Compiler

This is the schema compiler for libnklabs nkserialize.  It generates the
meta-data C-language source file needed for a given schema provided as a
C-language header file.

## Build instructions

type "make"

## Usage instructions

This builds the schema.c for
[../../tests/nkdbase/schema.h](../../tests/nkdbase/schema.h), a nkdbase unit
test schema.

	./scomp schema.h --top testtop >schema.c

## Options

	scomp filename [options]

	--top <name>	Select structure name of top-level object

	-I <path>	Add <path> to list of searched include file directories

	--help		Print help

	--lex		Output tokenized version of source file for debugging

	--dump		Output struct, union and typedef tables for debugging

	--tree		Output abstract syntax tree for debugging

## Source format

The schema compiler expects a C-language header file.  The compiler includes
a C-language preprocessor, so preprocessor directives and macros are
supported.  However only a subset of the C-language understood by the GNU C
Compiler (gcc) and clang is implemented, therefore it is not recommended to #include
standard library header files.  It will fail on things like
attributes and GNU-C extensions which are used in GLIBC's header files.

The macro \_\_SCOMP\_\_ is automatically #defined by the compiler and can be
used by conditional preprocessor directives- for example for excluding
header files #includes.

The schema header file should describe the schema using the following
things:

* Primitive types including: char, int, short, long, float and double
  * The signed and unsigned modifiers are allowed.
* Fixed length arrays of arrays, structures or primitive types
* Structures
* Unions

One structure must be provided as the the top-level structure of the schema. 
Its name is should be supplied to the compiler using the --top command line
option.

Strings are are defined like this:

	schar my_string[80];

'schar' is used instead of char to distinguish between strings and byte
arrays.  nkserialize.h provides a "typedef char schar" and should be
included before your use of the schema header file (but should not be
included when the schema header file is fed into the schema compiler).

Strings are NUL terminated variable length up to the size of the given schar
array.

Unions are allowed within structures as follows:

	struct example {
		int value_tag;

		union value {
			int integer;
			float real;
		} value;
	};

The discriminator for a union member must be named `<union_member_name>_tag`
and must be declared as an `int` in the same containing structure.

The integer tag indicates which member of the union is active. Union 
members are assigned tag values starting at 1 in the order that they are
declared. In this case value_tag = 1 selects value.integer and value_tag = 2
selects value.real. Only the selected member is serialized or deserialized.

A tag value of 0 represents a null value.  When the tag is 0, the union
member is serialized as `null` and no union member is selected.

Non-union members may also be nullable by providing a corresponding tag
member in the same containing structure:

	struct example {
		int value_tag;
		int value;
	};

The discriminator for a nullable member must be named `<member_name>_tag`
and must be declared as an `int` in the same containing structure.

A tag value of 0 indicates that the member is null.  A tag value of 1
indicates that the member contains a value.

In this case `value_tag = 0` causes `value` to be serialized as `null`,
while `value_tag = 1` causes the value of `value` to be serialized or
deserialized normally.

Variable length arrays are allowed within structures as follows;

	int my_array_len;
	int my_array[30];

The integer field my_array_len indicates that my_array is a variable array
length. The length must have the same name as the array with _len appended,
but does not need to immediately precede the array in the structure.

The maximum length of the array is 30 in this case.  The actual number of 
elements in the array is given by my_array_len.len, and can range from 0 
to 30 in this case.  When the array is serialized, only my_array_len 
elements will be included.  When it is deserialized, my_array_len will hold 
the number of elements loaded into my_array.

Tables are a special form of arrays of structures and my be included within
structures as follows:

	int my_table_table_len;
	struct row my_table[30];

The integer field my_table_table_len indicates that my_table is a variable 
length table. The length field must have the same name as the table with 
_table_len appended, but does not need to immediately precede the table in
the structure.

The maximum number of rows is 30 in this case.  The actual number of rows in 
the table is given by my_table_len.len, and can range from 0 to 30 in this 
case. When the table is serialized, only my_table_len.len rows will be 
included. When it is deserialized, my_table_len.len will hold the number of 
rows loaded into my_table.

Tables and variable arrays are serialized differently.  Tables are formatted
two-dimensionally such that they can be cut-and-paste into a spreadsheet. 
Instead of object (or structure) member names within each row, there is a
header row above the first data row with column names.
