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
a C-language preprocessor, so preprodcessor directives and macros are
supported.  However only a subset of the C-language understood by the GNU C
Compiler (gcc) and clang is implemented, therefore it is not recommended to #include
standard library header files.  It will fail on things like
attributes and GNU-C extensions which are used GLIBC's header files.

The macro \_\_SCOMP\_\_ is automatically #defined by the compiler and can be
used by conditional preprocessor directives- for example for excluding
header files #includes.

The schema header file should describe the schema using the following
things:

* Primitive types including: char, int, short, long, float and double
  * The signed and unsigned modifiers are allowed.
* Fixed length arrays of arrays, structures or primitive types
* Structures

One structure must be provided as the the top-level structure of the schema. 
Its name is should be supplied to the compiler using the --top command line
option.

Strings are are defined like this:

	schar my_string[80];

'schar' is used instead of char to distinguish between strings and byte
arrays.  nkserialize.h provided a "typedef char schar" and should be
included before your use of the schema header file (but should not be
included when fed into the schema compiler).

Strings are NUL terminated variable length up to the size of the given schar
array.

Variable length arrays are allowed within structures as follows;

	union len my_array_len;
	int my_array[30];

The "union len" indicates a variable length array.  The maximum length of
the array is 30 in this case.  The actual number of elements in the array is
given by my_array_len.len, and can range from 0 to 30 in this case.  When
the array is serialized, only my_array_len.len elements will be included. 
When it is deserialized, my_array_len.len will hold the number of elements
loaded into my_array.

Tables are a special form of arrays of structures and my be included within
structures as follows:

	union table_len my_table_len;
	struct row my_table[30];

The "union table_len" indicates a variable length table.  The maximum number
of rows is 30 in this case.  The actual number of rows in the table is
given by my_table_len.len, and can range from 0 to 30 in this case.  When
the table is serialized, only my_table_len.len rows will be included. 
When it is deserialized, my_table_len.len will hold the number of rows
loaded into my_table.

Tables and variable arrays are serialized differently.  Tables are formatted
two-dimensionally such that they can be cut-and-paste into a spreadsheet. 
Instead of object (or structure) member names within each row, there is a
header row above the first data row with column names.

