// Copyright 2020 NK Labs, LLC

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

#include "lib.h"

// Schema checked serializer / deserializer

// Standard primitive types

const struct type tyINT = {
	.what = tINT,
	.size = sizeof(int),
	.members = NULL,
	.subtype = NULL
};

const struct type tyBOOL = {
	.what = tBOOL,
	.size = sizeof(int),
	.members = NULL,
	.subtype = NULL
};

const struct type tyDOUBLE = {
	.what = tDOUBLE,
	.size = sizeof(double),
	.members = NULL,
	.subtype = NULL
};

const struct type tyFLOAT = {
	.what = tFLOAT,
	.size = sizeof(float),
	.members = NULL,
	.subtype = NULL
};

void indent(int x)
{
	while (x--)
		printf(" ");
}

void print_val(const struct type *type, void *location, int ind, char *ed)
{
    char sbuf[32];
    switch (type->what) {
        case tSTRUCT: {
            const struct member *m;
            const char *o_name = 0;
            const struct type *o_type = 0;
            ptrdiff_t o_offset = 0;

            printf("{\n");

            for (m = type->members; m->name; ++m) {
                if (o_name) {
                    if (o_type->what == tTABLE) {
                        indent(ind + 2); printf("%s:\n", o_name);
                        indent(ind + 4); print_val(o_type, (char *)location + o_offset, ind + 4, ",\n");
                    } else {
                        indent(ind + 2); printf("%s: ", o_name);
                        print_val(o_type, (char *)location + o_offset, ind + 2, ",\n");
                    }
                }
                o_name = m->name;
                o_type = m->type;
                o_offset = m->offset;
            }

            if (o_name) {
                if (o_type->what == tTABLE) {
                    indent(ind + 2); printf("%s:\n", o_name);
                    indent(ind + 4); print_val(o_type, (char *)location + o_offset, ind + 4, "\n");
                } else {
                    indent(ind + 2); printf("%s: ", o_name);
                    print_val(o_type, (char *)location + o_offset, ind + 2, "\n");
                }
            }

            indent(ind); printf("}%s", ed);
            break;
        } case tTABLE: {
            int y;
            const struct type *subtype = type->subtype;
            const struct member *m;
            int nrows = ((union len *)location)->len;
            location = (char *)location + sizeof(union len);

            printf("(");

            for (m = subtype->members; m->name; ++m) {
                printf("\t%s", m->name);
            }

            printf("\n");

            for (y = 0; y != nrows; ++y) {
                indent(ind); printf(":");
                for (m = subtype->members; m->name; ++m) {
                    printf("\t");
                    print_val(m->type, (char *)location + m->offset, 0, "");
                }
                printf("\n");
                location = (char *)location + subtype->size;
            }

            indent(ind); printf(")%s", ed);
            break;
        } case tARRAY: {
            int x;
            int len = ((union len *)location)->len;
            location = (char *)location + sizeof(union len);
            printf("[\n");
            for (x = 0; x != len; ++x) {
                indent(ind + 2);
                if (x + 1 == len)
                    print_val(type->subtype, location, ind + 2, "\n");
                else
                    print_val(type->subtype, location, ind + 2, ",\n");
                location = (char *)location + type->subtype->size;
            }
            indent(ind); printf("]%s", ed);
            break;
/*        } case tVOID: {
            printf("null%s", ed);
            break; */
        } case tDOUBLE: {
            int x;
            // Print double as decimal with enough precision to
            // recover the original double on read in.
            // (should be %.17g, but uses too much space)
            sprintf(sbuf, "%g", *(double *)location);
            // %g will omit the decimal point if it can.  But we
            // always want it.. so add it back.
            // FIXME: handle nan and inf
            for (x = 0; sbuf[x]; ++x)
                if (sbuf[x] == '.' || sbuf[x] == 'e')
                    break;
            if (!sbuf[x]) {
                sbuf[x++] = '.';
                sbuf[x++] = '0';
                sbuf[x] = 0;
            }
            printf("%s%s", sbuf, ed);
            break;
        } case tFLOAT: {
            int x;
            // Print double as decimal with enough precision to
            // recover the original double on read in.
            // (should be %.17g, but uses too much space)
            sprintf(sbuf, "%g", *(float *)location);
            // %g will omit the decimal point if it can.  But we
            // always want it.. so add it back.
            // FIXME: handle nan and inf
            for (x = 0; sbuf[x]; ++x)
                if (sbuf[x] == '.' || sbuf[x] == 'e')
                    break;
            if (!sbuf[x]) {
                sbuf[x++] = '.';
                sbuf[x++] = '0';
                sbuf[x] = 0;
            }
            printf("%s%s", sbuf, ed);
            break;
        } case tINT: {
            printf("%d%s", *(int *)location, ed);
            break;
        } case tBOOL: {
            if (*(int *)location)
                printf("true%s", ed);
            else
                printf("false%s", ed);
            break;
        } case tSTRING: {
            char *s = (char *)location;
            int x;
            printf("\"");
            for (x = 0; s[x]; ++x) {
                if (s[x] == '\"')
                    printf("\\\"");
                else if (s[x] == '\\')
                    printf("\\\\");
                else if (s[x] == '\n')
                    printf("\\n");
                else if (s[x] == '\r')
                    printf("\\r");
                else if (s[x] < 32 || s[x] > 126)
                    printf("\\x%2.2x", (0xFF & s[x]));
                else
                    printf("%c", s[x]);
            }
            printf("\"%s", ed);
            break;
        } default: {
            printf("unknown%s", ed);
            break;
        }
    }
}

