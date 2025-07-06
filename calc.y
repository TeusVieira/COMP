%debug
%{
#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void yyerror(const char *s);
int yylex(void);

typedef struct Variavel {
    char *nome;
    int tipo;
    int tamanho;
    union {
        int *vi;
        float *vf;
        char **vs;
    } valor;
} Variavel;

#define MAX_VARS 100
Variavel tabela_vars[MAX_VARS];
int num_vars = 0;

int procura_var(char *nome);
void adiciona_var(char *nome, int tipo, int tamanho);
Variavel* pega_var(char *nome);
void imprime_var(Variavel *v, int pos);
%}

%union {
    int ival;
    float fval;
    char *sval;
    Ast *ast;
}

%token <ival> INT_CONST
%token <fval> FLOAT_CONST
%token <sval> ID STRING

%token INICIO FIM
%token KW_INT KW_FLOAT KW_STRING
%token RECEBA DEVOLVA
%token KW_IF KW_ELSE KW_WHILE

%token OP_IGUAL OP_DIFERENTE OP_MAIORIGUAL OP_MENORIGUAL OP_MAIOR OP_MENOR OP_ATRIBUICAO
%token OP_AND OP_OR
%token ARI_MAIS ARI_MENOS ARI_VEZES ARI_DIVIDE

%left OP_OR
%left OP_AND
%left OP_IGUAL OP_DIFERENTE
%left OP_MAIOR OP_MENOR OP_MAIORIGUAL OP_MENORIGUAL
%left ARI_MAIS ARI_MENOS
%left ARI_VEZES ARI_DIVIDE

%type <ast> programa bloco linha declaracao comando condicional repeticao expressao
%type <sval> lista_de_impressao item_impressao

%%

programa:
    INICIO bloco FIM { printf("Programa finalizado.\n"); }
;

bloco:
    '{' lista_linhas '}' { $$ = ast_novo_bloco($2, NULL); }
;

lista_linhas:
    lista_linhas linha { $$ = ast_append($1, $2); }
  | linha { $$ = $1; }
;

linha:
    declaracao
  | comando
;

declaracao:
    KW_INT ID ';'          { adiciona_var($2, 0, 1); $$ = ast_declaracao($2, 0, 1); }
  | KW_FLOAT ID ';'        { adiciona_var($2, 1, 1); $$ = ast_declaracao($2, 1, 1); }
  | KW_STRING ID ';'       { adiciona_var($2, 2, 1); $$ = ast_declaracao($2, 2, 1); }
  | KW_INT ID '[' INT_CONST ']' ';' { adiciona_var($2, 0, $4); $$ = ast_declaracao($2, 0, $4); }
  | KW_FLOAT ID '[' INT_CONST ']' ';' { adiciona_var($2, 1, $4); $$ = ast_declaracao($2, 1, $4); }
  | KW_STRING ID '[' INT_CONST ']' ';' { adiciona_var($2, 2, $4); $$ = ast_declaracao($2, 2, $4); }
;

comando:
    RECEBA ID ';' { $$ = ast_receba($2, NULL); }
  | RECEBA ID '[' INT_CONST ']' ';' { $$ = ast_receba($2, ast_const_int($4)); }
  | DEVOLVA lista_de_impressao ';' { $$ = ast_devolva($2); }
  | ID OP_ATRIBUICAO expressao ';' { $$ = ast_atribuicao($1, NULL, $3); }
  | ID '[' INT_CONST ']' OP_ATRIBUICAO expressao ';' { $$ = ast_atribuicao($1, ast_const_int($3), $6); }
  | condicional { $$ = $1; }
  | repeticao { $$ = $1; }
;

lista_de_impressao:
      item_impressao { $$ = ast_impressao($1); }
    | lista_de_impressao ',' item_impressao { $$ = ast_lista_impressao($1, $3); }
;

item_impressao:
      STRING { $$ = ast_string($1); free($1); }
    | ID { $$ = ast_var($1, NULL); free($1); }
    | ID '[' expressao ']' { $$ = ast_var($1, $3); free($1); }
;

condicional:
    KW_IF '(' expressao ')' bloco { $$ = ast_if($3, $5, NULL); }
  | KW_IF '(' expressao ')' bloco KW_ELSE bloco { $$ = ast_if($3, $5, $7); }
;

repeticao:
    KW_WHILE '(' expressao ')' bloco { $$ = ast_while($3, $5); }
;

expressao:
      expressao OP_OR expressao   { $$ = ast_binop(OP_OR, $1, $3); }
    | expressao OP_AND expressao   { $$ = ast_binop(OP_AND, $1, $3); }
    | expressao OP_IGUAL expressao       { $$ = ast_binop("==", $1, $3); }
    | expressao OP_DIFERENTE expressao   { $$ = ast_binop("!=", $1, $3); }
    | expressao OP_MAIOR expressao       { $$ = ast_binop(">", $1, $3); }
    | expressao OP_MENOR expressao       { $$ = ast_binop("<", $1, $3); }
    | expressao OP_MAIORIGUAL expressao  { $$ = ast_binop(">=", $1, $3); }
    | expressao OP_MENORIGUAL expressao  { $$ = ast_binop("<=", $1, $3); }
    | expressao ARI_MAIS expressao { $$ = ast_binop(ARI_MAIS, $1, $3); }
    | expressao ARI_MENOS expressao { $$ = ast_binop(ARI_MENOS, $1, $3); }
    | expressao ARI_VEZES expressao { $$ = ast_binop(ARI_VEZES, $1, $3); }
    | expressao ARI_DIVIDE expressao { $$ = ast_binop(ARI_DIVIDE, $1, $3); }
    | '(' expressao ')' { $$ = $2; }
    | INT_CONST { $$ = ast_const_int($1); }
    | FLOAT_CONST { $$ = ast_const_float($1); }
    | ID { $$ = ast_var($1, NULL); free($1); }
    | ID '[' expressao ']' { $$ = ast_var($1, $3); free($1); }
;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Erro: %s\n", s);
}

int main(int argc, char **argv) {
    if(argc < 2) {
        fprintf(stderr, "Uso: %s arquivo.ml\n", argv[0]);
        return 1;
    }
    FILE *f = fopen(argv[1], "r");
    if(!f) {
        perror("Erro ao abrir arquivo");
        return 1;
    }
    extern FILE *yyin;
    yyin = f;
    printf("Iniciando compilador...\n");
    yydebug = 1;
    yyparse();
    fclose(f);
    return 0;
}
