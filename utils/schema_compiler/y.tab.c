/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 25 "gram.y"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lex.h"
#include "tree.h"

int yylex();
void yyerror(char *s);

int anon_count = 0;
int show_tree = 0;


#line 86 "y.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
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

#line 225 "y.tab.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* tEOF  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_tNUM = 3,                       /* tNUM  */
  YYSYMBOL_tSTRING = 4,                    /* tSTRING  */
  YYSYMBOL_tWORD = 5,                      /* tWORD  */
  YYSYMBOL_tFP = 6,                        /* tFP  */
  YYSYMBOL_tAUTO = 7,                      /* tAUTO  */
  YYSYMBOL_tBREAK = 8,                     /* tBREAK  */
  YYSYMBOL_tCASE = 9,                      /* tCASE  */
  YYSYMBOL_tCHAR = 10,                     /* tCHAR  */
  YYSYMBOL_tCONST = 11,                    /* tCONST  */
  YYSYMBOL_tCONTINUE = 12,                 /* tCONTINUE  */
  YYSYMBOL_tDEFAULT = 13,                  /* tDEFAULT  */
  YYSYMBOL_tDO = 14,                       /* tDO  */
  YYSYMBOL_tDOUBLE = 15,                   /* tDOUBLE  */
  YYSYMBOL_tELSE = 16,                     /* tELSE  */
  YYSYMBOL_tENUM = 17,                     /* tENUM  */
  YYSYMBOL_tEXTERN = 18,                   /* tEXTERN  */
  YYSYMBOL_tFLOAT = 19,                    /* tFLOAT  */
  YYSYMBOL_tFOR = 20,                      /* tFOR  */
  YYSYMBOL_tGOTO = 21,                     /* tGOTO  */
  YYSYMBOL_tIF = 22,                       /* tIF  */
  YYSYMBOL_tINT = 23,                      /* tINT  */
  YYSYMBOL_tLONG = 24,                     /* tLONG  */
  YYSYMBOL_tREGISTER = 25,                 /* tREGISTER  */
  YYSYMBOL_tRETURN = 26,                   /* tRETURN  */
  YYSYMBOL_tSHORT = 27,                    /* tSHORT  */
  YYSYMBOL_tSIGNED = 28,                   /* tSIGNED  */
  YYSYMBOL_tSIZEOF = 29,                   /* tSIZEOF  */
  YYSYMBOL_tSTATIC = 30,                   /* tSTATIC  */
  YYSYMBOL_tSTRUCT = 31,                   /* tSTRUCT  */
  YYSYMBOL_tSWITCH = 32,                   /* tSWITCH  */
  YYSYMBOL_tTYPEDEF = 33,                  /* tTYPEDEF  */
  YYSYMBOL_tUNION = 34,                    /* tUNION  */
  YYSYMBOL_tUNSIGNED = 35,                 /* tUNSIGNED  */
  YYSYMBOL_tVOID = 36,                     /* tVOID  */
  YYSYMBOL_tVOLATILE = 37,                 /* tVOLATILE  */
  YYSYMBOL_tWHILE = 38,                    /* tWHILE  */
  YYSYMBOL_tTYPE = 39,                     /* tTYPE  */
  YYSYMBOL_40_ = 40,                       /* '-'  */
  YYSYMBOL_41_ = 41,                       /* '+'  */
  YYSYMBOL_42_ = 42,                       /* '*'  */
  YYSYMBOL_43_ = 43,                       /* '/'  */
  YYSYMBOL_44_ = 44,                       /* '['  */
  YYSYMBOL_45_ = 45,                       /* ';'  */
  YYSYMBOL_46_ = 46,                       /* '{'  */
  YYSYMBOL_47_ = 47,                       /* '}'  */
  YYSYMBOL_48_ = 48,                       /* '('  */
  YYSYMBOL_49_ = 49,                       /* ')'  */
  YYSYMBOL_50_ = 50,                       /* ']'  */
  YYSYMBOL_YYACCEPT = 51,                  /* $accept  */
  YYSYMBOL_top = 52,                       /* top  */
  YYSYMBOL_input = 53,                     /* input  */
  YYSYMBOL_inside = 54,                    /* inside  */
  YYSYMBOL_simple_type = 55,               /* simple_type  */
  YYSYMBOL_decl_name = 56,                 /* decl_name  */
  YYSYMBOL_decl = 57,                      /* decl  */
  YYSYMBOL_const_expr = 58                 /* const_expr  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   210

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  51
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  8
/* YYNRULES -- Number of rules.  */
#define YYNRULES  43
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  71

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   294


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      48,    49,    42,    41,     2,    40,     2,    43,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    45,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    44,     2,    50,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    46,     2,    47,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    75,    75,    78,    82,    86,    96,   100,   110,   114,
     118,   122,   126,   130,   134,   138,   142,   146,   150,   154,
     158,   162,   166,   170,   181,   186,   208,   216,   253,   257,
     262,   266,   268,   271,   277,   282,   284,   292,   294,   296,
     300,   304,   308,   312
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "tEOF", "error", "\"invalid token\"", "tNUM", "tSTRING", "tWORD", "tFP",
  "tAUTO", "tBREAK", "tCASE", "tCHAR", "tCONST", "tCONTINUE", "tDEFAULT",
  "tDO", "tDOUBLE", "tELSE", "tENUM", "tEXTERN", "tFLOAT", "tFOR", "tGOTO",
  "tIF", "tINT", "tLONG", "tREGISTER", "tRETURN", "tSHORT", "tSIGNED",
  "tSIZEOF", "tSTATIC", "tSTRUCT", "tSWITCH", "tTYPEDEF", "tUNION",
  "tUNSIGNED", "tVOID", "tVOLATILE", "tWHILE", "tTYPE", "'-'", "'+'",
  "'*'", "'/'", "'['", "';'", "'{'", "'}'", "'('", "')'", "']'", "$accept",
  "top", "input", "inside", "simple_type", "decl_name", "decl",
  "const_expr", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-44)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-3)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     -44,    20,   132,   -44,   -41,   -44,   -44,   -44,   -44,   -44,
     -44,     1,    -3,   160,    -2,   125,   -44,   -44,    76,   -44,
     -44,   -44,   -44,   -44,   -44,   -38,   -44,   -44,   -17,   -44,
     -44,   -44,   -44,   -44,   -44,    31,   -44,    31,   -32,   -44,
      -1,   -44,    30,   -12,   -43,     2,   -44,    61,   -44,   -44,
      92,   -44,   -44,   -44,     2,     2,    63,   -44,   -44,   -44,
     161,     2,     2,     2,     2,   -44,   -44,   -27,   -27,   -44,
     -44
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       3,     0,     0,     1,     0,     9,    22,    21,    15,    18,
      12,     0,     0,     0,     0,     0,     8,    29,     0,     5,
      34,    10,    16,    19,    13,    23,     6,     4,    24,     6,
      11,    17,    20,    14,    30,     0,    36,     0,     0,     6,
       0,     6,     0,    32,     0,     0,    35,     0,    27,     7,
       0,    28,    31,    37,     0,     0,     0,    25,    26,    43,
       0,     0,     0,     0,     0,    33,    38,    40,    39,    41,
      42
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -44,   -44,   -44,   -22,   -44,     4,     8,    82
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     1,     2,    40,    18,    38,    49,    56
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
       4,    45,    25,    28,    20,    53,    52,    42,    39,     5,
      19,    21,    45,    46,     6,    63,    64,    47,     7,    50,
       3,    27,     8,     9,    22,    23,    10,    11,    24,    41,
      12,     4,    45,    14,    15,    16,    34,     0,    17,    43,
       5,    44,    54,    26,    29,     6,    48,     0,     0,     7,
      55,     0,     0,     8,     9,     0,     0,    10,    11,     0,
       0,    12,     4,     0,    14,    15,    16,     0,     0,    17,
       0,     5,     0,    35,     0,     0,     6,    51,     0,    37,
       7,    34,     0,     0,     8,     9,     0,     0,    10,    11,
       0,     0,    12,     4,     0,    14,    15,    16,     0,     0,
      17,     0,     5,    61,    62,    63,    64,     6,    57,     0,
       0,     7,     0,    65,     0,     8,     9,     0,    35,    10,
      11,    36,     0,    12,    37,     0,    14,    15,    16,     0,
       0,    17,    -2,     4,     0,    30,    59,    60,     0,    58,
       0,     0,     5,    67,    68,    69,    70,     6,    31,    32,
       0,     7,    33,     0,     0,     8,     9,     0,     0,    10,
      11,     4,     0,    12,     0,    13,    14,    15,    16,     0,
       5,    17,     0,     0,     0,     6,     0,     0,     0,     7,
       0,     0,     0,     8,     9,     0,     0,    10,    11,     0,
       0,    12,     0,     0,    14,    15,    16,     0,     0,    17,
       0,    61,    62,    63,    64,     0,     0,     0,     0,     0,
      66
};

