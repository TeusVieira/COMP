// ast.h
#ifndef AST_H
#define AST_H

#include <stdio.h> // Para FILE, NULL
#include <stdlib.h> // Para malloc, free, exit
#include <string.h> // Para strdup, strlen

// --- Enums para Tipos de Variáveis/Literais (para uso semântico) ---
typedef enum {
    TYPE_INT,
    TYPE_REAL,
    TYPE_STRING,
    TYPE_UNKNOWN // Para casos onde o tipo ainda não foi determinado
} VarType;

// --- Enums para Tipos de Nó da AST (nodetype) ---
// Estes são os identificadores que seu 'eval' usará no switch-case
typedef enum {
    // Nodos para Literais e Identificadores
    NODE_INT_LIT,       // Literal inteiro (ex: 10, 0, -5)
    NODE_REAL_LIT,      // Literal real (ex: 3.14, 0.0)
    NODE_STRING_LIT,    // Literal string (ex: "hello world")
    NODE_ID,            // Referência a um identificador (variável)
    NODE_ARRAY_ACCESS,  // Acesso a elemento de array (ex: meuArray[indice])

    // Nodos para Operadores Unários
    NODE_UMINUS,        // Menos unário (ex: -expressao)
    NODE_NOT,           // Negação lógica (ex: !condicao) // Se sua linguagem tem 'nao'

    // Nodos para Operadores Binários Aritméticos
    NODE_ADD,           // Soma (+)
    NODE_SUB,           // Subtração (-)
    NODE_MUL,           // Multiplicação (*)
    NODE_DIV,           // Divisão (/)

    // Nodos para Operadores de Comparação
    NODE_EQ,            // Igual (==)
    NODE_NE,            // Diferente (!=)
    NODE_LT,            // Menor que (<)
    NODE_LE,            // Menor ou igual (<=)
    NODE_GT,            // Maior que (>)
    NODE_GE,            // Maior ou igual (>=)

    // Nodos para Operadores Lógicos
    NODE_AND,           // E lógico (AND)
    NODE_OR,            // Ou lógico (OR)

    // Nodos para Comandos de Controle de Fluxo
    NODE_IF,            // Comando IF-THEN-ELSE (se...entao...senao...fimse)
    NODE_WHILE,         // Comando WHILE (enquanto...faca...fimenquanto)
    NODE_FOR,           // Comando FOR (para...faca...fimpara)

    // Nodos para Comandos de E/S (Input/Output)
    NODE_READ,          // Comando LER
    NODE_WRITE,         // Comando ESCREVER

    // Nodos para Atribuição e Declaração
    NODE_ASSIGN,        // Atribuição de variável simples (ID = EXPRESSAO)
    NODE_ARRAY_ASSIGN,  // Atribuição em array (ID[EXPRESSAO] = EXPRESSAO)
    NODE_VAR_DECL,      // Declaração de variável (int/real/string ID)
    NODE_ARRAY_DECL,    // Declaração de array (array int ID [SIZE])

    // Nodos para Construções de Bloco/Lista
    NODE_LIST,          // Uma lista sequencial de comandos/statements
    NODE_EMPTY,         // Um nó vazio (útil para partes opcionais, ex: ELSE vazio)

    // Nodos para Funções Embutidas (se existirem na sua gramática)
    NODE_SQRT,          // Raiz quadrada (sqrt)
    NODE_ABS            // Valor absoluto (abs)

    // Adicione mais tipos de nós conforme sua gramática se expandir
} NodeType;

// --- Estrutura unificada do nó da AST ---
typedef struct Ast {
    NodeType nodetype; // Tipo do nó (qual enum acima ele representa)
    VarType type;      // Tipo de dado semântico (usado para inferir/checar tipos)

    // Para literais
    int ivalue;         // Para NODE_INT_LIT
    double rvalue;      // Para NODE_REAL_LIT
    char *svalue;       // Para NODE_STRING_LIT ou NODE_ID (nome do identificador)

    // Ponteiros para filhos da AST
    // l, r, m, e podem ser usados de forma genérica dependendo do nodetype
    // Por exemplo:
    // NODE_ADD: l = operando esquerdo, r = operando direito
    // NODE_IF:  l = condição, r = bloco THEN, m = bloco ELSE
    // NODE_FOR: l = inicialização, r = condição, m = iteração, e = corpo
    struct Ast *l;      // Left child
    struct Ast *r;      // Right child
    struct Ast *m;      // Middle child (para 3 filhos)
    struct Ast *e;      // Extra child (para 4 filhos)

} Ast;

// --- Protótipos das Funções Auxiliares para Construir a AST ---
// Funções de propósito geral para criar nós
Ast *newast(NodeType nodetype, Ast *l, Ast *r);
Ast *newlist(Ast *first_item, Ast *rest_of_list); // Para criar listas de statements/args
Ast *newunary(NodeType nodetype, Ast *child); // Para operadores unários (ex: -expr, !cond)
Ast *newcmp(NodeType nodetype, Ast *l, Ast *r); // Para operações de comparação
Ast *newlogical(NodeType nodetype, Ast *l, Ast *r); // Para operadores lógicos

// Funções para criar nós de literais
Ast *newint(int i);
Ast *newreal(double r);
Ast *newtext(char *s);

// Funções para criar nós de identificadores e acessos
Ast *newid(char *name);
Ast *newarray_access(char *array_name, Ast *index_expr);

// Funções para comandos
Ast *newassign(char *id_name, Ast *value_expr);
Ast *newarray_assign(char *array_name, Ast *index_expr, Ast *value_expr);
Ast *newread(Ast *target_var); // target_var pode ser NODE_ID ou NODE_ARRAY_ACCESS
Ast *newwrite(Ast *write_item_expr); // write_item pode ser literal, ID, array_access, expr
                                      // Se 'escrever' aceita múltiplos args, usaremos NODE_LIST aqui

// Funções para declarações
Ast *newvardecl(VarType type, char *id_name, Ast *init_value_expr);
Ast *newarraydecl(VarType type, char *id_name, Ast *size_expr);

// Funções para fluxo de controle
Ast *newif(Ast *cond, Ast *then_body, Ast *else_body);
Ast *newwhile(Ast *cond, Ast *body);
Ast *newfor(Ast *init_stmt, Ast *cond_expr, Ast *iter_stmt, Ast *body);

// --- Variável global para a raiz da AST ---
extern Ast *root;

// --- Função para liberação de memória da AST ---
void treefree(Ast *a);

#endif // AST_H