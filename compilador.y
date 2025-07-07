%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h" // Inclui o novo ast.h

extern int yylex(); // Função gerada pelo Flex
extern FILE *yyin;  // Ponteiro para o arquivo de entrada do Flex
extern int yylineno; // Variável global do Flex para número da linha

void yyerror(char *s); // <--- ADICIONE ISSO AQUI!

// --- Definição da função de erro do parser ---
void yyerror(char *s) {
    fprintf(stderr, "Erro de sintaxe na linha %d: %s\n", yylineno, s);
}

// Variável global para a raiz da AST (declarada em ast.h e definida em ast.c)
extern Ast *root;

 
%}

// --- União para os tipos de valores que as regras do parser podem retornar ---
// yyval será uma variável dessa união
%union {
    int ival;    // Para inteiros (INT_LITERAL)
    double rval;    // Para floats (REAL_LITERAL)
    char *sval;     // Para strings (ID, STRING_LITERAL)
    Ast *node;    // Para nós da AST (agora a maioria das regras retornará isso)
}

// --- Tokens (recebidos do lexer) ---
%token INT_KW REAL_KW STRING_KW ARRAY_KW
%token IF_KW THEN_KW ELSE_KW ENDIF_KW
%token WHILE_KW DO_KW ENDWHILE_KW
%token FOR_KW ENDFOR_KW
%token READ_KW WRITE_KW
%token FUNC_SQRT FUNC_ABS // Renomeei para ser mais consistente com ch_funcoes.h
// %token EOL // Removido: EOL não é um token usado na gramática, é ignorado pelo lexer

%token <ival> INT_LITERAL
%token <rval> REAL_LITERAL
%token <sval> STRING_LITERAL ID

%token ADD SUB MUL DIV
%token GT LT NE EQ GE LE OR AND
%token ASSIGN

%token LPAREN RPAREN
%token LBRACK RBRACK
%token SEMICOLON
%token COMMA

// --- Tipos para as regras gramaticais que retornarão nós da AST ---
%type <node> program
%type <node> statements_block statement_list statement_item
%type <node> declaration assign_stmt read_stmt write_stmt write_item write_args
%type <node> if_stmt else_part while_stmt for_stmt
%type <node> array_declaration array_assign array_access
%type <node> expression logical_expression factor

// --- Precedência e Associatividade dos Operadores ---
%left OR
%left AND
%nonassoc EQ NE
%nonassoc GT GE LT LE
%left ADD SUB
%left MUL DIV 
%right UMINUS 

%%

// --- Regras Gramaticais ---

// O programa é um bloco de statements
program: statements_block { root = $1; } // Define a raiz da AST aqui
         ;


// Um bloco vazio retorna um nó NODE_EMPTY.
statements_block: /* vazio */    { $$ = newast(NODE_EMPTY, NULL, NULL); }
                | statement_list { $$ = $1; }
                ;

// statement_list: Uma lista de statement_items.
// Vamos construir a lista de forma que o primeiro item da lista seja o nó mais à esquerda.
// A iteração em 'eval' será `eval(node->l); eval(node->r);`
statement_list:  statement_list SEMICOLON statement_item { $$ = newlist($1, $3); } // Recursão à esquerda
                |statement_item                         { $$ = $1; }
             
              ;

// statement_item: agrupa todos os possíveis declarações/comandos
statement_item: declaration
              | assign_stmt
              | read_stmt
              | write_stmt
              | if_stmt
              | while_stmt
              | for_stmt
              | array_declaration
              | array_assign
              ;

// --- Declarações de Variáveis ---
// Note que as funções newvardecl e newarraydecl recebem o VarType diretamente.
declaration: INT_KW ID ASSIGN expression             { $$ = newvardecl(TYPE_INT, $2, $4); }
           | INT_KW ID                               { $$ = newvardecl(TYPE_INT, $2, NULL); }
           | REAL_KW ID ASSIGN expression            { $$ = newvardecl(TYPE_REAL, $2, $4); }
           | REAL_KW ID                              { $$ = newvardecl(TYPE_REAL, $2, NULL); }
           | STRING_KW ID ASSIGN STRING_LITERAL      { $$ = newvardecl(TYPE_STRING, $2, newtext($4)); }
           | STRING_KW ID                            { $$ = newvardecl(TYPE_STRING, $2, NULL); }
           ;

assign_stmt: ID ASSIGN expression                    { $$ = newassign($1, $3); }
           ;

// --- Operações de Leitura e Escrita ---
read_stmt: READ_KW ID                                { $$ = newread(newid($2)); } // Target pode ser um ID simples
         | READ_KW array_access                      { $$ = newread($2); } // Target pode ser um acesso a array
         ;

