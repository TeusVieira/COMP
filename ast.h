#ifndef AST_H
#define AST_H

typedef enum {
    AST_PROGRAMA,
    AST_BLOCO,
    AST_DECLARACAO,
    AST_ATRIBUICAO,
    AST_LEITURA,
    AST_ESCRITA,
    AST_IF,
    AST_IF_ELSE,
    AST_WHILE,
    AST_BINOP,
    AST_INT,
    AST_VAR
} AstType;

typedef struct Ast {
    AstType tipo;
    union {
        struct { struct Ast *bloco; } programa;
        struct { struct Ast *linha; struct Ast *prox; } bloco;
        struct {
            int var_tipo;
            char *nome;
            int tamanho;
        } decl;
        struct {
            struct Ast *var;
            struct Ast *expr;
        } atrib;
        struct Ast *leitura_var;
        struct Ast *escrita_lista;
        struct {
            struct Ast *cond;
            struct Ast *entao;
            struct Ast *senao;
        } ifs;
        struct {
            struct Ast *cond;
            struct Ast *corpo;
        } loop;
        struct {
            int op;
            struct Ast *esq;
            struct Ast *dir;
        } binop;
        int ival;
        char *var_nome;
    };
    struct Ast *prox;
} Ast;

Ast *ast_novo_programa(Ast *bloco);
Ast *ast_novo_bloco(Ast *linha, Ast *prox);
Ast *ast_nova_decl(int tipo, char *nome, int tamanho);
Ast *ast_nova_atrib(Ast *var, Ast *expr);
Ast *ast_nova_leitura(Ast *var);
Ast *ast_nova_escrita(Ast *lista);
Ast *ast_novo_if(Ast *cond, Ast *entao, Ast *senao);
Ast *ast_novo_while(Ast *cond, Ast *corpo);
Ast *ast_novo_binop(int op, Ast *esq, Ast *dir);
Ast *ast_nova_int(int val);
Ast *ast_nova_var(char *nome);

void ast_libera(Ast *a);
void ast_imprime(Ast *a, int nivel);

#endif
