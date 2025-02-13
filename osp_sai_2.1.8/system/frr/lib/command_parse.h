/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

#ifndef YY_CMD_YY_COMMAND_PARSE_H_INCLUDED
# define YY_CMD_YY_COMMAND_PARSE_H_INCLUDED
/* Debug traces.  */
#ifndef CMD_YYDEBUG
# if defined YYDEBUG
#if YYDEBUG
#   define CMD_YYDEBUG 1
#  else
#   define CMD_YYDEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define CMD_YYDEBUG 0
# endif /* ! defined YYDEBUG */
#endif  /* ! defined CMD_YYDEBUG */
#if CMD_YYDEBUG
extern int cmd_yydebug;
#endif
/* "%code requires" blocks.  */
#line 46 "command_parse.y" /* yacc.c:1909  */

  #include "stdlib.h"
  #include "string.h"
  #include "memory.h"
  #include "command.h"
  #include "log.h"
  #include "graph.h"

  DECLARE_MTYPE(LEX)

  #define YYSTYPE CMD_YYSTYPE
  #define YYLTYPE CMD_YYLTYPE
  struct parser_ctx;

  /* subgraph semantic value */
  struct subgraph {
    struct graph_node *start, *end;
  };

#line 72 "command_parse.h" /* yacc.c:1909  */

/* Token type.  */
#ifndef CMD_YYTOKENTYPE
# define CMD_YYTOKENTYPE
  enum cmd_yytokentype
  {
    WORD = 258,
    IPV4 = 259,
    IPV4_PREFIX = 260,
    IPV6 = 261,
    IPV6_PREFIX = 262,
    VARIABLE = 263,
    RANGE = 264
  };
#endif
/* Tokens.  */
#define WORD 258
#define IPV4 259
#define IPV4_PREFIX 260
#define IPV6 261
#define IPV6_PREFIX 262
#define VARIABLE 263
#define RANGE 264

/* Value type.  */
#if ! defined CMD_YYSTYPE && ! defined CMD_YYSTYPE_IS_DECLARED
typedef union CMD_YYSTYPE CMD_YYSTYPE;
union CMD_YYSTYPE
{
#line 66 "command_parse.y" /* yacc.c:1909  */

  long long number;
  char *string;
  struct graph_node *node;
  struct subgraph subgraph;

#line 109 "command_parse.h" /* yacc.c:1909  */
};
# define CMD_YYSTYPE_IS_TRIVIAL 1
# define CMD_YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined CMD_YYLTYPE && ! defined CMD_YYLTYPE_IS_DECLARED
typedef struct CMD_YYLTYPE CMD_YYLTYPE;
struct CMD_YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define CMD_YYLTYPE_IS_DECLARED 1
# define CMD_YYLTYPE_IS_TRIVIAL 1
#endif



int cmd_yyparse (struct parser_ctx *ctx);
/* "%code provides" blocks.  */
#line 73 "command_parse.y" /* yacc.c:1909  */

  #ifndef FLEX_SCANNER
  #include "command_lex.h"
  #endif

  extern void set_lexer_string (yyscan_t *scn, const char *string);
  extern void cleanup_lexer (yyscan_t *scn);

  struct parser_ctx {
    yyscan_t scanner;

    struct cmd_element *el;

    struct graph *graph;
    struct graph_node *currnode;

    /* pointers to copy of command docstring */
    char *docstr_start, *docstr;
  };

#line 154 "command_parse.h" /* yacc.c:1909  */

#endif /* !YY_CMD_YY_COMMAND_PARSE_H_INCLUDED  */
