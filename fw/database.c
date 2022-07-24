#include <stdbool.h>
#include <string.h>

#include "nkcli.h"
#include "nkprintf.h"
#include "nkscan.h"
#include "nkdbase.h"
#include "nkmcuflash.h"
#include "nkmcuflash_map.h"
#include "database.h"

// Calibration metadata

const struct type tyIPADDR = {
    .what = tARRAY,
    .size = sizeof(((struct cal *)0)->ip_addr),
    .members = NULL,
    .subtype = &tyUINT8,
    .check = NULL
};

const struct type tyMACADDR = {
    .what = tARRAY,
    .size = sizeof(((struct cal *)0)->mac_addr),
    .members = NULL,
    .subtype = &tyUINT8,
    .check = NULL
};

const struct member cal_members[] = {
    { "port", &tyUINT16, offsetof(struct cal, port), 0 },
    { "ip_addr", &tyIPADDR, offsetof(struct cal, ip_addr), 0 },
    { "mac_addr", &tyMACADDR, offsetof(struct cal, mac_addr), 0 },
    { NULL, NULL, 0, 0 }
};

const struct type tyCAL = {
    .what = tSTRUCT,
    .size = sizeof(struct cal),
    .members = cal_members,
    .subtype = NULL,
    .check = NULL
};

// Calibration database

struct cal cal;
char cal_rev;

const struct cal cal_defaults = {
    .port = 8888,
    .ip_addr = { 10, 0, 0, 2 },
    .mac_addr = { 0xfc, 0xff, 0xff, 0x00, 0x00, 0x11 }
};

unsigned char xfer_buf[128];

const struct nk_dbase cal_dbase =
{
    .ty = &tyCAL,

    .bank0 = {
        .area_size = FLASH_KEYVAL_SIZE,
        .area_base = FLASH_CAL_KEYVAL_ADDR_1,
        .erase_size = NK_MCUFLASH_ERASE_SIZE,
        .info = NULL,
        .flash_read = (int (*)(const void *, uint32_t, uint8_t *, uint32_t))nk_mcuflash_read, // info, address, data, byte_count
        .flash_erase = (int (*)(const void *info, uint32_t addr, uint32_t size))nk_mcuflash_erase,
        .flash_write = (int (*)(const void *, uint32_t, const uint8_t *, uint32_t))nk_mcuflash_write, // info, address, data, byte_count
        .granularity = 1
    },

    .bank1 = {
        .area_size = FLASH_KEYVAL_SIZE,
        .area_base = FLASH_CAL_KEYVAL_ADDR_2,
        .erase_size = NK_MCUFLASH_ERASE_SIZE,
        .info = NULL,
        .flash_read = (int (*)(const void *, uint32_t, uint8_t *, uint32_t))nk_mcuflash_read, // info, address, data, byte_count
        .flash_erase = (int (*)(const void *info, uint32_t addr, uint32_t size))nk_mcuflash_erase,
        .flash_write = (int (*)(const void *, uint32_t, const uint8_t *, uint32_t))nk_mcuflash_write, // info, address, data, byte_count
        .granularity = 1
    },

    .buf = xfer_buf,
    .buf_size = sizeof(xfer_buf),
    .flash_granularity = 1
};


// Initialize calibration database

void database_init()
{
    int rtn;

    nk_startup_message("Database\n");

    cal = cal_defaults;

    rtn = nk_dbase_load(&cal_dbase, &cal_rev, &cal);
    if (rtn)
    {
        nk_printf("cal load failed.\n");
    }
}

static int cmd_dbase(nkinfile_t *args)
{
    int rtn;
    char key[80];
    if (nk_fscan(args, "clear ")) {
        nk_printf("Clearing calibration store\n");
        cal = cal_defaults;
        rtn = nk_dbase_save(&cal_dbase, &cal_rev, &cal);
        if (rtn)
            nk_printf("Save failure\n");
        rtn = nk_dbase_save(&cal_dbase, &cal_rev, &cal);
        if (rtn)
            nk_printf("Save failure\n");
        nk_printf("done.\n");
        nk_printf("Please power cycle!\n");
    } else if (nk_fscan(args, "save ")) {
        rtn = nk_dbase_save(&cal_dbase, &cal_rev, &cal);
        if (rtn)
            nk_printf("Save failure\n");
    } else if (nk_fscan(args, "load ")) {
        rtn = nk_dbase_load(&cal_dbase, &cal_rev, &cal);
        if (rtn)
            nk_printf("Load failure\n");
    } else if (nk_fscan(args, "show ")) {
        nk_printf("BEGIN\n");
        nk_dbase_fprint(nkstdout, &tyCAL, &cal, 0, "\n");
        nk_printf("END\n");
#if 0
    } else if (nk_fscan(args, "replace ")) {
        char *l = multi_gets();
        cal = cal_defaults;
        if (nk_fscan(&l, " %v ", &tyCAL, &cal)) {
            nk_printf("Replacing keyval store with ");
            print_val(&tyCAL, &cal, 0, "\n");
        } else {
            cal = cal_defaults;
            nk_printf("Syntax error\n");
        }
#endif
    } else if (nk_fscan(args, "get %w ", key, sizeof(key))) {
        void *loc = &cal;
        uint32_t trigs = 0;
        const struct type *t = nk_xpath(key, &tyCAL, &loc, &trigs);
        if (!t) {
            nk_printf("Not found.\n");
        } else {
            nk_printf("BEGIN\n");
            nk_dbase_fprint(nkstdout, t, loc, 0, "\n");
            nk_printf("END\n");
        }
    } else if (nk_fscan(args, "set %w %e", key, sizeof(key))) {
        void *loc = &cal;
        uint32_t trigs = 0;
        const struct type *t = nk_xpath(key, &tyCAL, &loc, &trigs);
        if (!t) {
            nk_printf("Not found.\n");
        } else {
            if (nk_fscan(args, "%v ", t, (void *)loc)) {
                nk_printf("Setting %s to ", key);
                nk_dbase_fprint(nkstdout, t, loc, 0, "\n");
#if 0
            } else if (nk_fscan(args, " ")) {
                char *l = multi_gets();
                if (nk_fscan(&l, " %v ", t, (void *)loc)) {
                    nk_printf("Setting %s to ", key);
                    print_val(t, loc, 0, "\n");
                } else {
                    nk_printf("Syntax error\n");
                }
#endif
            } else {
                nk_printf("Syntax error\n");
            }
        }
    } else {
        nk_printf("Syntax error\n");
    }
    return 0;
}

COMMAND(cmd_dbase,
    ">dbase                     Database store\n"
    "-dbase clear               Clear database store\n"
    "-dbase show                Show entire database\n"
    "-dbase load                Load database from flash\n"
    "-dbase save                Save database to flash\n"
    "-dbase replace             Replace database: prompts for multi-line value\n"
    "-dbase get <key>           Get value assigned to key\n"
    "-dbase set <key> <value>\n"
    "-                          Assign value to a key\n"
    "-dbase set <key>           Assign value to a key: Prompts for multi-line value\n"
)
