/* C-language subset for introspection */

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


%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lex.h"
#include "tree.h"

#define fCHAR 1
#define fSHORT 2
#define fINT 4
#define fLONG 8
#define fSIGNED 16
#define fUNSIGNED 32
#define fLONGLONG 64

int yylex();
void yyerror(char *s);

int anon_count = 0;
int show_tree = 0;

%}

%union {
    struct llvalterm term;
    struct llvalnum num;
    struct llvalfp fp;
    struct llvals s;
    struct llvaln n;
    struct llvalflag flag;
}

%token tEOF 0

%token <num> tNUM
%token <s> tSTRING
%token <s> tWORD
%token <fp> tFP

%token <term> tAUTO tBREAK tCASE tCHAR tCONST tCONTINUE
%token <term> tDEFAULT tDO tDOUBLE tELSE tENUM tEXTERN tFLOAT tFOR tGOTO tIF tINT
%token <term> tLONG tREGISTER tRETURN tSHORT tSIGNED tSIZEOF tSTATIC tSTRUCT tSWITCH
%token <term> tTYPEDEF tUNION tUNSIGNED tVOID tVOLATILE tWHILE
%token <term> tBOOL tSCHAR
%token <n> tTYPE

%type <num> const_expr

%type <n> input inside simple_type decl_name decl
%type <flag> flag_type

%left <term> '-' '+'
%left <term> '*' '/'
%left <term> '['

%token <term> ';'

/* These are not returned by the lexer */

%%

top : input { if (show_tree) { print_tree($1.n); printf("\n"); } };

input
  : %empty {
    $$.file_name = 0;
    $$.line = 0;
    $$.n = (void *)0;
} | input tTYPEDEF decl {
    $$ = $1;
    if ($3.n->s)
      add_typedef($3.n->s, $3.n->r);
} | input decl {
    if ($1.n) {
        $$.file_name = $1.file_name;
        $$.line = $1.line;
        $$.n = cons(.what = nLIST, .l = $1.n, .r = $2.n);
    } else
        $$ = $2;
} ;

inside
  : %empty {
    $$.file_name = 0;
    $$.line = 0;
    $$.n = (void *)0;
} | inside decl {
    if ($1.n) {
        $$.file_name = $1.file_name;
        $$.line = $1.line;
        $$.n = cons(.what = nLIST, .l = $1.n, .r = $2.n);
    } else
        $$ = $2;
} ;

// Build type by settings flags

flag_type
  : tCHAR {
    $$.file_name = $1.file_name;
    $$.line = $1.line;
    $$.flag = fCHAR;
} | tINT {
    $$.file_name = $1.file_name;
    $$.line = $1.line;
    $$.flag = fINT;
} | tSIGNED {
    $$.file_name = $1.file_name;
    $$.line = $1.line;
    $$.flag = fSIGNED;
} | tUNSIGNED {
    $$.file_name = $1.file_name;
    $$.line = $1.line;
    $$.flag = fUNSIGNED;
} | tSHORT {
    $$.file_name = $1.file_name;
    $$.line = $1.line;
    $$.flag = fSHORT;
} | tLONG {
    $$.file_name = $1.file_name;
    $$.line = $1.line;
    $$.flag = fLONG;
} | flag_type tCHAR {
    $$ = $1;
    if ($$.flag & (fCHAR | fINT | fSHORT | fLONG)) {
      printf("%s %d: Invalid type combination\n", $2.file_name, $2.line);
    } else {
      $$.flag |= fCHAR;
    }
} | flag_type tINT {
    $$ = $1;
    if ($$.flag & (fCHAR | fINT)) {
      printf("%s %d: Invalid type combination\n", $2.file_name, $2.line);
    } else {
      $$.flag |= fINT;
    }
} | flag_type tSIGNED {
    $$ = $1;
    if ($$.flag & (fCHAR | fSIGNED | fUNSIGNED)) {
      printf("%s %d: Invalid type combination\n", $2.file_name, $2.line);
    } else {
      $$.flag |= fSIGNED;
    }
} | flag_type tUNSIGNED {
    $$ = $1;
    if ($$.flag & (fCHAR | fSIGNED | fUNSIGNED)) {
      printf("%s %d: Invalid type combination\n", $2.file_name, $2.line);
    } else {
      $$.flag |= fUNSIGNED;
    }
} | flag_type tSHORT {
    $$ = $1;
    if ($$.flag & (fCHAR | fSHORT | fLONG)) {
      printf("%s %d: Invalid type combination\n", $2.file_name, $2.line);
    } else {
      $$.flag |= fSHORT;
    }
} | flag_type tLONG {
    $$ = $1;
    if ($$.flag & (fCHAR | fSHORT | fLONGLONG)) {
      printf("%s %d: Invalid type combination\n", $2.file_name, $2.line);
    } else if ($$.flag & fLONG) {
      $$.flag |= fLONGLONG;
    } else {
      $$.flag |= fLONG;
    }
} ;

