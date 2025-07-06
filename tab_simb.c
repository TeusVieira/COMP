// tab_simb.c
#include "tab_simb.h" // Inclui o tab_simb.h
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // Para strcmp, strcpy, strdup

// --- Variáveis globais da tabela de símbolos (definidas aqui) ---
VARS *rvar = NULL;
VARSI *ivar = NULL;
VARST *tvar = NULL;
ARRAY_VAR *array_table = NULL;
Function *listFunction = NULL; // DESCOMENTE APENAS SE ESTIVER USANDO FUNÇÕES


// --- Implementações das Funções da Tabela de Símbolos ---

// Variáveis REAL
VARS *insr(VARS *l, char n[]) {
    VARS *novo = (VARS *)malloc(sizeof(VARS));
    if (!novo) {
        perror("Erro de alocacao de memoria para VARS");
        exit(1);
    }
    strcpy(novo->name, n);
    novo->v = 0.0; // Valor inicial padrão
    novo->next = l; // <-- Corrigido: 'next'
    return novo;
}

VARS *srch(VARS *l, char n[]) {
    VARS *aux = l; // Use uma variável auxiliar para iterar
    for (; aux != NULL; aux = aux->next) { // <-- Corrigido: 'next'
        if (strcmp(aux->name, n) == 0) {
            return aux;
        }
    }
    return NULL;
}

// Variáveis INT
VARSI *insi(VARSI *l, char n[]) {
    VARSI *novo = (VARSI *)malloc(sizeof(VARSI));
    if (!novo) {
        perror("Erro de alocacao de memoria para VARSI");
        exit(1);
    }
    strcpy(novo->name, n);
    novo->v = 0; // Valor inicial padrão
    novo->next = l; // <-- Corrigido: 'next'
    return novo;
}

VARSI *srchi(VARSI *l, char n[]) {
    VARSI *aux = l; // Use uma variável auxiliar para iterar
    for (; aux != NULL; aux = aux->next) { // <-- Corrigido: 'next'
        if (strcmp(aux->name, n) == 0) {
            return aux;
        }
    }
    return NULL;
}

// Variáveis STRING
VARST *inst(VARST *l, char n[]) {
    VARST *novo = (VARST *)malloc(sizeof(VARST));
    if (!novo) {
        perror("Erro de alocacao de memoria para VARST");
        exit(1);
    }
    strcpy(novo->name, n);
    novo->v[0] = '\0'; // String vazia como valor inicial padrão
    novo->next = l; // <-- Corrigido: 'next'
    return novo;
}

VARST *srcht(VARST *l, char n[]) {
    VARST *aux = l; // Use uma variável auxiliar para iterar
    for (; aux != NULL; aux = aux->next) { // <-- Corrigido: 'next'
        if (strcmp(aux->name, n) == 0) {
            return aux;
        }
    }
    return NULL;
}

// Arrays
// CORREÇÃO: Assinatura da função para usar VarType
ARRAY_VAR *ins_array(ARRAY_VAR *l, char n[], VarType type, int size) {
    ARRAY_VAR *novo = (ARRAY_VAR *)malloc(sizeof(ARRAY_VAR));
    if (!novo) {
        perror("Erro de alocacao de memoria para ARRAY_VAR");
        exit(1);
    }
    strcpy(novo->name, n);
    novo->type = type;
    novo->size = size;

    // Aloca a memória para os dados do array com base no tipo
    if (type == TYPE_INT) {
        novo->data = calloc(size, sizeof(int));
    } else if (type == TYPE_REAL) {
        novo->data = calloc(size, sizeof(double));
    } else if (type == TYPE_STRING) {
        novo->data = calloc(size, sizeof(char*));
        // Inicializa ponteiros de string para NULL
        for (int i = 0; i < size; i++) {
            ((char**)novo->data)[i] = NULL;
        }
    } else {
        fprintf(stderr, "Erro: Tipo de array desconhecido na alocacao.\n");
        exit(1);
    }

    if (!novo->data) {
        perror("Erro de alocacao de memoria para dados do array");
        exit(1);
    }
    novo->next = l; // <-- Corrigido: 'next'
    return novo;
}

ARRAY_VAR *srch_array(ARRAY_VAR *l, char n[]) {
    ARRAY_VAR *aux = l; // Use uma variável auxiliar para iterar
    for (; aux != NULL; aux = aux->next) { // <-- Corrigido: 'next'
        if (strcmp(aux->name, n) == 0) {
            return aux;
        }
    }
    return NULL;
}

// Verifica se uma variável (simples ou array) já existe
bool varexiste(char v[]) {
    if (srch(rvar, v) != NULL) return true;
    if (srchi(ivar, v) != NULL) return true;
    if (srcht(tvar, v) != NULL) return true;
    if (srch_array(array_table, v) != NULL) return true;
    return false;
}

// Funções para Funções (DESCOMENTE APENAS SE ESTIVER USANDO FUNÇÕES)
Function *insfunction(Function *l, char n[], VarType type, Ast *body) {
    Function *novo = (Function *)malloc(sizeof(Function));
    if (!novo) {
        perror("Erro de alocacao de memoria para Function");
        exit(1);
    }
    strcpy(novo->name, n);
    novo->return_type = type;
    novo->body = body;
    novo->next = l; // <-- Corrigido: 'next'
    return novo;
}

Function *srchfunction(Function *l, char n[]){
    Function *aux = l; // Use uma variável auxiliar para iterar
    for(;aux != NULL;aux = aux->next) { // <-- Corrigido: 'next'
        if(strcmp(aux->name, n) == 0) {
            return aux;
        }
    }
    return NULL;
}


// --- Funções para liberar a memória da tabela de símbolos ---

void free_rvar(VARS *l) {
    VARS *temp;
    while (l != NULL) {
        temp = l;
        l = l->next;
        free(temp);
    }
}

void free_ivar(VARSI *l) {
    VARSI *temp;
    while (l != NULL) {
        temp = l;
        l = l->next;
        free(temp);
    }
}

void free_tvar(VARST *l) {
    VARST *temp;
    while (l != NULL) {
        temp = l;
        l = l->next;
        free(temp);
    }
}

void free_array_table(ARRAY_VAR *l) {
    ARRAY_VAR *temp;
    while (l != NULL) {
        temp = l;
        l = l->next;
        // Libera os dados do array
        if (temp->type == TYPE_STRING) {
            // Para arrays de string, liberar cada string individualmente
            char **str_data = (char**)temp->data;
            for (int i = 0; i < temp->size; i++) {
                if (str_data[i] != NULL) {
                    free(str_data[i]);
                }
            }
        }
        free(temp->data); // Libera o ponteiro para os dados
        free(temp); // Libera o nó da lista
    }
}

// Função para liberar funções (DESCOMENTE APENAS SE ESTIVER USANDO FUNÇÕES)
void free_functions(Function *l) {
    Function *temp;
    while (l != NULL) {
        temp = l;
        l = l->next;
        // Se o corpo da função (body) for uma AST, você precisa liberar ela também
        // treefree(temp->body); // Descomente se 'body' é uma Ast* e precisa ser liberada
        free(temp);
    }
}

