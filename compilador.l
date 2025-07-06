%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ast.h" 
#include "compilador.tab.h" 


extern int yylineno; 
%}

%option noyywrap
%option yylineno

%%


"//".* {  }
"/*" {
    int c, d;
    while ( (c = input()) != 0 ) { 
        if (c == '*') {
            if ((d = input()) == '/') { 
                break; 
            } else {
                unput(d); 
            }
        } else if (c == '\n') {
            yylineno++;
        }
    }
    if (c == 0) { 
        fprintf(stderr, "Erro léxico na linha %d: Comentário de bloco não fechado (EOF prematuro).\n", yylineno);
        exit(1);
    }
}

[ \t]+ {  }
\n     {yylineno++;} 


"int"       { return INT_KW; }
"real"      { return REAL_KW; }
"string"    { return STRING_KW; }
"array"     { return ARRAY_KW; }
"se"        { return IF_KW; }
"então"     { return THEN_KW; }
"senão"     { return ELSE_KW; }
"fimse"     { return ENDIF_KW; }
"enquanto"  { return WHILE_KW; }
"faça"      { return DO_KW; }
"fimenquanto" { return ENDWHILE_KW; }
"para"      { return FOR_KW; }
"fimpara"   { return ENDFOR_KW; }
"ler"       { return READ_KW; }
"escrever"  { return WRITE_KW; }
"raiz"      { return FUNC_SQRT; }
"abs"       { return FUNC_ABS; }


[a-zA-Z_][a-zA-Z0-9_]* { yylval.sval = strdup(yytext); return ID; }


[0-9]+"."[0-9]*([Ee][+-]?[0-9]+)? { yylval.rval = atof(yytext); return REAL_LITERAL; }
[0-9]+    { yylval.ival = atoi(yytext); return INT_LITERAL; }


\"(\\.|[^"\\])*\" {
    char *raw_string = yytext + 1; // Pula a aspa inicial
    size_t raw_len = yyleng - 2;   // Ignora as aspas
    char *processed_string = (char *)malloc(raw_len + 1); // Pode ser menor, mas nunca maior
    if (!processed_string) {
        fprintf(stderr, "Erro de memória ao processar string literal.\n");
        exit(1);
    }

    size_t i = 0; // Índice para raw_string
    size_t j = 0; // Índice para processed_string

    while (i < raw_len) {
        if (raw_string[i] == '\\') {
            i++; // Pula a barra invertida
            if (i >= raw_len) {
                // Erro: barra invertida no final da string
                fprintf(stderr, "Erro no lexer: barra invertida inesperada no final da string literal.\n");
                exit(1);
            }
            switch (raw_string[i]) {
                case 'n':
                    processed_string[j++] = '\n';
                    break;
                case 't':
                    processed_string[j++] = '\t';
                    break;
                case '\\':
                    processed_string[j++] = '\\';
                    break;
                case '"':
                    processed_string[j++] = '"';
                    break;
                // Adicione outros caracteres de escape conforme necessário, por exemplo:
                // case 'r': processed_string[j++] = '\r'; break;
                // case 'f': processed_string[j++] = '\f'; break;
                // case 'b': processed_string[j++] = '\b'; break;
                default:
                    // Caractere de escape inválido, ou você quer manter o \ literal
                    // Para o seu caso, se for '\a', queremos que imprima 'a'
                    // Se quiser gerar erro para escapes desconhecidos:
                    fprintf(stderr, "Erro no lexer: sequencia de escape invalida '\\%c'.\n", raw_string[i]);
                    exit(1);
                    // Ou, para ignorar o escape e manter o caractere literal:
                    // processed_string[j++] = raw_string[i];
                    // break;
            }
        } else {
            processed_string[j++] = raw_string[i];
        }
        i++;
    }
    processed_string[j] = '\0'; // Termina a string

    yylval.sval = processed_string; // Atribui a string processada ao yylval
    return STRING_LITERAL;
}

"+"       { return ADD; }
"-"       { return SUB; }
"*"       { return MUL; }
"/"       { return DIV; }


">"       { return GT; }
"<"       { return LT; }
"!="      { return NE; }
"=="      { return EQ; }
">="      { return GE; }
"<="      { return LE; }
"||"      { return OR; }
"&&"      { return AND; }


"="       { return ASSIGN; }
"("       { return LPAREN; }
")"       { return RPAREN; }
"["       { return LBRACK; }
"]"       { return RBRACK; }
";"       { return SEMICOLON; }
","       { return COMMA; }


.         { fprintf(stderr, "Erro léxico na linha %d: Caractere inesperado '%s'\n", yylineno, yytext); exit(1); }

%%

