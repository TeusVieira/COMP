// ast.c
#include "ast.h" // Inclui ast.h (que já inclui stdio.h, stdlib.h, string.h)

// Variável global para a raiz da AST
Ast *root = NULL;

// --- Função auxiliar interna para alocar um novo nó ---
// Inicializa os campos comuns a todos os nós
static Ast *mknode(NodeType nodetype, Ast *l, Ast *r, Ast *m, Ast *e) {
    Ast *a = (Ast *)malloc(sizeof(Ast));
    if (!a) {
        perror("Erro de alocacao de memoria para no AST");
        exit(1); // Aborta se não conseguir alocar memória
    }
    a->nodetype = nodetype;
    a->type = TYPE_UNKNOWN; // Tipo semântico inicial desconhecido
    a->ivalue = 0;
    a->rvalue = 0.0;
    a->svalue = NULL;
    a->l = l;
    a->r = r;
    a->m = m;
    a->e = e;
    return a;
}

// --- Funções para criar nós da AST (Implementações) ---

// Nós genéricos de 2 filhos (ou menos)
Ast *newast(NodeType nodetype, Ast *l, Ast *r) {
    return mknode(nodetype, l, r, NULL, NULL);
}

// Nós unários
Ast *newunary(NodeType nodetype, Ast *child) {
    return mknode(nodetype, child, NULL, NULL, NULL);
}

// Nós de comparação
Ast *newcmp(NodeType nodetype, Ast *l, Ast *r) {
    return mknode(nodetype, l, r, NULL, NULL);
}

// Nós lógicos
Ast *newlogical(NodeType nodetype, Ast *l, Ast *r) {
    return mknode(nodetype, l, r, NULL, NULL);
}

// Nó de lista (para sequências de comandos, argumentos de print, etc.)
// A lista é construída de forma encadeada: newlist(item_atual, resto_da_lista)
Ast *newlist(Ast *first_item, Ast *rest_of_list) {
    return mknode(NODE_LIST, first_item, rest_of_list, NULL, NULL);
}

// --- Funções para Literais ---
Ast *newint(int i) {
    Ast *a = mknode(NODE_INT_LIT, NULL, NULL, NULL, NULL);
    a->ivalue = i;
    a->type = TYPE_INT;
    return a;
}

Ast *newreal(double r) {
    Ast *a = mknode(NODE_REAL_LIT, NULL, NULL, NULL, NULL);
    a->rvalue = r;
    a->type = TYPE_REAL;
    return a;
}

Ast *newtext(char *s) {
    Ast *a = mknode(NODE_STRING_LIT, NULL, NULL, NULL, NULL);
    a->svalue = strdup(s); // Duplica a string para evitar problemas de memória
    if (!a->svalue) {
        perror("Erro de alocacao para string literal");
        exit(1);
    }
    a->type = TYPE_STRING;
    return a;
}

// --- Funções para Identificadores e Acessos ---
Ast *newid(char *name) {
    Ast *a = mknode(NODE_ID, NULL, NULL, NULL, NULL);
    a->svalue = strdup(name);
    if (!a->svalue) {
        perror("Erro de alocacao para nome de identificador");
        exit(1);
    }
    // O 'type' será preenchido durante a análise semântica (tabela de símbolos)
    return a;
}

Ast *newarray_access(char *array_name, Ast *index_expr) {
    // NODE_ARRAY_ACCESS: l = ID do array, r = expressao do índice
    Ast *id_node = newid(array_name); // Cria um nó para o nome do array
    return mknode(NODE_ARRAY_ACCESS, id_node, index_expr, NULL, NULL);
}

// --- Funções para Comandos ---
Ast *newassign(char *id_name, Ast *value_expr) {
    // NODE_ASSIGN: l = ID da variável, r = expressao do valor
    Ast *id_node = newid(id_name);
    return mknode(NODE_ASSIGN, id_node, value_expr, NULL, NULL);
}

Ast *newarray_assign(char *array_name, Ast *index_expr, Ast *value_expr) {
    // NODE_ARRAY_ASSIGN: l = NODE_ARRAY_ACCESS (nome+indice), r = expressao do valor
    Ast *array_access_node = newarray_access(array_name, index_expr);
    return mknode(NODE_ARRAY_ASSIGN, array_access_node, value_expr, NULL, NULL);
}

Ast *newread(Ast *target_var) {
    // NODE_READ: l = NODE_ID ou NODE_ARRAY_ACCESS
    return mknode(NODE_READ, target_var, NULL, NULL, NULL);
}

Ast *newwrite(Ast *write_item_expr) {
    // NODE_WRITE: l = item a ser impresso (literal, ID, expr), r = proximo item na lista (se NODE_LIST)
    // O tratamento de múltiplos argumentos para 'escrever' será feito com NODE_LIST no compilador.y
    return mknode(NODE_WRITE, write_item_expr, NULL, NULL, NULL);
}


// --- Funções para Declarações ---
Ast *newvardecl(VarType type, char *id_name, Ast *init_value_expr) {
    // NODE_VAR_DECL: l = ID, r = valor inicial (se houver)
    Ast *id_node = newid(id_name);
    Ast *a = mknode(NODE_VAR_DECL, id_node, init_value_expr, NULL, NULL);
    a->type = type; // Armazena o tipo declarado aqui
    return a;
}

Ast *newarraydecl(VarType type, char *id_name, Ast *size_expr) {
    // NODE_ARRAY_DECL: l = ID, r = expressao do tamanho
    Ast *id_node = newid(id_name);
    Ast *a = mknode(NODE_ARRAY_DECL, id_node, size_expr, NULL, NULL);
    a->type = type; // Armazena o tipo base do array
    return a;
}

// --- Funções para Fluxo de Controle ---
Ast *newif(Ast *cond, Ast *then_body, Ast *else_body) {
    // NODE_IF: l = condicao, r = bloco THEN, m = bloco ELSE
    return mknode(NODE_IF, cond, then_body, else_body, NULL);
}

Ast *newwhile(Ast *cond, Ast *body) {
    // NODE_WHILE: l = condicao, r = corpo do loop
    return mknode(NODE_WHILE, cond, body, NULL, NULL);
}

Ast *newfor(Ast *init_stmt, Ast *cond_expr, Ast *iter_stmt, Ast *body) {
    // NODE_FOR: l = inicializacao, r = condicao, m = iteracao, e = corpo
    return mknode(NODE_FOR, init_stmt, cond_expr, iter_stmt, body);
}

// --- Função para Liberação de Memória da AST ---
void treefree(Ast *a) {
    if (a == NULL) return;

    // Libera os filhos recursivamente
    treefree(a->l);
    treefree(a->r);
    treefree(a->m);
    treefree(a->e);

    // Libera a string se for um nó que a contém (NODE_STRING_LIT, NODE_ID)
    if (a->nodetype == NODE_STRING_LIT || a->nodetype == NODE_ID) {
        if (a->svalue != NULL) {
            free(a->svalue);
        }
    }
    // Libera o próprio nó
    free(a);
}