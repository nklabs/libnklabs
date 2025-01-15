/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    tEOF = 0,                      /* tEOF  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    tNUM = 258,                    /* tNUM  */
    tSTRING = 259,                 /* tSTRING  */
    tWORD = 260,                   /* tWORD  */
    tFP = 261,                     /* tFP  */
    tAUTO = 262,                   /* tAUTO  */
    tBREAK = 263,                  /* tBREAK  */
    tCASE = 264,                   /* tCASE  */
    tCHAR = 265,                   /* tCHAR  */
    tCONST = 266,                  /* tCONST  */
    tCONTINUE = 267,               /* tCONTINUE  */
    tDEFAULT = 268,                /* tDEFAULT  */
    tDO = 269,                     /* tDO  */
    tDOUBLE = 270,                 /* tDOUBLE  */
    tELSE = 271,                   /* tELSE  */
    tENUM = 272,                   /* tENUM  */
    tEXTERN = 273,                 /* tEXTERN  */
    tFLOAT = 274,                  /* tFLOAT  */
    tFOR = 275,                    /* tFOR  */
    tGOTO = 276,                   /* tGOTO  */
    tIF = 277,                     /* tIF  */
    tINT = 278,                    /* tINT  */
    tLONG = 279,                   /* tLONG  */
    tREGISTER = 280,               /* tREGISTER  */
    tRETURN = 281,                 /* tRETURN  */
    tSHORT = 282,                  /* tSHORT  */
    tSIGNED = 283,                 /* tSIGNED  */
    tSIZEOF = 284,                 /* tSIZEOF  */
    tSTATIC = 285,                 /* tSTATIC  */
    tSTRUCT = 286,                 /* tSTRUCT  */
    tSWITCH = 287,                 /* tSWITCH  */
    tTYPEDEF = 288,                /* tTYPEDEF  */
    tUNION = 289,                  /* tUNION  */
    tUNSIGNED = 290,               /* tUNSIGNED  */
    tVOID = 291,                   /* tVOID  */
    tVOLATILE = 292,               /* tVOLATILE  */
    tWHILE = 293,                  /* tWHILE  */
    tTYPE = 294                    /* tTYPE  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEMPTY -2
#define tEOF 0
#define YYerror 256
#define YYUNDEF 257
#define tNUM 258
#define tSTRING 259
#define tWORD 260
#define tFP 261
#define tAUTO 262
#define tBREAK 263
#define tCASE 264
#define tCHAR 265
#define tCONST 266
#define tCONTINUE 267
#define tDEFAULT 268
#define tDO 269
#define tDOUBLE 270
#define tELSE 271
#define tENUM 272
#define tEXTERN 273
#define tFLOAT 274
#define tFOR 275
#define tGOTO 276
#define tIF 277
#define tINT 278
#define tLONG 279
#define tREGISTER 280
#define tRETURN 281
#define tSHORT 282
#define tSIGNED 283
#define tSIZEOF 284
#define tSTATIC 285
#define tSTRUCT 286
#define tSWITCH 287
#define tTYPEDEF 288
#define tUNION 289
#define tUNSIGNED 290
#define tVOID 291
#define tVOLATILE 292
#define tWHILE 293
#define tTYPE 294

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 40 "gram.y"

    struct llvalterm term;
    struct llvalnum num;
    struct llvalfp fp;
    struct llvals s;
    struct llvaln n;

#line 153 "y.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