// Serialize a database with a given schema

int serialize_val(char *org, const struct type *type, void *location)
{
	char *buf = org;
	switch (type->what) {
		case tSTRUCT: {
			const struct member *m;
			*buf++ = '{';
			m = type->members;
			while (m->name) {
				sprintf(buf, "%s:", m->name); buf += strlen(buf);
				buf += serialize_val(buf, m->type, (char *)location + m->offset);
				++m;
				if (m->name) {
					sprintf(buf, ", "); buf += strlen(buf);
				}
			}
			*buf++ = '}';
			break;
		} case tARRAY: {
			*buf++ = '[';
			int count = ((union len *)location)->len;
			location = (char *)location + sizeof(union len);
			while (count--) {
				buf += serialize_val(buf, type->subtype, location);
				location = (char *)location + type->subtype->size;
				if (count) {
					sprintf(buf, ", "); buf += strlen(buf);
				}
			}
			*buf++ = ']';
			break;
		} case tTABLE: {
			const struct member *m;
			*buf++ = '(';
			int count = ((union len *)location)->len;
			location = (char *)location + sizeof(union len);
			m = type->subtype->members;
			while (m->name) {
				sprintf(buf, "%s", m->name); buf += strlen(buf);
				++m;
				*buf++ = ' ';
			}
			while (count--) {
				*buf++ = ':';
				m = type->subtype->members;
				while (m->name) {
					buf += serialize_val(buf, m->type, (char *)location + m->offset);
					++m;
					if (m->name)
						*buf++ = ' ';
				}
				location = (char *)location + type->subtype->size;
			}
			*buf++ = ')';
			break;
		} case tBOOL: {
			if (*(int *)location)
				sprintf(buf, "true");
			else
				sprintf(buf, "false");
			buf += strlen(buf);
			break;
		} case tINT: {
			sprintf(buf, "%d", *(int *)location);
			buf += strlen(buf);
			break;
		} case tDOUBLE: {
			sprintf(buf, "%g", *(double *)location);
			buf += strlen(buf);
			break;
		} case tFLOAT: {
			sprintf(buf, "%g", *(float *)location);
			buf += strlen(buf);
			break;
		} case tSTRING: {
			char *s = (char *)location;
			*buf++ = '"';
			while (*s) {
				if (*s == '"') {
					*buf++ = '\\';
					*buf++ = '"';
				} else if (*s == '\\') {
					*buf++ = '\\';
					*buf++ = '\\';
				} else if (*s == '\n') {
					*buf++ = '\\';
					*buf++ = 'n';
				} else if (*s == '\r') {
					*buf++ = '\\';
					*buf++ = 'r';
				} else if (*s < 32 || *s > 126) {
					*buf++ = '\\';
					*buf++ = 'x';
					sprintf(buf, "%2.2x", *(unsigned char *)s);
					buf += strlen(buf);
				} else {
					*buf++ = *s;
				}
				++s;
			}
			*buf++ = '"';
		}
	}
	return buf - org;
}

