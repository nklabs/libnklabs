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

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct node {
    int what;
    union {
        double d;
        float f;
        long i;
        unsigned long u;
        struct {
            union {
                char *s; // String or name pointer
                size_t siz; // Array size
                struct node *l;
            };
            union {
                size_t len; // String length
                struct node *r;
                struct structbase *uct;
            };
        };
    };
} Node;

struct structbase
{
    struct structbase *next;
    char *name;
    Node *body;
    bool reached;
};

enum {
    // Constants
    ncCHAR,
    ncUCHAR,
    ncSHORT,
    ncUSHORT,
    ncLONG,
    ncULONG,
    ncFLOAT,
    ncDOUBLE,
    ncSTRING,
    ncNAME,

    // Types
    ntVOID,
    ntCHAR,
    ntUCHAR,
    ntSHORT,
    ntUSHORT,
    ntLONG,
    ntULONG,
    ntFLOAT,
    ntDOUBLE,
    ntPTR, /* Pointer to type in r */
    ntARRAY, /* Array: type in r, array size in siz */
    ntSTRUCT, /* Structure: list of declarations in r, Name in s */
    ntUNION, /* Union: list of declarations in r, Name in s */

    // Declaration
    nDECL, /* Type in r, Name in s */
    nLIST, /* Content in l, Next in r */
    nNOTHING, /* There is nothing here. */
};

//#define cons(...) ({ Node *n = (Node *)malloc(sizeof(Node)); *n = (Node){ __VA_ARGS__ }; n; })
#define cons(...) (Node *)memcpy(malloc(sizeof(Node)), &(Node){ __VA_ARGS__ }, sizeof(Node))

struct llvalterm {
    char *file_name;
    int line;
};

struct llvalnum {
    char *file_name;
    int line;
    struct num num;
};

struct llvalfp {
    char *file_name;
    int line;
    double fp;
};

struct llvals {
    char *file_name;
    int line;
    char *s;
};

struct llvaln {
    char *file_name;
    int line;
    Node *n;
};


void print_tree(Node *n);
Node *list_append(Node *l, Node *r);

// Typedef table

Node *lookup_typedef(char *s);
int add_typedef(char *s, Node *body);
void show_typedefs();

// Struct table

struct structbase *lookup_struct(char *s);
struct structbase *add_struct(char *s, Node *body);
void show_structs();

// Union table

Node *lookup_union(char *s);
int add_union(char *s, Node *body);
void show_unions();

void gen_meta(char *name);
