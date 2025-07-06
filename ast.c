#include "ast.h"
#include "calc.tab.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static void imprime_indent(int n) {
    for (int i = 0; i < n; i++) printf("  ");
}

// Outras funções permanecem iguais...

Ast *ast_novo_binop(int op, Ast *esq, Ast *dir) {
    Ast *a = malloc(sizeof(Ast));
    a->tipo = AST_BINOP;
    a->binop.op = op;  // duplicar string para manter a cópia
    a->binop.esq = esq;
    a->binop.dir = dir;
    a->prox = NULL;
    return a;
}

void ast_libera(Ast *a) {
    if (!a) return;
    switch (a->tipo) {
        case AST_PROGRAMA: ast_libera(a->programa.bloco); break;
        case AST_BLOCO:
            ast_libera(a->bloco.linha);
            ast_libera(a->bloco.prox);
            break;
        case AST_DECLARACAO:
            free(a->decl.nome);
            break;
        case AST_ATRIBUICAO:
            ast_libera(a->atrib.var);
            ast_libera(a->atrib.expr);
            break;
        case AST_LEITURA:
            ast_libera(a->leitura_var);
            break;
        case AST_ESCRITA:
            ast_libera(a->escrita_lista);
            break;
        case AST_IF:
        case AST_IF_ELSE:
            ast_libera(a->ifs.cond);
            ast_libera(a->ifs.entao);
            if (a->ifs.senao) ast_libera(a->ifs.senao);
            break;
        case AST_WHILE:
            ast_libera(a->loop.cond);
            ast_libera(a->loop.corpo);
            break;
        case AST_BINOP:
            ast_libera(a->binop.esq);
            ast_libera(a->binop.dir);
            break;
        case AST_VAR:
            free(a->var_nome);
            break;
        case AST_INT:
            break;
    }
    if (a->prox) ast_libera(a->prox);
    free(a);
}

void ast_imprime(Ast *a, int nivel) {
    if (!a) return;
    imprime_indent(nivel);
    switch (a->tipo) {
        case AST_PROGRAMA:
            printf("Programa\n");
            ast_imprime(a->programa.bloco, nivel + 1);
            break;
        case AST_BLOCO:
            printf("Bloco\n");
            ast_imprime(a->bloco.linha, nivel + 1);
            ast_imprime(a->bloco.prox, nivel);
            break;
        case AST_DECLARACAO:
            printf("Declaracao: tipo=%d nome=%s tamanho=%d\n", a->decl.var_tipo, a->decl.nome, a->decl.tamanho);
            break;
        case AST_ATRIBUICAO:
            printf("Atribuicao\n");
            ast_imprime(a->atrib.var, nivel + 1);
            ast_imprime(a->atrib.expr, nivel + 1);
            break;
        case AST_LEITURA:
            printf("Leitura\n");
            ast_imprime(a->leitura_var, nivel + 1);
            break;
        case AST_ESCRITA:
            printf("Escrita\n");
            ast_imprime(a->escrita_lista, nivel + 1);
            break;
        case AST_IF:
        case AST_IF_ELSE:
            printf("If\n");
            ast_imprime(a->ifs.cond, nivel + 1);
            ast_imprime(a->ifs.entao, nivel + 1);
            if (a->ifs.senao) {
                imprime_indent(nivel);
                printf("Else\n");
                ast_imprime(a->ifs.senao, nivel + 1);
            }
            break;
        case AST_WHILE:
            printf("While\n");
            ast_imprime(a->loop.cond, nivel + 1);
            ast_imprime(a->loop.corpo, nivel + 1);
            break;
        case AST_BINOP:
            printf("Binop: ");
            switch (a->binop.op) {
                case OP_AND: printf("&&\n"); break;
                case OP_OR: printf("||\n"); break;
                case OP_IGUAL: printf("==\n"); break;
                case OP_DIFERENTE: printf("!=\n"); break;
                case OP_MAIOR: printf(">\n"); break;
                case OP_MENOR: printf("<\n"); break;
                case OP_MAIORIGUAL: printf(">=\n"); break;
                case OP_MENORIGUAL: printf("<=\n"); break;
                case ARI_MAIS: printf("+\n"); break;
                case ARI_MENOS: printf("-\n"); break;
                case ARI_VEZES: printf("*\n"); break;
                case ARI_DIVIDE: printf("/\n"); break;
                default: printf("op desconhecido (%d)\n", a->binop.op);
                }
            
            ast_imprime(a->binop.esq, nivel + 1);
            ast_imprime(a->binop.dir, nivel + 1);
            break;
        case AST_VAR:
            printf("Var: %s\n", a->var_nome);
            break;
        case AST_INT:
            printf("Int: %d\n", a->ival);
            break;
    }
    if (nivel == 0) printf("\n");
}