// Xpath traversal
// It would be nice if this worked for tables...

const struct type *xpath(char *key, const struct type *type, void **location_loc)
{
    char buf[80];
    char *p = key;
    void *location = *location_loc;
    int idx;

    while (*p) {
        if (nk_scan(&p, "[ %d ]", &idx)) {
            int count;
            if (type->what == tARRAY || type->what == tTABLE) {
                count = ((union len *)location)->len;
                location = (char *)location + sizeof(union len);
                if (idx >= count || idx < 0) {
                    printf("Array index out of bounds in key %s\n", key);
                    return 0;
                }
                location = (char *)location + idx * type->subtype->size;
                type = type->subtype;
            } else {
                printf("Attempt to index a non-array in key %s\n", key);
                return 0;
            }
            if (*p == '.')
                ++p;
            else if (*p == '[')
                ;
            else
                break;
        } else if (nk_scan(&p, "%i", buf, sizeof(buf))) {
            const struct member *m;
            if (type->what != tSTRUCT) {
                printf("Attempt to index a non-struct in key %s\n", key);
                return 0;
            }
            for (m = type->members; m->name; ++m)
                if (!strcmp(m->name, buf))
                    break;
            if (!m->name) {
                printf("Item does not exist %s\n", buf);
                return 0;
            }
            type = m->type;
            location = (char *)location + m->offset;
            if (*p == '.')
                ++p;
            else if (*p == '[')
                ;
            else
                break;
        } else {
            printf("Syntax error in key %s\n", key);
            return 0;
        }
    }
    if (*p) {
        printf("Syntax error in key %s\n", key);
        return 0;
    }
    *location_loc = location;
    return type;
}

// Database management

char keyval_cal_rev;
char keyval_config_rev;
char keyval_model_rev;

char bigbuf[65536];

void keyval_cal_clear()
{
    cal = cal_defaults;
}

void keyval_config_clear()
{
    config = config_defaults;
}

void keyval_model_clear()
{
    model = model_defaults;
}

int keyval_cal_flush()
{
    int size = serialize_val(bigbuf, &tyCAL, &cal);
    unsigned int cr;
    bigbuf[size++] = 0;
    bigbuf[size++] = ++keyval_cal_rev;
    printf("saving: %s\n", bigbuf);
    printf("size = %d\n", size);
    printf("rev = %d\n", keyval_cal_rev);
    cr = crc_check((unsigned char *)bigbuf, size);
    bigbuf[size++] = (cr >> 24);
    bigbuf[size++] = (cr >> 16);
    bigbuf[size++] = (cr >> 8);
    bigbuf[size++] = (cr);
    if (keyval_cal_rev & 1) { // It goes in bank 2
        printf("Erasing...\n");
        flash_erase(FLASH_CAL_KEYVAL_ADDR_2);
        printf("Writing...\n");
        flash_write(FLASH_CAL_KEYVAL_ADDR_2, (unsigned char *)bigbuf, size);
        printf("done.\n");
    } else { // It goes in bank 1
        printf("Erasing...\n");
        flash_erase(FLASH_CAL_KEYVAL_ADDR_1);
        printf("Writing...\n");
        flash_write(FLASH_CAL_KEYVAL_ADDR_1, (unsigned char *)bigbuf, size);
        printf("done.\n");
    }
    return 0;
}

