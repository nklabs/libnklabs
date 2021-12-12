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

// Key value store

// The data is stored in the SPI Flash

#include "lib.h"

/* Compute hash value */

#define hnext(accu, c) (((accu) << 4) + ((accu) >> 28) + (c))

unsigned hash(const char *s)
{
        unsigned accu = 0;
        while (*s)
                accu = hnext(accu, *(unsigned char *)s++);
        return accu;
}

/* Find hash table index which should be used for name.  Indicated struct keyval_entry either has matching name or NULL. */

static int slot_for_string(struct keyval_hash *t, const char *name)
{
        int x;

        for (
                x = (hash(name) & t->str_tab_mask);
                t->str_tab[x].name && strcmp(t->str_tab[x].name, name);
                x = ((x + 1) & t->str_tab_mask)
        );

        return x;
}

struct value *get_by_string(struct keyval_hash *t, const char *name)
{
        int x = slot_for_string(t, name);
        if (t->str_tab[x].name)
                return &t->str_tab[x].value;
        else
                return NULL;
}

/* Expand hash table to avoid clashes */

static void expand_string_tab(struct keyval_hash *t)
{
        int x, y;
        int new_tab_size = (t->str_tab_mask + 1) * 2;
        int new_tab_mask = new_tab_size - 1;

        struct keyval_entry *new_tab = (struct keyval_entry *)calloc(new_tab_size, sizeof(struct keyval_entry));

        for (y = 0; y != t->str_tab_mask + 1; ++y) {
                if (t->str_tab[y].name) {
                        for (x = (hash(t->str_tab[y].name) & new_tab_mask); new_tab[x].name; x = ((x + 1) & new_tab_mask));
                        new_tab[x] = t->str_tab[y];
                }
        }

        free(t->str_tab);

        t->str_tab = new_tab;
        t->str_tab_mask = new_tab_mask;
}

struct value *set_by_string(struct keyval_hash *t, const char *name)
{
        int x = slot_for_string(t, name);

        if (t->str_tab[x].name) {
                // It already exists
                return &t->str_tab[x].value;
        } else {
                // It's new, create it
                if (t->str_tab_count == t->str_tab_mask) {
                        expand_string_tab(t);
                        x = slot_for_string(t, name);
                }
                ++t->str_tab_count;
                t->str_tab[x].name = strdup(name);
                return &t->str_tab[x].value;
        }
}

struct keyval_hash *alloc_hash()
{
        struct keyval_hash *o;
        int str_size = 8;

        o = (struct keyval_hash *)calloc(1, sizeof(struct keyval_hash));

        o->str_tab_mask = str_size - 1;
        o->str_tab = (struct keyval_entry *)calloc(str_size, sizeof(struct keyval_entry));
        o->str_tab_count = 0;

        return o;
}

void free_hash(struct keyval_hash *o)
{
        if (o->str_tab) {
                int x;
                for (x = 0; x != o->str_tab_mask + 1; ++x) {
                    if (o->str_tab[x].name)
                        free(o->str_tab[x].name);
                    free_val(&o->str_tab[x].value);
                }
                free(o->str_tab);
        }
        free(o);
}

struct keyval_table *alloc_table()
{
    struct keyval_table *t;
    t = (struct keyval_table *)calloc(1, sizeof(struct keyval_table));
    return t;
}

struct keyval_table *alloc_table_sized(int nrows, int ncols)
{
    struct keyval_table *t;
    int y;
    t = (struct keyval_table *)calloc(1, sizeof(struct keyval_table));
    if (!t)
        return 0;
    t->nrows = nrows;
    t->ncols = ncols;
    t->colname = (char **)calloc(sizeof(char *), ncols);
    if (!t->colname) {
        free_table(t);
        return 0;
    }
    t->row = (struct value **)calloc(sizeof(struct value *), nrows);
    if (!t->row) {
        free_table(t);
        return 0;
    }
    for (y = 0; y != nrows; ++y)
        if (!(t->row[y] = (struct value *)calloc(sizeof(struct value), ncols))) {
            free_table(t);
            return 0;
        }
    return t;
}

