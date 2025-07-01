%{
#include "calc.tab.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
%}

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

"+"             return '+';
"-"             return '-';
"*"             return '*';
"/"             return '/';

";"             return ';';
","             return ',';
"("             return '(';
")"             return ')';
"["             return '[';
"]"             return ']';

{FLOAT_CONST}   { yylval.fval = atof(yytext); return FLOAT_CONST; }
{INT_CONST}     { yylval.ival = atoi(yytext); return INT_CONST; }

{STRING} {
    int len = strlen(yytext);
    char *str = (char*) malloc(len - 1);  // sem aspas
    if (!str) {
        fprintf(stderr, "Erro de memória\n");
        exit(1);
    }
    strncpy(str, yytext + 1, len - 2); // copia sem aspas
    str[len - 2] = '\0';  // adiciona '\0' no final
    yylval.sval = str;
    return STRING;
}

{ID}           { yylval.sval = strdup(yytext); return ID; }

[ \t\r\n]+     ;  // ignora espaços em branco
"//".*         ;  // ignora comentários de linha

.              { printf("Caractere inválido: %s\n", yytext); }

%%