int keyval_config_flush()
{
    int size = serialize_val(bigbuf, &tyCONFIG, &config);
    unsigned int cr;
    bigbuf[size++] = 0;
    bigbuf[size++] = ++keyval_config_rev;
    printf("saving: %s\n", bigbuf);
    printf("size = %d\n", size);
    printf("rev = %d\n", keyval_config_rev);
    cr = crc_check((unsigned char *)bigbuf, size);
    bigbuf[size++] = (cr >> 24);
    bigbuf[size++] = (cr >> 16);
    bigbuf[size++] = (cr >> 8);
    bigbuf[size++] = (cr);
    if (keyval_config_rev & 1) { // It goes in bank 2
        printf("Erasing...\n");
        flash_erase(FLASH_CONFIG_KEYVAL_ADDR_2);
        printf("Writing...\n");
        flash_write(FLASH_CONFIG_KEYVAL_ADDR_2, (unsigned char *)bigbuf, size);
        printf("done.\n");
    } else { // It goes in bank 1
        printf("Erasing...\n");
        flash_erase(FLASH_CONFIG_KEYVAL_ADDR_1);
        printf("Writing...\n");
        flash_write(FLASH_CONFIG_KEYVAL_ADDR_1, (unsigned char *)bigbuf, size);
        printf("done.\n");
    }
    return 0;
}

int keyval_model_flush()
{
    int size = serialize_val(bigbuf, &tyMODEL, &model);
    unsigned int cr;
    bigbuf[size++] = 0;
    bigbuf[size++] = ++keyval_model_rev;
    printf("saving: %s\n", bigbuf);
    printf("size = %d\n", size);
    printf("rev = %d\n", keyval_model_rev);
    cr = crc_check((unsigned char *)bigbuf, size);
    bigbuf[size++] = (cr >> 24);
    bigbuf[size++] = (cr >> 16);
    bigbuf[size++] = (cr >> 8);
    bigbuf[size++] = (cr);
    if (keyval_config_rev & 1) { // It goes in bank 2
        printf("Erasing...\n");
        flash_erase(FLASH_MODEL_KEYVAL_ADDR_2);
        printf("Writing...\n");
        flash_write(FLASH_MODEL_KEYVAL_ADDR_2, (unsigned char *)bigbuf, size);
        printf("done.\n");
    } else { // It goes in bank 1
        printf("Erasing...\n");
        flash_erase(FLASH_MODEL_KEYVAL_ADDR_1);
        printf("Writing...\n");
        flash_write(FLASH_MODEL_KEYVAL_ADDR_1, (unsigned char *)bigbuf, size);
        printf("done.\n");
    }
    return 0;
}

int keyval_cal_get(int n)
{
    int x;
    if (n)
        flash_read(FLASH_CAL_KEYVAL_ADDR_2, (unsigned char *)bigbuf, sizeof(bigbuf));
    else
        flash_read(FLASH_CAL_KEYVAL_ADDR_1, (unsigned char *)bigbuf, sizeof(bigbuf));
    for (x = 0; x != sizeof(bigbuf) && bigbuf[x]; ++x);
    if (x == sizeof(bigbuf)) {
        printf("size is bad\n");
        return -1;
    }
    ++x;
    keyval_cal_rev = bigbuf[x++];
    printf("size = %d\n", x);
    printf("rev = %d\n", keyval_cal_rev);
    x += 4;
    if (crc_check((unsigned char *)bigbuf, x)) {
        printf("crc is bad\n");
        return -1;
    }
    // printf("data = %s\n", bigbuf);
    return 0;
}

