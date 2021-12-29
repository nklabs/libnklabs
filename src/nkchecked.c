#include "nkcrclib.h"
#include "nkchecked.h"

// Open file for reading
int nk_checked_read_open(nk_checked_t *var_file, const nk_checked_base_t *file, unsigned char *buffer, size_t buf_size)
{
    int rtn;
    uint32_t crc;
    uint32_t size;
    uint32_t addr;
    var_file->file = file;
    // Get header
    rtn = file->flash_read(file->info, file->area_base, buffer, sizeof(nk_checked_header_t));
    if (rtn)
        return rtn;
    size = var_file->size = ((nk_checked_header_t *)buffer)->size;
    var_file->crc = ((nk_checked_header_t *)buffer)->crc;
    // Check file integrity
    // Is size good?
    if (var_file->size > file->area_size - sizeof(nk_checked_header_t))
        return -1;

    // How about CRC?
    addr = sizeof(nk_checked_header_t) + file->area_base;
    crc = 0;
    while (size) {
        uint32_t x;
        uint32_t len = size;
        if (len > buf_size)
            len = buf_size;
        rtn = file->flash_read(file->info, addr, buffer, len);
        if (rtn)
            return rtn;
        for (x = 0; x != len; ++x)
            crc = nk_crc32be_update(crc, buffer[x]);
        size -= len;
        addr += len;
    }

    if (crc != var_file->crc)
        return -1;
    // It's good
    return 0;
}

// For nkinfile_t: read a block from the file
size_t nk_checked_read(void *ptr, unsigned char *buffer, size_t offset)
{
    int rtn;
    uint32_t len;
    nk_checked_t *var_file = (nk_checked_t *)ptr;
    const nk_checked_base_t *file = var_file->file;
    len = file->buf_size;
    if (offset + len > var_file->size)
        len = var_file->size - offset;
    if (len) {
        rtn = file->flash_read(file->info, file->area_base + sizeof(nk_checked_header_t) + offset, buffer, len);
        if (rtn)
            len = 0;
    }
    return len;
}

// Open file for writing
// This erases first block of area
int nk_checked_write_open(nk_checked_t *var_file, const nk_checked_base_t *file)
{
    int rtn = 0;
    var_file->file = file;
    var_file->crc = 0;
    var_file->size = 0;
    if (file->flash_erase)
        rtn = file->flash_erase(file->info, file->area_base, file->erase_size);
    return rtn;
}

// For nkoutfile_t: write a block to the file
int nk_checked_write(void *ptr, unsigned char *buffer, size_t len)
{
    int rtn = 0;
    nk_checked_t *var_file = (nk_checked_t *)ptr;
    const nk_checked_base_t *file = var_file->file;
    uint32_t x;

    // Update CRC
    for (x = 0; x != len; ++x)
        var_file->crc = nk_crc32be_update(var_file->crc, buffer[x]);

    // Starting address
    uint32_t address = file->area_base + sizeof(nk_checked_header_t) + var_file->size;

    // Write write pointer
    var_file->size += len;

    // Write a page at a time
    while (len) {
        uint32_t page_offset = (address & (file->erase_size - 1)); // Starting offset within page
	uint32_t page_len = file->erase_size - page_offset; // Up to one page
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

// Close write file: write header
int nk_checked_write_close(nk_checked_t *var_file)
{
    nk_checked_header_t header;
    const nk_checked_base_t *file = var_file->file;
    header.crc = var_file->crc;
    header.size = var_file->size;
    return file->flash_write(file->info, file->area_base, (unsigned char *)&header, sizeof(header));
}