void free_table(struct keyval_table *t)
{
    int x, y;
    if (t) {
        if (t->colname) {
            for (x = 0; x != t->ncols; ++x)
                if (t->colname[x])
                    free(t->colname[x]);
            free(t->colname);
        }
        if (t->row) {
            for (y = 0; y != t->nrows; ++y) {
                if (t->row[y]) {
                    for (x = 0; x != t->ncols; ++x)
                        free_val(&t->row[y][x]);
                    free(t->row[y]);
                }
            }
            free(t->row);
        }
        free(t);
    }
}

void table_set_colname(struct keyval_table *t, int x, char *name)
{
    if (t->colname[x])
        free(t->colname[x]);
    t->colname[x] = strdup(name);
}

void table_add_col(struct keyval_table *t, char *name)
{
    if (t->ncols) {
        t->colname = (char **)realloc(t->colname, sizeof(char *) * (t->ncols + 1));
    } else {
        t->colname = (char **)calloc(sizeof(char *), 1);
    }
    t->colname[t->ncols] = strdup(name);
    ++t->ncols;
}

void table_add_row(struct keyval_table *t)
{
    if (t->nrows) {
        t->row = (struct value **)realloc(t->row, sizeof(struct value *) * (t->nrows + 1));
    } else {
        t->row = (struct value **)calloc(sizeof(struct value *), 1);
    }
    t->row[t->nrows] = (struct value *)calloc(sizeof(struct value), t->ncols);
    ++t->nrows;
    t->col_idx = 0;
}

void table_add_field(struct keyval_table *t, struct value v)
{
    t->row[t->nrows - 1][t->col_idx++] = v;
}

void table_set_field(struct keyval_table *t, int row, int col, struct value v)
{
    free_val(&t->row[row][col]);
    t->row[row][col] = v;
}

struct value *get_by_number(struct keyval_array *t, int num)
{
    if (num >= t->ary_len || num < 0)
        return NULL;
    else
        return &t->ary[num];
}

struct value *set_by_number(struct keyval_array *t, int num)
{
    if (num < 0)
        return NULL;

    if (num >= t->ary_len) {
        if (num >= t->ary_size) {
            t->ary = (struct value *)realloc(t->ary, sizeof(struct value) * (num + 16));
            memset(t->ary + t->ary_size, 0, sizeof(struct value) * (num + 16 - t->ary_size));
            t->ary_size = num + 16;
        }
        t->ary_len = num + 1;
    }

    return &t->ary[num];
}

struct keyval_array *alloc_array()
{
    struct keyval_array *o;
    int array_size = 8;

    o = (struct keyval_array *)calloc(1, sizeof(struct keyval_array));

    o->ary_size = array_size;
    o->ary_len = 0;
    o->ary = (struct value *)calloc(array_size, sizeof(struct value));

    return o;
}

void free_array(struct keyval_array *o)
{
    if (o->ary) {
        int x;
        for (x = 0; x != o->ary_len; ++x) {
            free_val(&o->ary[x]);
        }
        free(o->ary);
    }
    free(o);
}

void free_val(struct value *v)
{
    switch (v->what) {
        case tHASH: {
            free_hash(v->u.hash);
            break;
        } case tARRAY: {
            free_array(v->u.array);
            break;
        } case tSTR: {
            free(v->u.str);
            break;
        } case tTABLE: {
            free_table(v->u.table);
            break;
        }
    }
}

void indent(int x)
{
    while (x--)
        printf(" ");
}