simple_type
  : tVOID {
    $$.file_name = $1.file_name;
    $$.line = $1.line;
    $$.n = cons(.what = ntVOID);
} | tBOOL {
    $$.file_name = $1.file_name;
    $$.line = $1.line;
    $$.n = cons(.what = ntBOOL);
} | tSCHAR {
    $$.file_name = $1.file_name;
    $$.line = $1.line;
    $$.n = cons(.what = ntSCHAR);
} | flag_type {
    $$.file_name = $1.file_name;
    $$.line = $1.line;
    if ($1.flag & fLONGLONG)
    {
      // fLONGLONG and fLONG may both be set, in this case it's a long long
      if ($1.flag & fUNSIGNED)
        $$.n = cons(.what = ntULONGLONG);
      else
        $$.n = cons(.what = ntLONGLONG);
    }
    else if ($1.flag & fLONG)
    {
      if ($1.flag & fUNSIGNED)
        $$.n = cons(.what = ntULONG);
      else
        $$.n = cons(.what = ntLONG);
    }
    else if ($1.flag & fSHORT)
    {
      if ($1.flag & fUNSIGNED)
        $$.n = cons(.what = ntUSHORT);
      else
        $$.n = cons(.what = ntSHORT);
    }
    else if ($1.flag & fCHAR)
    {
      if ($1.flag & fUNSIGNED)
        $$.n = cons(.what = ntUCHAR);
      else
        $$.n = cons(.what = ntCHAR);
    }
    else
    {
      // Otherwise it must be an int
      if ($1.flag & fUNSIGNED)
        $$.n = cons(.what = ntULONG);
      else
        $$.n = cons(.what = ntLONG);
    }
} | tFLOAT {
    $$.file_name = $1.file_name;
    $$.line = $1.line;
    $$.n = cons(.what = ntFLOAT);
} | tDOUBLE {
    $$.file_name = $1.file_name;
    $$.line = $1.line;
    $$.n = cons(.what = ntDOUBLE);
} | tLONG tDOUBLE {
    $$.file_name = $1.file_name;
    $$.line = $1.line;
    $$.n = cons(.what = ntDOUBLE);
} | tDOUBLE tLONG {
    $$.file_name = $1.file_name;
    $$.line = $1.line;
    $$.n = cons(.what = ntDOUBLE);
} | tSTRUCT tWORD {
    struct structbase *u;
    u = lookup_struct($2.s);
    if (!u)
    {
      u = add_struct($2.s, NULL);
    }
    // printf("Find struct %s found %p\n", $2, u);
    $$.file_name = $1.file_name;
    $$.line = $1.line;
    $$.n = cons(.what = ntSTRUCT, .s = $2.s, .uct = u);
} | tUNION tWORD {
    Node *ty = lookup_union($2.s);
    $$.file_name = $1.file_name;
    $$.line = $1.line;
    $$.n = cons(.what = ntUNION, .s = $2.s, .r = ty);
} | tSTRUCT tWORD '{' inside '}' {
    struct structbase *u;
    $$.file_name = $1.file_name;
    $$.line = $1.line;

    u = lookup_struct($2.s);
    if (u)
    {
      if (u->body)
        printf("duplicate struct definition\n");
      else
      {
        // Fill in forward reference..
        u->body = $4.n;
      }
    }
    else
    {
        u = add_struct($2.s, $4.n);
    }

    $$.n = cons(.what = ntSTRUCT, .s = $2.s, .uct = u);
} | tUNION tWORD '{' inside '}' {
    $$.file_name = $1.file_name;
    $$.line = $1.line;
    $$.n = cons(.what = ntUNION, .s = $2.s, .r = $4.n);
    if (add_union($$.n->s, $$.n->r))
    {
        printf("duplicate union definition\n");
    }
} | tSTRUCT '{' inside '}' {
    // Anonymous structure...
    // Make up a name for it
    char anon_name[80];
    char *aname;
    sprintf(anon_name, "anon_%d", ++anon_count);
    aname = strdup(anon_name);

    struct structbase *u;
    $$.file_name = $1.file_name;
    $$.line = $1.line;

    u = lookup_struct(aname);
    if (u)
    {
      if (u->body)
        printf("duplicate struct definition\n");
      else
      {
        // Fill in forward reference..
        u->body = $3.n;
      }
    }
    else
    {
        u = add_struct(aname, $3.n);
    }

    $$.n = cons(.what = ntSTRUCT, .s = aname, .uct = u);

#if 0
    struct structbase *u = (struct structbase *)calloc(1, sizeof(struct structbase));
    u->body = $3.n;
    $$.file_name = $1.file_name;
    $$.line = $1.line;
    $$.n = cons(.what = ntSTRUCT, .s = NULL, .uct = u);
#endif
} | tUNION '{' inside '}' {
    $$.file_name = $1.file_name;
    $$.line = $1.line;
    $$.n = cons(.what = ntUNION, .s = NULL, .r = $3.n);
} | tTYPE {
    $$ = $1;
} ;