int keyval_config_get(int n)
{
    int x;
    if (n)
        flash_read(FLASH_CONFIG_KEYVAL_ADDR_2, (unsigned char *)bigbuf, sizeof(bigbuf));
    else
        flash_read(FLASH_CONFIG_KEYVAL_ADDR_1, (unsigned char *)bigbuf, sizeof(bigbuf));
    for (x = 0; x != sizeof(bigbuf) && bigbuf[x]; ++x);
    if (x == sizeof(bigbuf)) {
        printf("size is bad\n");
        return -1;
    }
    ++x;
    keyval_config_rev = bigbuf[x++];
    printf("size = %d\n", x);
    printf("rev = %d\n", keyval_config_rev);
    x += 4;
    if (crc_check((unsigned char *)bigbuf, x)) {
        printf("crc is bad\n");
        return -1;
    }
    // printf("data = %s\n", bigbuf);
    return 0;
}

int keyval_model_get(int n)
{
    int x;
    if (n)
        flash_read(FLASH_MODEL_KEYVAL_ADDR_2, (unsigned char *)bigbuf, sizeof(bigbuf));
    else
        flash_read(FLASH_MODEL_KEYVAL_ADDR_1, (unsigned char *)bigbuf, sizeof(bigbuf));
    for (x = 0; x != sizeof(bigbuf) && bigbuf[x]; ++x);
    if (x == sizeof(bigbuf)) {
        printf("size is bad\n");
        return -1;
    }
    ++x;
    keyval_model_rev = bigbuf[x++];
    printf("size = %d\n", x);
    printf("rev = %d\n", keyval_model_rev);
    x += 4;
    if (crc_check((unsigned char *)bigbuf, x)) {
        printf("crc is bad\n");
        return -1;
    }
    // printf("data = %s\n", bigbuf);
    return 0;
}

int keyval_cal_load()
{
    int first_good = !keyval_cal_get(0);
    int first_rev = keyval_cal_rev;
    int second_good = !keyval_cal_get(1);
    int second_rev = keyval_cal_rev;
    cal = cal_defaults;
    if (second_good && first_good) {
        if (first_rev - second_rev >= 0)
            goto get_first;
        else
            goto do_parse;
    } else if (second_good) {
        goto do_parse;
    } else if (first_good) {
        char *p;
        get_first:
        keyval_cal_get(0);
        do_parse:
        p = bigbuf;
        if (nk_scan(&p, " %v", &tyCAL, &cal)) {
            printf("Calibration store loaded OK\n");
            return 0;
        } else {
            printf("CRC good, but calibration store failed to parse on load?\n");
            cal = cal_defaults;
            return -1;
        }
    } else {
        printf("No good CRCs\n");
        return -1;
    }
}

int keyval_config_load()
{
    int first_good = !keyval_config_get(0);
    int first_rev = keyval_config_rev;
    int second_good = !keyval_config_get(1);
    int second_rev = keyval_config_rev;
    config = config_defaults;
    if (second_good && first_good) {
        if (first_rev - second_rev >= 0)
            goto get_first;
        else
            goto do_parse;
    } else if (second_good) {
        goto do_parse;
    } else if (first_good) {
        char *p;
        get_first:
        keyval_config_get(0);
        do_parse:
        p = bigbuf;
        if (nk_scan(&p, " %v", &tyCONFIG, &config)) {
            printf("Configuration store loaded OK\n");
            return 0;
        } else {
            config = config_defaults;
            printf("CRC good, but configuration store failed to parse on load?\n");
            return -1;
        }
    } else {
        printf("No good CRCs\n");
        return -1;
    }
}

int keyval_model_load()
{
    int first_good = !keyval_model_get(0);
    int first_rev = keyval_model_rev;
    int second_good = !keyval_model_get(1);
    int second_rev = keyval_model_rev;
    model = model_defaults;
    if (second_good && first_good) {
        if (first_rev - second_rev >= 0)
            goto get_first;
        else
            goto do_parse;
    } else if (second_good) {
        goto do_parse;
    } else if (first_good) {
        char *p;
        get_first:
        keyval_model_get(0);
        do_parse:
        p = bigbuf;
        if (nk_scan(&p, " %v", &tyMODEL, &model)) {
            printf("Model store loaded OK\n");
            return 0;
        } else {
            model = model_defaults;
            printf("CRC good, but model store failed to parse on load?\n");
            return -1;
        }
    } else {
        printf("No good CRCs\n");
        return -1;
    }
}