void print_val(struct value *v, int ind, char *m)
{
    char sbuf[32];
    switch (v->what) {
        case tHASH: {
            int x;
            char *o_name = 0;
            struct value *o_val = 0;
            printf("{\n");
            for (x = 0; x != v->u.hash->str_tab_mask + 1; ++x) {
                if (v->u.hash->str_tab[x].name) {
                    if (o_name) {
                        indent(ind + 2); printf("%s: ", o_name);
                        print_val(o_val, ind + 2, ",\n");
                    }
                    o_name = v->u.hash->str_tab[x].name;
                    o_val = &v->u.hash->str_tab[x].value;
                }
            }
            if (o_name) {
                indent(ind + 2); printf("%s: ", o_name);
                print_val(o_val, ind + 2, "\n");
            }
            indent(ind); printf("}%s", m);
            break;
        } case tTABLE: {
            int x, y;
            printf("(");
            for (x = 0; x != v->u.table->ncols; ++x)
                printf("\t%s", v->u.table->colname[x]);
            printf("\n");
            for (y = 0; y != v->u.table->nrows; ++y) {
                indent(ind); printf(":");
                for (x = 0; x != v->u.table->ncols; ++x) {
                    printf("\t");
                    print_val(&v->u.table->row[y][x], 0, "");
                }
                printf("\n");
            }
            indent(ind); printf(")%s", m);
            break;
        } case tARRAY: {
            int x;
            printf("[\n");
            for (x = 0; x != v->u.array->ary_len; ++x) {
                indent(ind + 2);
                if (x + 1 == v->u.array->ary_len)
                    print_val(&v->u.array->ary[x], ind + 2, "\n");
                else
                    print_val(&v->u.array->ary[x], ind + 2, ",\n");
            }
            indent(ind); printf("]%s", m);
            break;
        } case tVOID: {
            printf("null%s", m);
            break;
        } case tFP: {
            int x;
            // Print double as decimal with enough precision to
            // recover the original double on read in.
            // (should be %.17g, but uses too much space)
            sprintf(sbuf, "%g", v->u.fp);
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
            printf("%s%s", sbuf, m);
            break;
        } case tNUM: {
            printf("%d%s", v->u.num, m);
            break;
        } case tBOOL: {
            if (v->u.truth)
                printf("true%s", m);
            else
                printf("false%s", m);
            break;
        } case tSTR: {
            int x;
            printf("\"");
            for (x = 0; v->u.str[x]; ++x) {
                if (v->u.str[x] == '\"')
                    printf("\\\"");
                else if (v->u.str[x] == '\\')
                    printf("\\\\");
                else if (v->u.str[x] == '\n')
                    printf("\\n");
                else if (v->u.str[x] == '\r')
                    printf("\\r");
                else if (v->u.str[x] < 32 || v->u.str[x] > 126)
                    printf("\\x%2.2x", (0xFF & v->u.str[x]));
                else
                    printf("%c", v->u.str[x]);
            }
            printf("\"%s", m);
            break;
        } default: {
            printf("unknown%s", m);
            break;
        }
    }
}

int serialize_val(char *org, struct value *v, int more)
{
    char sbuf[32];
    char *buf = org;
    switch (v->what) {
        case tHASH: {
            int x;
            char *o_name = 0;
            struct value *o_val = 0;
            *buf++ = '{';
            for (x = 0; x != v->u.hash->str_tab_mask + 1; ++x) {
                if (v->u.hash->str_tab[x].name) {
                    if (o_name) {
                        sprintf(buf, "%s:", o_name);
                        buf += strlen(buf);
                        buf += serialize_val(buf, o_val, 1);
                    }
                    o_name = v->u.hash->str_tab[x].name;
                    o_val = &v->u.hash->str_tab[x].value;
                }
            }
            if (o_name) {
                    sprintf(buf, "%s:", o_name);
                    buf += strlen(buf);
                    buf += serialize_val(buf, o_val, 0);
            }
            *buf++ = '}';
            break;
        } case tARRAY: {
            int x;
            *buf++ = '[';
            for (x = 0; x != v->u.array->ary_len; ++x) {
                if (x + 1 == v->u.array->ary_len)
                    buf += serialize_val(buf, &v->u.array->ary[x], 0);
                else
                    buf += serialize_val(buf, &v->u.array->ary[x], 1);
            }
            *buf++ = ']';
            break;
        } case tTABLE: {
            int x, y;
            *buf++ = '(';
            if (v->u.table->ncols) {
                sprintf(buf, "%s", v->u.table->colname[0]);
                buf += strlen(buf);
            }
            for (x = 1; x < v->u.table->ncols; ++x) {
                sprintf(buf, " %s", v->u.table->colname[x]);
                buf += strlen(buf);
            }
            for (y = 0; y != v->u.table->nrows; ++y) {
                *buf++ = ':';
                if (v->u.table->ncols) {
                    buf += serialize_val(buf, &v->u.table->row[y][0], 0);
                }
                for (x = 1; x < v->u.table->ncols; ++x) {
                    *buf++ = ' ';
                    buf += serialize_val(buf, &v->u.table->row[y][x], 0);
                }
            }
            *buf++ = ')';
            break;
        } case tVOID: {
            sprintf(buf, "null");
            buf += strlen(buf);
            break;
        } case tFP: {
            int x;
            // Print double as decimal with enough precision to
            // recover the original double on read in.
            // (should be %.17g)
            sprintf(sbuf, "%g", v->u.fp);
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
            strcpy(buf, sbuf);
            buf += strlen(buf);
            break;
        } case tNUM: {
            sprintf(buf, "%d", v->u.num);
            buf += strlen(buf);
            break;
        } case tBOOL: {
            if (v->u.truth) {
                strcpy(buf, "true");
            } else {
                strcpy(buf, "false");
            }
            buf += strlen(buf);
            break;
        } case tSTR: {
            int x;
            *buf++ = '"';
            for (x = 0; v->u.str[x]; ++x) {
                if (v->u.str[x] == '\"') {
                    *buf++ = '\\';
                    *buf++ = '"';
                } else if (v->u.str[x] == '\\') {
                    *buf++ = '\\';
                    *buf++ = '\\';
                } else if (v->u.str[x] == '\n') {
                    *buf++ = '\\';
                    *buf++ = 'n';
                } else if (v->u.str[x] == '\r') {
                    *buf++ = '\\';
                    *buf++ = 'r';
                } else if (v->u.str[x] < 32 || v->u.str[x] > 126) {
                    *buf++ = '\\';
                    *buf++ = 'x';
                    sprintf(buf, "%2.2x", (0xff & v->u.str[x]));
                    buf += strlen(buf);
                } else
                    *buf++ = v->u.str[x];
            }
            *buf++ = '"';
            break;
        } default: {
            printf("Unknown value type??\n");
            sprintf(buf, "void");
            buf += strlen(buf);
            break;
        }
    }
    if (more)
        *buf++ = ',';
    return buf - org;
}