write_stmt: WRITE_KW write_args                      { $$ = $2; } // write_args já é uma lista de nós NODE_WRITE

// write_item para item individual
write_item: expression                               { $$ = $1; } // Expressões numéricas
          | logical_expression                       { $$ = $1;}
          ;

// write_args para lista de write_item. Cria uma NODE_LIST de NODE_WRITE.
write_args: write_item                               { $$ = newwrite($1); } // O primeiro item é um NODE_WRITE
          | write_args COMMA write_item              { $$ = newlist($1, newwrite($3)); } // Cria uma lista de NODE_WRITE
                                                                                        // Mantido a recursão à esquerda: $1 é a lista existente, $3 é o novo item
          ;


// --- Estruturas Condicionais ---
if_stmt: IF_KW LPAREN logical_expression RPAREN THEN_KW statements_block else_part ENDIF_KW
         { $$ = newif($3, $6, $7); }

else_part: ELSE_KW statements_block                  { $$ = $2; }
         | /* vazio */                               { $$ = newast(NODE_EMPTY, NULL, NULL); } // Retorna um nó vazio para else opcional
         ;

// --- Estruturas de Repetição ---
while_stmt: WHILE_KW LPAREN logical_expression RPAREN DO_KW statements_block ENDWHILE_KW
            { $$ = newwhile($3, $6); }
            ;

for_stmt: FOR_KW LPAREN assign_stmt SEMICOLON logical_expression SEMICOLON assign_stmt RPAREN DO_KW statements_block ENDFOR_KW
          { $$ = newfor($3, $5, $7, $10); }
        ;

// --- Arrays (Vetores) ---
array_declaration: ARRAY_KW INT_KW ID LBRACK expression RBRACK
                   { $$ = newarraydecl(TYPE_INT, $3, $5); }
                 | ARRAY_KW REAL_KW ID LBRACK expression RBRACK
                   { $$ = newarraydecl(TYPE_REAL, $3, $5); }
                 | ARRAY_KW STRING_KW ID LBRACK expression RBRACK
                   { $$ = newarraydecl(TYPE_STRING, $3, $5); }
                 ;

array_access: ID LBRACK expression RBRACK
              { $$ = newarray_access($1, $3); }
            ;

array_assign: ID LBRACK expression RBRACK ASSIGN expression
              { $$ = newarray_assign($1, $3, $6); } // Ordem dos argumentos na função newarray_assign é (array_name, index_expr, value_expr)
            ;

// --- Expressões ---
expression: expression ADD expression                { $$ = newast(NODE_ADD, $1, $3); }
          | expression SUB expression                { $$ = newast(NODE_SUB, $1, $3); }
          | expression MUL expression                { $$ = newast(NODE_MUL, $1, $3); }
          | expression DIV expression                { $$ = newast(NODE_DIV, $1, $3); }
          | SUB factor %prec UMINUS                  { $$ = newunary(NODE_UMINUS, $2); } // Usando newunary
          | factor
          ;

factor: INT_LITERAL                                  { $$ = newint($1); }
      | REAL_LITERAL                                 { $$ = newreal($1); }
      | STRING_LITERAL                               { $$ = newtext($1); }
      | ID                                           { $$ = newid($1); }
      | array_access
      | LPAREN expression RPAREN                     { $$ = $2; }
      | FUNC_SQRT LPAREN expression RPAREN           { $$ = newunary(NODE_SQRT, $3); } // Usando newunary
      | FUNC_ABS LPAREN expression RPAREN            { $$ = newunary(NODE_ABS, $3); } // Usando newunary
      ;

// --- Expressões Lógicas (Condições) ---
logical_expression: expression GT expression         { $$ = newcmp(NODE_GT, $1, $3); }
                  | expression LT expression         { $$ = newcmp(NODE_LT, $1, $3); }
                  | expression NE expression         { $$ = newcmp(NODE_NE, $1, $3); }
                  | expression EQ expression         { $$ = newcmp(NODE_EQ, $1, $3); }
                  | expression GE expression         { $$ = newcmp(NODE_GE, $1, $3); }
                  | expression LE expression         { $$ = newcmp(NODE_LE, $1, $3); }
                  | logical_expression OR logical_expression { $$ = newlogical(NODE_OR, $1, $3); } // Usando newlogical
                  | logical_expression AND logical_expression { $$ = newlogical(NODE_AND, $1, $3); } // Usando newlogical
                  | LPAREN logical_expression RPAREN { $$ = $2; }
                  ;

%%