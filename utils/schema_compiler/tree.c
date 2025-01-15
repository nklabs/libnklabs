// Syntax tree

// Copyright 2024 Joseph Allen

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

#include <stdio.h>
#include "lex.h"
#include "tree.h"

void ind(int n)
{
    while (n--) printf(" ");
}

void print_tree(struct node *n)
{
    if (n)
    {
        switch (n->what)
        {
            case ncCHAR: printf("(char)%ld", n->i); break;
            case ncUCHAR: printf("(unsigned char)%llu", (unsigned long long)n->i); break;

            case ncSHORT: printf("(short)%ld", n->i); break;
            case ncUSHORT: printf("(unsigned short)%llu", (unsigned long long)n->i); break;

            case ncLONG: printf("(long)%ld", n->i); break;
            case ncULONG: printf("(unsigned long)%llu", (unsigned long long)n->i); break;

            case ncFLOAT: printf("(float)%g", n->f); break;
            case ncDOUBLE: printf("(double)%g", n->d); break;

            case ncSTRING: printf("\"%s\"", n->s); break;

            case ncNAME: printf("%s", n->s); break;

            case ntSCHAR: printf("schar"); break;
            case ntBOOL: printf("bool"); break;
            case ntCHAR: printf("char"); break;
            case ntUCHAR: printf("unsigned char"); break;
            case ntSHORT: printf("short"); break;
            case ntUSHORT: printf("unsigned short"); break;
            case ntLONG: printf("long"); break;
            case ntULONG: printf("unsigned long"); break;
            case ntFLOAT: printf("float"); break;
            case ntDOUBLE: printf("double"); break;
            case ntPTR: printf("* "); print_tree(n->r); break;
            case ntARRAY: printf("[%zu] ", n->siz); print_tree(n->r); break;
            case ntSTRUCT:
            {
              if (n->uct->body)
              {
                  printf("struct %s { ", n->s ? n->s : "NULL"); print_tree(n->uct->body); printf(" }");
              }
              else
              {
                  // Anonymous structure
                  printf("struct %s ", n->s ? n->s : "NULL");
              }
              break;
            }
            case ntUNION: printf("union %s { ", n->s ? n->s : "NULL"); print_tree(n->r); printf(" }"); break;

            case nLIST: print_tree(n->l); printf(", "); print_tree(n->r); break;
            case nDECL: printf("DCL %s ", n->s); print_tree(n->r); break;
            case nNOTHING: printf("nothing"); break;

            default: printf("UNKNOWN<%d>", n->what); break;
        }
    }
    else
    {
        printf("NULL");
    }
}

Node *list_append(Node *l, Node *r)
{
    if (!l) return r;
    else {
        l->r = list_append(l->r, r);
        return l;
    }
}

struct nodelist {
    struct nodelist *next;
    char *name;
    Node *body;
};

Node *node_lookup(struct nodelist *l, char *s)
{
    while (l)
        if (!strcmp(l->name, s))
            return l->body;
        else
            l = l->next;
    return 0;
}

struct nodelist *node_add(struct nodelist *l, char *s, Node *body)
{
    struct nodelist *n = (struct nodelist *)malloc(sizeof(struct nodelist));
    n->next = l;
    n->name = s;
    n->body = body;
    return n;
}

void show_nodes(struct nodelist *l)
{
    while (l)
    {
        printf("%s: ", l->name);
        print_tree(l->body);
        printf("\n");
        l = l->next;
    }
}

// Typedef table

struct nodelist *typedefs;

Node *lookup_typedef(char *s)
{
    return node_lookup(typedefs, s);
}

int add_typedef(char *s, Node *body)
{
    if (lookup_typedef(s))
        return -1;
    else {
        typedefs = node_add(typedefs, s, body);
        return 0;
    }
}

void show_typedefs()
{
    show_nodes(typedefs);
}

// Struct table

struct structbase *structs;

struct structbase *lookup_struct(char *s)
{
    struct structbase *l;
    for (l = structs; l; l = l->next)
        if (!strcmp(l->name, s))
            return l;
    return 0;
}

struct structbase *add_struct(char *s, Node *body)
{
    if (lookup_struct(s))
        return 0;
    else {
        struct structbase *n = (struct structbase *)calloc(1, sizeof(struct structbase));
        n->next = structs;
        n->name = s;
        n->body = body;
        structs = n;
        return n;
    }
}

void show_structs()
{
    struct structbase *l;
    for (l = structs; l; l = l->next)
    {
        printf("%s: ", l->name);
        print_tree(l->body);
        printf("\n");
    }
}

// Union table

struct nodelist *unions;

Node *lookup_union(char *s)
{
    return node_lookup(unions, s);
}

int add_union(char *s, Node *body)
{
    if (lookup_union(s))
        return -1;
    else {
        unions = node_add(unions, s, body);
        return 0;
    }
}

void show_unions()
{
    show_nodes(unions);
}