void keyval_init()
{
    startup("Key/Value store");
    show_heap();
    if (keyval_cal_load()) {
        printf("Creating empty calibration store\n");
    }
    show_heap();
    if (keyval_config_load()) {
        printf("Creating empty configuration store\n");
    }
    show_heap();
    if (keyval_model_load()) {
        printf("Creating empty model store\n");
    }
    show_heap();
}

// Read in a long string

char *multi_gets()
{
    bigbuf[0] = 0;
    int idx = 0;
    int state = 0;
    printf("Type BEGIN to start the input\n");
    for (;;) {
        char *s = prompt("-", 1);
        if (state == 1) {
            if (!stricmp(s, "END"))
                break;
            else {
                strcpy(bigbuf + idx, s);
                idx += strlen(s);
                bigbuf[idx++] = ' ';
                bigbuf[idx] = 0;
            }
        } else {
            if (!stricmp(s, "BEGIN")) {
                printf("Input started.  Type END to end the input\n");
                state = 1;
            }
            else if (!stricmp(s, "END"))
                break;
        }
    }
    return bigbuf;
}

int cmd_cal(char *args)
{
    char key[80];
    if (facmode && nk_scan(&args, "clear %e")) {
        printf("Clearing calibration store\n");
        keyval_cal_clear();
        keyval_cal_flush();
        keyval_cal_flush();
        printf("done.\n");
        cmd_reboot("");
    } else if (facmode && nk_scan(&args, "flush %e")) {
        keyval_cal_flush();
    } else if (facmode && nk_scan(&args, "load %e")) {
        keyval_cal_load();
    } else if (facmode && nk_scan(&args, "show %e")) {
        printf("BEGIN\n");
        print_val(&tyCAL, &cal, 0, "\n");
        printf("END\n");
    } else if (facmode && nk_scan(&args, "replace %e")) {
        char *l = multi_gets();
        cal = cal_defaults;
        if (nk_scan(&l, " %v %e", &tyCAL, &cal)) {
            printf("Replacing keyval store with ");
            print_val(&tyCAL, &cal, 0, "\n");
        } else {
            cal = cal_defaults;
            printf("Syntax error\n");
        }
    } else if (facmode && nk_scan(&args, "get %w %e", key, sizeof(key))) {
        void *loc = &cal;
        const struct type *t = xpath(key, &tyCAL, &loc);
        if (!t) {
            printf("Not found.\n");
        } else {
            printf("BEGIN\n");
            print_val(t, loc, 0, "\n");
            printf("END\n");
        }
    } else if (facmode && nk_scan(&args, "set %w ", key, sizeof(key))) {
        void *loc = &cal;
        const struct type *t = xpath(key, &tyCAL, &loc);
        if (!t) {
            printf("Not found.\n");
        } else {
            if (nk_scan(&args, "%v %e", t, (void *)loc)) {
                printf("Setting %s to ", key);
                print_val(t, loc, 0, "\n");
            } else if (nk_scan(&args, " %e")) {
                char *l = multi_gets();
                if (nk_scan(&l, " %v %e", t, (void *)loc)) {
                    printf("Setting %s to ", key);
                    print_val(t, loc, 0, "\n");
                } else {
                    printf("Syntax error\n");
                }
            } else {
                printf("Syntax error\n");
            }
        }
    } else {
        printf("Syntax error\n");
    }
    return 0;
}

