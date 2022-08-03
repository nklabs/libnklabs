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

#include <string.h>
#include "nkcrclib.h"
#include "nkdirect.h"

// Open file for reading
int nk_direct_read_open(nk_direct_t *var_file, const nk_direct_base_t *file, unsigned char *buffer, size_t buf_size)
{
    int rtn = 0;
    (void)buffer;
    (void)buf_size;
    var_file->file = file;
    var_file->crc = 0;
    var_file->size = file->area_size;
    return rtn;
}

// For nkinfile_t: read a block from the file
size_t nk_direct_read(nk_direct_t *var_file, uint32_t offset, unsigned char *buffer, size_t buf_size)
{
    int rtn;
    size_t len;
    const nk_direct_base_t *file = var_file->file;
    len = buf_size;
    if (offset + len > var_file->size)
        len = (size_t)(var_file->size - offset);
    if (len) {
        rtn = file->flash_read(file->info, file->area_base + offset, buffer, len);
        if (rtn)
            len = 0;
    }
    return len;
}

// Open file for writing
// This erases first block of area
int nk_direct_write_open(nk_direct_t *var_file, const nk_direct_base_t *file)
{
    int rtn = 0;
    var_file->file = file;
    var_file->crc = 0;
    var_file->size = 0;
    return rtn;
}

// For nkoutfile_t: write a block to the file
int nk_direct_write(nk_direct_t *var_file, const unsigned char *buffer, size_t len)
{
    int rtn = 0;
    const nk_direct_base_t *file = var_file->file;
    size_t x;

    // Update CRC
    for (x = 0; x != len; ++x)
        var_file->crc = nk_crc32be_update(var_file->crc, buffer[x]);

    // Starting address
    uint32_t address = file->area_base + var_file->size;

    // Write write pointer
    var_file->size += len;

    // Write a page at a time
    while (len) {
        uint32_t page_offset = (address & (file->erase_size - 1)); // Starting offset within page
	size_t page_len = (size_t)(file->erase_size - page_offset); // Up to one page
	if (len < page_len)
	    page_len = len;
        if (file->flash_erase && ((address & (file->erase_size - 1)) == 0)) {
            // First page of a block... erase the block
            rtn = file->flash_erase(file->info, address, file->erase_size);
            if (rtn)
                return rtn;
        }
	rtn = file->flash_write(file->info, address, buffer, page_len);
	if (rtn)
	    return rtn;
	len -= page_len;
	address += page_len;
	buffer += page_len;
    }

    return rtn;
}

// Close write file
int nk_direct_write_close(nk_direct_t *var_file)
{
    (void)var_file;
    return 0;
}