void gen_members(char *name, Node *n)
{
    if (n->what == nLIST) {
        gen_members(name, n->l);
        gen_members(name, n->r);
    } else if (n->what == nNOTHING) {
    } else if (n->what == nDECL) {
        char tname[80];
        switch (n->r->what) {
            case ntARRAY:
            {
                if (n->r->r->what == ntSCHAR)
                    sprintf(tname, "ty_%s_%s_string", name, n->s);
                else
                    sprintf(tname, "ty_%s_%s_array", name, n->s);
                break;
            }
            case ntSTRUCT: sprintf(tname, "ty_%s", n->r->s); break;
            case ntBOOL: sprintf(tname, "tyBOOL"); break;
            case ntCHAR: sprintf(tname, "tyINT8"); break;
            case ntUCHAR: sprintf(tname, "tyUINT8"); break;
            case ntSHORT: sprintf(tname, "tyINT16"); break;
            case ntUSHORT: sprintf(tname, "tyUINT16"); break;
            case ntLONG: sprintf(tname, "tyINT"); break;
            case ntULONG: sprintf(tname, "tyUINT"); break;
            case ntFLOAT: sprintf(tname, "tyFLOAT"); break;
            case ntDOUBLE: sprintf(tname, "tyDOUBLE"); break;
            default: sprintf(tname, "invalid"); break;
        }
        printf("    { \"%s\", &%s, offsetof(struct %s, %s), 0 },\n", n->s, tname, name, n->s);
    } else {
        printf("Huh?\n");
    }
}

void gen_array(char *owner, char *member, Node *n, int depth);
void gen_struct(struct structbase *n, int top);

void recur_members(char *name, Node *n)
{
    if (n->what == nLIST) {
        recur_members(name, n->l);
        recur_members(name, n->r);
    } else if (n->what == nNOTHING) {
    } else if (n->what == nDECL) {
        char tname[80];
        switch (n->r->what) {
            case ntARRAY: gen_array(name, n->s, n->r->r, 0); break;
            case ntSTRUCT: gen_struct(n->r->uct, 0); break;
            default: break;
        }
    } else {
        printf("Huh?\n");
    }
}

void gen_struct(struct structbase *n, int top)
{
    if (!n->reached)
    {
        n->reached = true;
        if (n->body)
        {
            recur_members(n->name, n->body);

            printf("const struct member %s_members[] = {\n", n->name);
            gen_members(n->name, n->body);
            printf("    { NULL, NULL, 0, 0 }\n");
            printf("};\n");
            printf("\n");
        }
        else
        {
            printf("Error: struct %s not defined, can not generate %s_members\n", n->name, n->name);
        }

        if (top)
            printf("struct type ty_%s = {\n", n->name);
        else
            printf("const struct type ty_%s = {\n", n->name);
        printf("    .what = tSTRUCT,\n");
        printf("    .size = sizeof(struct %s),\n", n->name);
        printf("    .members = %s_members,\n", n->name);
        printf("    .subtype = NULL,\n");
        printf("    .check = NULL\n");
        printf("};\n\n");
    }
}

void gen_array(char *owner, char *member, Node *n, int depth)
{
    char tname[80];
    int i;

    // If base type is schar, it's a string
    if (n->what == ntSCHAR)
    {
        printf("const struct type ty_%s_%s_string = {\n", owner, member);
        printf("    .what = tSTRING,\n");
        printf("    .size = nk_member_size(struct %s, %s", owner, member);
        for (i = 0; i != depth; ++i)
            printf("[0]");
        printf("),\n");
        printf("    .members = NULL,\n");
        printf("    .subtype = NULL,\n");
        printf("    .check = NULL\n");
        printf("};\n\n");
        return;
    }

    if (n->what == ntARRAY)
    {
        gen_array(owner, member, n->r, depth + 1);
    }

    if (depth)
        printf("const struct type ty_%s_%s_array%d = {\n", owner, member, depth);
    else
        printf("const struct type ty_%s_%s_array = {\n", owner, member);
    printf("    .what = tARRAY,\n");

    printf("    .size = nk_member_size(struct %s, %s", owner, member);
    for (i = 0; i != depth; ++i)
        printf("[0]");
    printf("),\n");

    printf("    .members = NULL\n");
    
    switch (n->what) {
        case ntARRAY:
        {
            if (n->r->what == ntSCHAR)
                sprintf(tname, "ty_%s_%s_string", owner, member);
            else
                sprintf(tname, "ty_%s_%s_array%d", owner, member, depth + 1);
            break;
        }
        case ntSTRUCT: sprintf(tname, "ty_%s", n->s); break;
        case ntBOOL: sprintf(tname, "tyBOOL"); break;
        case ntCHAR: sprintf(tname, "tyINT8"); break;
        case ntUCHAR: sprintf(tname, "tyUINT8"); break;
        case ntSHORT: sprintf(tname, "tyINT16"); break;
        case ntUSHORT: sprintf(tname, "tyUINT16"); break;
        case ntLONG: sprintf(tname, "tyINT"); break;
        case ntULONG: sprintf(tname, "tyUINT"); break;
        case ntFLOAT: sprintf(tname, "tyFLOAT"); break;
        case ntDOUBLE: sprintf(tname, "tyDOUBLE"); break;
        default: sprintf(tname, "invalid"); break;
    }

    printf("    .subtype = &%s,\n", tname);
    printf("    .check = NULL\n");
    printf("};\n\n");
}

extern char *filename;

void gen_meta(char *name)
{
    struct structbase *n = lookup_struct(name);
    if (!n) {
        printf("Couldn't find 'struct %s'\n", name);
        return ;
    }

    printf("/* This file was autogenerated by the libnklabs schema compiler */\n");
    printf("\n");
    printf("#include \"nkserialize.h\"\n");
    printf("#include \"%s\"\n", filename);
    printf("\n");

    gen_struct(n, 1);
}