int cmd_model(char *args)
{
    char key[80];
    if (facmode && nk_scan(&args, "clear %e")) {
        printf("Clearing model store\n");
        keyval_model_clear();
        keyval_model_flush();
        keyval_model_flush();
        printf("done.\n");
        cmd_reboot("");
    } else if (facmode && nk_scan(&args, "flush %e")) {
        keyval_model_flush();
    } else if (facmode && nk_scan(&args, "load %e")) {
        keyval_model_load();
    } else if (facmode && nk_scan(&args, "show %e")) {
        printf("BEGIN\n");
        print_val(&tyMODEL, &model, 0, "\n");
        printf("END\n");
    } else if (facmode && nk_scan(&args, "replace %e")) {
        char *l = multi_gets();
        model = model_defaults;
        if (nk_scan(&l, " %v %e", &tyMODEL, &model)) {
            printf("Replacing model store with ");
            print_val(&tyMODEL, &model, 0, "\n");
        } else {
            model = model_defaults;
            printf("Syntax error\n");
        }
    } else if (facmode && nk_scan(&args, "get %w %e", key, sizeof(key))) {
        void *loc = &model;
        const struct type *t = xpath(key, &tyMODEL, &loc);
        if (!t) {
            printf("Not found.\n");
        } else {
            printf("BEGIN\n");
            print_val(t, loc, 0, "\n");
            printf("END\n");
        }
    } else if (facmode && nk_scan(&args, "set %w ", key, sizeof(key))) {
        void *loc = &model;
        const struct type *t = xpath(key, &tyMODEL, &loc);
        if (!t) {
            printf("Not found.\n");
        } else {
            if (nk_scan(&args, "%v %e", t, (void *)loc)) {
                printf("Setting %s to ", key);
                print_val(t, loc, 0, "\n");
            } else if (nk_scan(&args, " %e")) {
                char *l = multi_gets();
                if (nk_scan(&l, " %v %e", t, (void *)loc)) {
                    printf("Setting %s to ", key);
                    print_val(t, loc, 0, "\n");
                } else {
                    printf("Syntax error\n");
                }
            } else {
                printf("Syntax error\n");
            }
        }
    } else {
        printf("Syntax error\n");
    }
    return 0;
}

int cmd_config(char *args)
{
    char key[80];
    if (nk_scan(&args, "clear %e")) {
        printf("Clearing configuration store\n");
        keyval_config_clear();
        keyval_config_flush();
        keyval_config_flush();
        printf("done.\n");
        cmd_reboot("");
    } else if (nk_scan(&args, "flush %e")) {
        keyval_config_flush();
    } else if (nk_scan(&args, "load %e")) {
        keyval_config_load();
    } else if (nk_scan(&args, "show %e")) {
        printf("BEGIN\n");
        print_val(&tyCONFIG, &config, 0, "\n");
        printf("END\n");
    } else if (nk_scan(&args, "replace %e")) {
        char *l = multi_gets();
        config = config_defaults;
        if (nk_scan(&l, " %v %e", &tyCONFIG, &config)) {
            printf("Replacing keyval store with ");
            print_val(&tyCONFIG, &config, 0, "\n");
        } else {
            config = config_defaults;
            printf("Syntax error\n");
        }
    } else if (facmode && nk_scan(&args, "get %w %e", key, sizeof(key))) {
        void *loc = &config;
        const struct type *t = xpath(key, &tyCONFIG, &loc);
        if (!t) {
            printf("Not found.\n");
        } else {
            printf("BEGIN\n");
            print_val(t, loc, 0, "\n");
            printf("END\n");
        }
    } else if (facmode && nk_scan(&args, "set %w ", key, sizeof(key))) {
        void *loc = &config;
        const struct type *t = xpath(key, &tyCONFIG, &loc);
        if (!t) {
            printf("Not found.\n");
        } else {
            if (nk_scan(&args, "%v %e", t, (void *)loc)) {
                printf("Setting %s to ", key);
                print_val(t, loc, 0, "\n");
            } else if (nk_scan(&args, " %e")) {
                char *l = multi_gets();
                if (nk_scan(&l, " %v %e", t, (void *)loc)) {
                    printf("Setting %s to ", key);
                    print_val(t, loc, 0, "\n");
                } else {
                    printf("Syntax error\n");
                }
            } else {
                printf("Syntax error\n");
            }
        }
    } else {
        printf("Syntax error\n");
    }
    return 0;
}