int keyval_read_raw(struct value *database, char *key, struct value *val)
{
    char buf[80];
    char *p = key;
    int idx;
    struct value *v = database;

    while (*p) {
        if (nk_scan(&p, "[ %d ]", &idx)) {
            if (v->what != tARRAY) {
                printf("Attempt to index a non-array in key %s\n", key);
                return -1;
            }
            v = get_by_number(v->u.array, idx);
            if (!v) {
                printf("Array index out of bounds in key %s\n", key);
                return -1;
            }
            if (*p == '.')
                ++p;
            else if (*p == '[')
                ;
            else
                break;
        } else if (nk_scan(&p, "%i", buf, sizeof(buf))) {
            if (v->what != tHASH) {
                printf("Attempt to index a non-hash in key %s\n", key);
                return -1;
            }
            v = get_by_string(v->u.hash, buf);
            if (!v) {
                printf("Item does not exist %s\n", buf);
                return -1;
            }
            if (*p == '.')
                ++p;
            else if (*p == '[')
                ;
            else
                break;
        } else {
            printf("Syntax error in key %s\n", key);
            return -1;
        }
    }
    if (*p) {
        printf("Syntax error in key %s\n", key);
        return -1;
    }
    *val = *v;
    return 0;
}

int keyval_write_raw(struct value *database, char *key, struct value val)
{
    char buf[80];
    char *p = key;
    int idx;
    struct value *v = database;

    while (*p) {
        if (nk_scan(&p, "[ %d ]", &idx)) {
            if (v->what != tARRAY) {
                if (v == database) {
                    printf("Attempt to use root as an array\n");
                    return -1;
                }
                free_val(v);
                v->what = tARRAY;
                v->u.array = alloc_array();
            }
            v = set_by_number(v->u.array, idx);
            if (!v) {
                printf("Array index out of bounds in key %s\n", key);
                return -1;
            }
            if (*p == '.')
                ++p;
            else if (*p == '[')
                ;
            else
                break;
        } else if (nk_scan(&p, "%i", buf, sizeof(buf))) {
            if (v->what != tHASH) {
                free_val(v);
                v->what = tHASH;
                v->u.hash = alloc_hash();
            }
            v = set_by_string(v->u.hash, buf);
            if (!v) {
                printf("Item does not exist %s\n", buf);
                return -1;
            }
            if (*p == '.')
                ++p;
            else if (*p == '[')
                ;
            else
                break;
        } else {
            printf("Syntax error in key %s\n", key);
            return -1;
        }
    }
    if (*p) {
        printf("Syntax error in key %s\n", key);
        return -1;
    }
    free_val(v);
    *v = val;
    return 0;
}
