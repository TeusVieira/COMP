/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
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

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_COMPILADOR_TAB_H_INCLUDED
# define YY_YY_COMPILADOR_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    INT_KW = 258,
    REAL_KW = 259,
    STRING_KW = 260,
    ARRAY_KW = 261,
    IF_KW = 262,
    THEN_KW = 263,
    ELSE_KW = 264,
    ENDIF_KW = 265,
    WHILE_KW = 266,
    DO_KW = 267,
    ENDWHILE_KW = 268,
    FOR_KW = 269,
    ENDFOR_KW = 270,
    READ_KW = 271,
    WRITE_KW = 272,
    FUNC_SQRT = 273,
    FUNC_ABS = 274,
    INT_LITERAL = 275,
    REAL_LITERAL = 276,
    STRING_LITERAL = 277,
    ID = 278,
    ADD = 279,
    SUB = 280,
    MUL = 281,
    DIV = 282,
    GT = 283,
    LT = 284,
    NE = 285,
    EQ = 286,
    GE = 287,
    LE = 288,
    OR = 289,
    AND = 290,
    ASSIGN = 291,
    LPAREN = 292,
    RPAREN = 293,
    LBRACK = 294,
    RBRACK = 295,
    SEMICOLON = 296,
    COMMA = 297,
    UMINUS = 298
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 26 "compilador.y"

    int ival;    // Para inteiros (INT_LITERAL)
    double rval;    // Para floats (REAL_LITERAL)
    char *sval;     // Para strings (ID, STRING_LITERAL)
    Ast *node;    // Para nós da AST (agora a maioria das regras retornará isso)

#line 108 "compilador.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_COMPILADOR_TAB_H_INCLUDED  */