decl_name
 : tWORD {
   $$.file_name = $1.file_name;
   $$.line = $1.line;
   $$.n = cons(.what = nDECL, .s = $1.s, .r = NULL);
} | '(' decl_name ')' {
    $$ = $2;
} | '*' decl_name {
    $$ = $2;
    $$.n->r = list_append($$.n->r, cons(.what = ntPTR, .r = NULL));
} | decl_name '[' const_expr ']' {
    $$ = $1;
    $$.n->r = list_append($$.n->r, cons(.what = ntARRAY, .siz = $3.num.num, .r = NULL));
} ;

decl
  : error ';' {
    printf("%s %d: Syntax error\n", $2.file_name, $2.line);
    $$.file_name = $2.file_name;
    $$.line = $2.line;
    $$.n = cons(.what = nNOTHING);
}  | simple_type decl_name ';' {
    $$ = $2; $$.n->r = list_append($$.n->r, $1.n);
} | simple_type ';' {
    // FIXME: should complain if it's a type with no side-effects
    $$.file_name = $1.file_name;
    $$.line = $1.line;
    $$.n = cons(.what = nNOTHING);
} ;

const_expr
  : tNUM {
    $$ = $1;
} | '(' const_expr ')' {
    $$ = $2;
} | const_expr '+' const_expr {
    $$.file_name = $2.file_name;
    $$.line = $2.line;
    $$.num.num = $1.num.num + $3.num.num;
} | const_expr '-' const_expr {
    $$.file_name = $2.file_name;
    $$.line = $2.line;
    $$.num.num = $1.num.num - $3.num.num;
} | const_expr '*' const_expr {
    $$.file_name = $2.file_name;
    $$.line = $2.line;
    $$.num.num = $1.num.num * $3.num.num;
} | const_expr '/' const_expr {
    $$.file_name = $2.file_name;
    $$.line = $2.line;
    $$.num.num = $1.num.num / $3.num.num;
} | '-' const_expr %prec '*' {
    $$.file_name = $1.file_name;
    $$.line = $1.line;
    $$.num.num = -$2.num.num;
} ;

%%

int yylex()
{
    int t;
    Node *n;
    t = get_tok(0);
    if (t == tWORD && (n = lookup_typedef(word_buffer))) {
        t = tTYPE;
        yylval.n.file_name = file_name;
        yylval.n.line = line;
        yylval.n.n = n;
    } else if (t == tFP) {
        yylval.fp.file_name = file_name;
        yylval.fp.line = line;
        yylval.fp.fp = float_val;
    } else if (t == tNUM) {
        yylval.num.file_name = file_name;
        yylval.num.line = line;
        yylval.num.num = num;
    } else if (t == tWORD || t == tSTRING) {
        yylval.s.file_name = file_name;
        yylval.s.line = line;
        yylval.s.s = strdup(word_buffer);
    } else {
        yylval.term.file_name = file_name;
        yylval.term.line = line;
    }
    return t;
}

extern char *file_name;
extern int line;

void yyerror(char *s)
{
    printf("%s %d: %s\n", file_name, line, s);
}
