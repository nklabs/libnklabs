# Database based on serialization library


Functions are provided to create a database located in flash memory.  This
just means that we save a serialized form of a structure into flash memory. 
The serialized form is useful because it tends to preserve data across
schema changes.

Two areas of memory (banks) are allocated so that an older version of the
database is always preserved in case of a power outage during a write.   The
data in each bank is marked with a revision number and a 32-bit CRC.

## Files

[nkdbase.h](../inc/nkdbase.h), [nkdbase.c](../src/nkdbase.c)

## Usage

An nk_dbase structure should be filled out with information about the database:

~~~c
	const struct nk_dbase test_dbase =
	{
	    .ty = &tyTESTTOP,			// Schema for database
	    .bank0 = 65536+32768+0,		// Location of bank0 in flash memory
	    .bank1 = 65536+32768+8192,		// Location of bank1 in flash memory
	    .bank_size = 8192,			// Size of each flash memory bank (this size is used for flash_erase)
	    .buf = xfer_buf,			// Transfer buffer: used for flash_read and flash_write
	    .buf_size = sizeof(xfer_buf),	// Sizeof transfer buffer
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

Returns zero for success.

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

Returns zero for success.

## Configuration or calibration database

A template CLI command along with example database schema is provided which
shows how a user interface for a database might work, see
[../app/database.c](../app/database.c)

The dbase command allows you to query or set elements of the database.  The
advantage of this method is that it avoids the need for writing a user
interface for each configuration variable.

A typical CLI command looks like this:

	dbase show			Print entire database in serialized format
	dbase replace			Replace the entire database with following multi-line input
	dbase get a.b			Print a particular element
	dbase set a.b 7		        Set a particular element
	dbase set a { b:7, c:6 }	Set an entire sub-structure
	dbase set a			Set an entire sub-structure with following multi-line input
	dbase save			Save database to flash
	dbase load			Load database from flash
	dbase reset			Reset database to default values