static const yytype_int8 yycheck[] =
{
       1,    44,     5,     5,    45,     3,    49,    29,    46,    10,
       2,    10,    44,    45,    15,    42,    43,    39,    19,    41,
       0,    13,    23,    24,    23,    24,    27,    28,    27,    46,
      31,     1,    44,    34,    35,    36,     5,    -1,    39,    35,
      10,    37,    40,    46,    46,    15,    47,    -1,    -1,    19,
      48,    -1,    -1,    23,    24,    -1,    -1,    27,    28,    -1,
      -1,    31,     1,    -1,    34,    35,    36,    -1,    -1,    39,
      -1,    10,    -1,    42,    -1,    -1,    15,    47,    -1,    48,
      19,     5,    -1,    -1,    23,    24,    -1,    -1,    27,    28,
      -1,    -1,    31,     1,    -1,    34,    35,    36,    -1,    -1,
      39,    -1,    10,    40,    41,    42,    43,    15,    47,    -1,
      -1,    19,    -1,    50,    -1,    23,    24,    -1,    42,    27,
      28,    45,    -1,    31,    48,    -1,    34,    35,    36,    -1,
      -1,    39,     0,     1,    -1,    10,    54,    55,    -1,    47,
      -1,    -1,    10,    61,    62,    63,    64,    15,    23,    24,
      -1,    19,    27,    -1,    -1,    23,    24,    -1,    -1,    27,
      28,     1,    -1,    31,    -1,    33,    34,    35,    36,    -1,
      10,    39,    -1,    -1,    -1,    15,    -1,    -1,    -1,    19,
      -1,    -1,    -1,    23,    24,    -1,    -1,    27,    28,    -1,
      -1,    31,    -1,    -1,    34,    35,    36,    -1,    -1,    39,
      -1,    40,    41,    42,    43,    -1,    -1,    -1,    -1,    -1,
      49
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    52,    53,     0,     1,    10,    15,    19,    23,    24,
      27,    28,    31,    33,    34,    35,    36,    39,    55,    57,
      45,    10,    23,    24,    27,     5,    46,    57,     5,    46,
      10,    23,    24,    27,     5,    42,    45,    48,    56,    46,
      54,    46,    54,    56,    56,    44,    45,    54,    47,    57,
      54,    47,    49,     3,    40,    48,    58,    47,    47,    58,
      58,    40,    41,    42,    43,    50,    49,    58,    58,    58,
      58
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    51,    52,    53,    53,    53,    54,    54,    55,    55,
      55,    55,    55,    55,    55,    55,    55,    55,    55,    55,
      55,    55,    55,    55,    55,    55,    55,    55,    55,    55,
      56,    56,    56,    56,    57,    57,    57,    58,    58,    58,
      58,    58,    58,    58
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     0,     3,     2,     0,     2,     1,     1,
       2,     2,     1,     2,     2,     1,     2,     2,     1,     2,
       2,     1,     1,     2,     2,     5,     5,     4,     4,     1,
       1,     3,     2,     4,     2,     3,     2,     1,     3,     3,
       3,     3,     3,     2
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= tEOF)
    {
      yychar = tEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* top: input  */
#line 75 "gram.y"
            { if (show_tree) { print_tree((yyvsp[0].n).n); printf("\n"); } }
#line 1334 "y.tab.c"
    break;

  case 3: /* input: %empty  */
#line 78 "gram.y"
           {
    (yyval.n).file_name = 0;
    (yyval.n).line = 0;
    (yyval.n).n = (void *)0;
}
#line 1344 "y.tab.c"
    break;

  case 4: /* input: input tTYPEDEF decl  */
#line 82 "gram.y"
                        {
    (yyval.n) = (yyvsp[-2].n);
    if ((yyvsp[0].n).n->s)
      add_typedef((yyvsp[0].n).n->s, (yyvsp[0].n).n->r);
}
#line 1354 "y.tab.c"
    break;

  case 5: /* input: input decl  */
#line 86 "gram.y"
               {
    if ((yyvsp[-1].n).n) {
        (yyval.n).file_name = (yyvsp[-1].n).file_name;
        (yyval.n).line = (yyvsp[-1].n).line;
        (yyval.n).n = cons(.what = nLIST, .l = (yyvsp[-1].n).n, .r = (yyvsp[0].n).n);
    } else
        (yyval.n) = (yyvsp[0].n);
}
#line 1367 "y.tab.c"
    break;

  case 6: /* inside: %empty  */
#line 96 "gram.y"
           {
    (yyval.n).file_name = 0;
    (yyval.n).line = 0;
    (yyval.n).n = (void *)0;
}
#line 1377 "y.tab.c"
    break;

  case 7: /* inside: inside decl  */
#line 100 "gram.y"
                {
    if ((yyvsp[-1].n).n) {
        (yyval.n).file_name = (yyvsp[-1].n).file_name;
        (yyval.n).line = (yyvsp[-1].n).line;
        (yyval.n).n = cons(.what = nLIST, .l = (yyvsp[-1].n).n, .r = (yyvsp[0].n).n);
    } else
        (yyval.n) = (yyvsp[0].n);
}
#line 1390 "y.tab.c"
    break;

  case 8: /* simple_type: tVOID  */
#line 110 "gram.y"
          {
    (yyval.n).file_name = (yyvsp[0].term).file_name;
    (yyval.n).line = (yyvsp[0].term).line;
    (yyval.n).n = cons(.what = ntVOID);
}
#line 1400 "y.tab.c"
    break;

  case 9: /* simple_type: tCHAR  */
#line 114 "gram.y"
          {
    (yyval.n).file_name = (yyvsp[0].term).file_name;
    (yyval.n).line = (yyvsp[0].term).line;
    (yyval.n).n = cons(.what = ntCHAR);
}
#line 1410 "y.tab.c"
    break;

  case 10: /* simple_type: tSIGNED tCHAR  */
#line 118 "gram.y"
                  {
    (yyval.n).file_name = (yyvsp[-1].term).file_name;
    (yyval.n).line = (yyvsp[-1].term).line;
    (yyval.n).n = cons(.what = ntCHAR);
}
#line 1420 "y.tab.c"
    break;

  case 11: /* simple_type: tUNSIGNED tCHAR  */
#line 122 "gram.y"
                    {
    (yyval.n).file_name = (yyvsp[-1].term).file_name;
    (yyval.n).line = (yyvsp[-1].term).line;
    (yyval.n).n = cons(.what = ntUCHAR);
}
#line 1430 "y.tab.c"
    break;

  case 12: /* simple_type: tSHORT  */
#line 126 "gram.y"
           {
    (yyval.n).file_name = (yyvsp[0].term).file_name;
    (yyval.n).line = (yyvsp[0].term).line;
    (yyval.n).n = cons(.what = ntSHORT);
}
#line 1440 "y.tab.c"
    break;

  case 13: /* simple_type: tSIGNED tSHORT  */
#line 130 "gram.y"
                   {
    (yyval.n).file_name = (yyvsp[-1].term).file_name;
    (yyval.n).line = (yyvsp[-1].term).line;
    (yyval.n).n = cons(.what = ntSHORT);
}
#line 1450 "y.tab.c"
    break;

  case 14: /* simple_type: tUNSIGNED tSHORT  */
#line 134 "gram.y"
                     {
    (yyval.n).file_name = (yyvsp[-1].term).file_name;
    (yyval.n).line = (yyvsp[-1].term).line;
    (yyval.n).n = cons(.what = ntUSHORT);
}
#line 1460 "y.tab.c"
    break;

  case 15: /* simple_type: tINT  */
#line 138 "gram.y"
         {
    (yyval.n).file_name = (yyvsp[0].term).file_name;
    (yyval.n).line = (yyvsp[0].term).line;
    (yyval.n).n = cons(.what = ntLONG);
}
#line 1470 "y.tab.c"
    break;

  case 16: /* simple_type: tSIGNED tINT  */
#line 142 "gram.y"
                 {
    (yyval.n).file_name = (yyvsp[-1].term).file_name;
    (yyval.n).line = (yyvsp[-1].term).line;
    (yyval.n).n = cons(.what = ntLONG);
}
#line 1480 "y.tab.c"
    break;

  case 17: /* simple_type: tUNSIGNED tINT  */
#line 146 "gram.y"
                   {
    (yyval.n).file_name = (yyvsp[-1].term).file_name;
    (yyval.n).line = (yyvsp[-1].term).line;
    (yyval.n).n = cons(.what = ntULONG);
}
#line 1490 "y.tab.c"
    break;

  case 18: /* simple_type: tLONG  */
#line 150 "gram.y"
          {
    (yyval.n).file_name = (yyvsp[0].term).file_name;
    (yyval.n).line = (yyvsp[0].term).line;
    (yyval.n).n = cons(.what = ntLONG);
}
#line 1500 "y.tab.c"
    break;

  case 19: /* simple_type: tSIGNED tLONG  */
#line 154 "gram.y"
                  {
    (yyval.n).file_name = (yyvsp[-1].term).file_name;
    (yyval.n).line = (yyvsp[-1].term).line;
    (yyval.n).n = cons(.what = ntLONG);
}
#line 1510 "y.tab.c"
    break;

  case 20: /* simple_type: tUNSIGNED tLONG  */
#line 158 "gram.y"
                    {
    (yyval.n).file_name = (yyvsp[-1].term).file_name;
    (yyval.n).line = (yyvsp[-1].term).line;
    (yyval.n).n = cons(.what = ntULONG);
}
#line 1520 "y.tab.c"
    break;

  case 21: /* simple_type: tFLOAT  */
#line 162 "gram.y"
           {
    (yyval.n).file_name = (yyvsp[0].term).file_name;
    (yyval.n).line = (yyvsp[0].term).line;
    (yyval.n).n = cons(.what = ntFLOAT);
}
#line 1530 "y.tab.c"
    break;

  case 22: /* simple_type: tDOUBLE  */
#line 166 "gram.y"
            {
    (yyval.n).file_name = (yyvsp[0].term).file_name;
    (yyval.n).line = (yyvsp[0].term).line;
    (yyval.n).n = cons(.what = ntDOUBLE);
}
#line 1540 "y.tab.c"
    break;

  case 23: /* simple_type: tSTRUCT tWORD  */
#line 170 "gram.y"
                  {
    struct structbase *u;
    u = lookup_struct((yyvsp[0].s).s);
    if (!u)
    {
      u = add_struct((yyvsp[0].s).s, NULL);
    }
    // printf("Find struct %s found %p\n", $2, u);
    (yyval.n).file_name = (yyvsp[-1].term).file_name;
    (yyval.n).line = (yyvsp[-1].term).line;
    (yyval.n).n = cons(.what = ntSTRUCT, .s = (yyvsp[0].s).s, .uct = u);
}
#line 1557 "y.tab.c"
    break;

  case 24: /* simple_type: tUNION tWORD  */
#line 181 "gram.y"
                 {
    Node *ty = lookup_union((yyvsp[0].s).s);
    (yyval.n).file_name = (yyvsp[-1].term).file_name;
    (yyval.n).line = (yyvsp[-1].term).line;
    (yyval.n).n = cons(.what = ntUNION, .s = (yyvsp[0].s).s, .r = ty);
}
#line 1568 "y.tab.c"
    break;

  case 25: /* simple_type: tSTRUCT tWORD '{' inside '}'  */
#line 186 "gram.y"
                                 {
    struct structbase *u;
    (yyval.n).file_name = (yyvsp[-4].term).file_name;
    (yyval.n).line = (yyvsp[-4].term).line;

    u = lookup_struct((yyvsp[-3].s).s);
    if (u)
    {
      if (u->body)
        printf("duplicate struct definition\n");
      else
      {
        // Fill in forward reference..
        u->body = (yyvsp[-1].n).n;
      }
    }
    else
    {
        u = add_struct((yyvsp[-3].s).s, (yyvsp[-1].n).n);
    }

    (yyval.n).n = cons(.what = ntSTRUCT, .s = (yyvsp[-3].s).s, .uct = u);
}
#line 1596 "y.tab.c"
    break;

  case 26: /* simple_type: tUNION tWORD '{' inside '}'  */
#line 208 "gram.y"
                                {
    (yyval.n).file_name = (yyvsp[-4].term).file_name;
    (yyval.n).line = (yyvsp[-4].term).line;
    (yyval.n).n = cons(.what = ntUNION, .s = (yyvsp[-3].s).s, .r = (yyvsp[-1].n).n);
    if (add_union((yyval.n).n->s, (yyval.n).n->r))
    {
        printf("duplicate union definition\n");
    }
}
#line 1610 "y.tab.c"
    break;

  case 27: /* simple_type: tSTRUCT '{' inside '}'  */
#line 216 "gram.y"
                           {
    // Anonymous structure...
    // Make up a name for it
    char anon_name[80];
    char *aname;
    sprintf(anon_name, "anon_%d", ++anon_count);
    aname = strdup(anon_name);

    struct structbase *u;
    (yyval.n).file_name = (yyvsp[-3].term).file_name;
    (yyval.n).line = (yyvsp[-3].term).line;

    u = lookup_struct(aname);
    if (u)
    {
      if (u->body)
        printf("duplicate struct definition\n");
      else
      {
        // Fill in forward reference..
        u->body = (yyvsp[-1].n).n;
      }
    }
    else
    {
        u = add_struct(aname, (yyvsp[-1].n).n);
    }

    (yyval.n).n = cons(.what = ntSTRUCT, .s = aname, .uct = u);

#if 0
    struct structbase *u = (struct structbase *)calloc(1, sizeof(struct structbase));
    u->body = (yyvsp[-1].n).n;
    (yyval.n).file_name = (yyvsp[-3].term).file_name;
    (yyval.n).line = (yyvsp[-3].term).line;
    (yyval.n).n = cons(.what = ntSTRUCT, .s = NULL, .uct = u);
#endif
}
#line 1653 "y.tab.c"
    break;

  case 28: /* simple_type: tUNION '{' inside '}'  */
#line 253 "gram.y"
                          {
    (yyval.n).file_name = (yyvsp[-3].term).file_name;
    (yyval.n).line = (yyvsp[-3].term).line;
    (yyval.n).n = cons(.what = ntUNION, .s = NULL, .r = (yyvsp[-1].n).n);
}
#line 1663 "y.tab.c"
    break;

  case 29: /* simple_type: tTYPE  */
#line 257 "gram.y"
          {
    (yyval.n) = (yyvsp[0].n);
}
#line 1671 "y.tab.c"
    break;

  case 30: /* decl_name: tWORD  */
#line 262 "gram.y"
         {
   (yyval.n).file_name = (yyvsp[0].s).file_name;
   (yyval.n).line = (yyvsp[0].s).line;
   (yyval.n).n = cons(.what = nDECL, .s = (yyvsp[0].s).s, .r = NULL);
}
#line 1681 "y.tab.c"
    break;

  case 31: /* decl_name: '(' decl_name ')'  */
#line 266 "gram.y"
                      {
    (yyval.n) = (yyvsp[-1].n);
}
#line 1689 "y.tab.c"
    break;

  case 32: /* decl_name: '*' decl_name  */
#line 268 "gram.y"
                  {
    (yyval.n) = (yyvsp[0].n);
    (yyval.n).n->r = list_append((yyval.n).n->r, cons(.what = ntPTR, .r = NULL));
}
#line 1698 "y.tab.c"
    break;

  case 33: /* decl_name: decl_name '[' const_expr ']'  */
#line 271 "gram.y"
                                 {
    (yyval.n) = (yyvsp[-3].n);
    (yyval.n).n->r = list_append((yyval.n).n->r, cons(.what = ntARRAY, .siz = (yyvsp[-1].num).num.num, .r = NULL));
}
#line 1707 "y.tab.c"
    break;

  case 34: /* decl: error ';'  */
#line 277 "gram.y"
              {
    printf("%s %d: Syntax error\n", (yyvsp[0].term).file_name, (yyvsp[0].term).line);
    (yyval.n).file_name = (yyvsp[0].term).file_name;
    (yyval.n).line = (yyvsp[0].term).line;
    (yyval.n).n = cons(.what = nNOTHING);
}
#line 1718 "y.tab.c"
    break;

  case 35: /* decl: simple_type decl_name ';'  */
#line 282 "gram.y"
                               {
    (yyval.n) = (yyvsp[-1].n); (yyval.n).n->r = list_append((yyval.n).n->r, (yyvsp[-2].n).n);
}
#line 1726 "y.tab.c"
    break;

  case 36: /* decl: simple_type ';'  */
#line 284 "gram.y"
                    {
    // FIXME: should complain if it's a type with no side-effects
    (yyval.n).file_name = (yyvsp[-1].n).file_name;
    (yyval.n).line = (yyvsp[-1].n).line;
    (yyval.n).n = cons(.what = nNOTHING);
}
#line 1737 "y.tab.c"
    break;

  case 37: /* const_expr: tNUM  */
#line 292 "gram.y"
         {
    (yyval.num) = (yyvsp[0].num);
}
#line 1745 "y.tab.c"
    break;

  case 38: /* const_expr: '(' const_expr ')'  */
#line 294 "gram.y"
                       {
    (yyval.num) = (yyvsp[-1].num);
}
#line 1753 "y.tab.c"
    break;

  case 39: /* const_expr: const_expr '+' const_expr  */
#line 296 "gram.y"
                              {
    (yyval.num).file_name = (yyvsp[-1].term).file_name;
    (yyval.num).line = (yyvsp[-1].term).line;
    (yyval.num).num.num = (yyvsp[-2].num).num.num + (yyvsp[0].num).num.num;
}
#line 1763 "y.tab.c"
    break;

  case 40: /* const_expr: const_expr '-' const_expr  */
#line 300 "gram.y"
                              {
    (yyval.num).file_name = (yyvsp[-1].term).file_name;
    (yyval.num).line = (yyvsp[-1].term).line;
    (yyval.num).num.num = (yyvsp[-2].num).num.num - (yyvsp[0].num).num.num;
}
#line 1773 "y.tab.c"
    break;

  case 41: /* const_expr: const_expr '*' const_expr  */
#line 304 "gram.y"
                              {
    (yyval.num).file_name = (yyvsp[-1].term).file_name;
    (yyval.num).line = (yyvsp[-1].term).line;
    (yyval.num).num.num = (yyvsp[-2].num).num.num * (yyvsp[0].num).num.num;
}
#line 1783 "y.tab.c"
    break;

  case 42: /* const_expr: const_expr '/' const_expr  */
#line 308 "gram.y"
                              {
    (yyval.num).file_name = (yyvsp[-1].term).file_name;
    (yyval.num).line = (yyvsp[-1].term).line;
    (yyval.num).num.num = (yyvsp[-2].num).num.num / (yyvsp[0].num).num.num;
}
#line 1793 "y.tab.c"
    break;

  case 43: /* const_expr: '-' const_expr  */
#line 312 "gram.y"
                             {
    (yyval.num).file_name = (yyvsp[-1].term).file_name;
    (yyval.num).line = (yyvsp[-1].term).line;
    (yyval.num).num.num = -(yyvsp[0].num).num.num;
}
#line 1803 "y.tab.c"
    break;


#line 1807 "y.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= tEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == tEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 318 "gram.y"


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

void yyerror(char *s)
{
    printf("%s\n", s);
}
