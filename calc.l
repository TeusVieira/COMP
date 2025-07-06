%{
#include "calc.tab.h"
#include "ast.h" 
#include <stdlib.h>
#include <string.h>
%}

%option noyywrap

DIGITO       [0-9]
LETRA        [a-zA-Z_]
ID           {LETRA}({LETRA}|{DIGITO})*

INT_CONST    {DIGITO}+
FLOAT_CONST  {DIGITO}+"."{DIGITO}+
STRING       \"([^\\\"]|\\.)*\"

%%

"INICIO"        return INICIO;
"FIM"           return FIM;

"int"           return KW_INT;
"float"         return KW_FLOAT;
"string"        return KW_STRING;

"receba"        return RECEBA;
"devolva"       return DEVOLVA;

"if"            return KW_IF;
"else"          return KW_ELSE;
"while"         return KW_WHILE;

"=="            return OP_IGUAL;
"!="            return OP_DIFERENTE;
">="            return OP_MAIORIGUAL;
"<="            return OP_MENORIGUAL;
">"             return OP_MAIOR;
"<"             return OP_MENOR;
"="             return OP_ATRIBUICAO;

"+"             return ARI_MAIS;
"-"             return ARI_MENOS;
"*"             return ARI_VEZES;
"/"             return ARI_DIVIDE;

";"             return ';';
","             return ',';
"("             return '(';
")"             return ')';
"["             return '[';
"]"             return ']';

"&&"            return OP_AND;
"||"            return OP_OR;

"{"             return '{';
"}"             return '}';

{FLOAT_CONST}   { yylval.fval = atof(yytext); return FLOAT_CONST; }
{INT_CONST}     { yylval.ival = atoi(yytext); return INT_CONST; }

{STRING} {
    int len = strlen(yytext);
    char *str = (char*) malloc(len - 1);
    strncpy(str, yytext + 1, len - 2);
    str[len - 2] = '\0';
    yylval.sval = str;
    return STRING;
}

{ID}           { yylval.sval = strdup(yytext); return ID; }

[ \t\r\n]+     ;  // Ignora espaços em branco
"//".*         ;  // Ignora comentários de linha

.              { printf("Caractere inválido: %s\n", yytext); }

%%
