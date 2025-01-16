// Schema compiler

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
#include <stdio.h>
#include <string.h>

#include "lex.h"
#include "tree.h"
#include "y.tab.h"

extern int show_tree;

char *filename = NULL;

int main(int argc, char *argv[])
{
        int c;
        int x;
        char *top = NULL;
        int justlex = 0;
        int dump = 0;

        set_macro("__SCOMP__", NULL, NULL, 0); // Define to indicate we're running in the schema compiler

        // Print defines: gcc -dM -E - < /dev/null
        // Include path from: echo | clang -E -Wp,-v -
        // add_path("/usr/lib/gcc/x86_64-pc-cygwin/7.3.0/include");
        //add_path("/usr/lib/gcc/x86_64-linux-gnu/7.5.0/include");

        add_path("/usr/include");
        add_path("/usr/lib/gcc/x86_64-linux-gnu/11/include"); // gcc
        //add_path("/usr/lib/llvm-14/lib/clang/14.0.0/include"); // clang
        add_path("/usr/local/include");
        add_path("/usr/include/x86_64-linux-gnu");

        //add_path("/usr/include/w32api");

        for (x = 1; argv[x]; ++x)
                if (argv[x+1] && !strcmp(argv[x], "--top")) {
                        top = argv[x+1];
                        ++x;
                } else if (!strcmp(argv[x], "--lex")) {
                        justlex = 1;
                } else if (argv[x+1] && !strcmp(argv[x], "-I")) {
                        add_path(argv[x+1]);
                        ++x;
                } else if (!strcmp(argv[x], "--dump")) {
                        dump = 1;
                } else if (!strcmp(argv[x], "--tree")) {
                        show_tree = 1;
                } else if (!strcmp(argv[x], "--help") || !strcmp(argv[x], "-h")) {
                        printf("%s [options] filename\n", argv[0]);
                        printf("   --top <structname>     Select top-level structure\n");
                        printf("   --lex                  Show lexical analyzer output only for debugging it\n");
                        printf("   --dump                 Show collected tables for debugging\n");
                        printf("   --tree                 Show AST for debugging\n");
                        printf("   -I <path>              Add include file path\n");
                        return 0;
                } else {
                        filename = argv[x];
                }

        if (filename)
        {
                if (open_file(filename))
                {
                        fprintf(stderr,"Couldn't open file %s\n", argv[1]);
                        return -1;
                }
        }
        else
        {
                fprintf(stderr, "Missing filename\n");
                return -1;
        }

        if (justlex) {
                do {
                        c = get_tok(0);
                        show_tok(c);
                } while(c != tEOF);
                show_macros();
                return 0;
        }

        c = yyparse();
        if (c) {
                printf("yyparse returned %d\n", c);
        }

        if (dump)
        {
                printf("Typedefs:\n");
                show_typedefs();
                printf("Structs:\n");
                show_structs();
                printf("Unions:\n");
                show_unions();
        }

        if (top) {
                gen_meta(top);
        }

        return 0;
}
