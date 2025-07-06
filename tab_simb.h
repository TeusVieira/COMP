// tab_simb.h
#ifndef TAB_SIMB_H
#define TAB_SIMB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "ast.h" // Necessário para VarType no ARRAY_VAR e em 'varexiste' talvez

// Supondo um tamanho máximo para strings nas variáveis
#define MAX_STR_LEN 256

// --- Estruturas da Tabela de Símbolos ---

// Para variáveis do tipo REAL (double)
typedef struct Vars {
    char name[256];
    double v;
    struct Vars *next; // <-- AQUI: use 'next'
} VARS;

// Para variáveis do tipo INT (int)
typedef struct Varsi {
    char name[256];
    int v;
    struct Varsi *next; // <-- AQUI: use 'next'
} VARSI;

// Para variáveis do tipo STRING (char*)
typedef struct Varst {
    char name[256];
    char v[MAX_STR_LEN];
    struct Varst *next; // <-- AQUI: use 'next'
} VARST;

// Para Arrays (Vetores)
typedef struct Array_Var {
    char name[256];
    VarType type; // Tipo dos elementos do array (TYPE_INT, TYPE_REAL, TYPE_STRING)
    int size;     // Tamanho do array
    void *data;   // Ponteiro genérico para os dados (malloc para int*, double*, char**)
    struct Array_Var *next; // <-- AQUI: use 'next'
} ARRAY_VAR;

// Para Funções (DESCOMENTE SE FOR USAR)
typedef struct Function {
    char name[256];
    Ast *body; // Ponteiro para o nó da AST que representa o corpo da função
    VarType return_type; // Tipo de retorno da função
    // ... outros campos como lista de parâmetros ...
    struct Function *next; // <-- AQUI: use 'next'
} Function;


// --- Variáveis Globais da Tabela de Símbolos (declaradas em tab_simb.c) ---
extern VARS *rvar;
extern VARSI *ivar;
extern VARST *tvar;
extern ARRAY_VAR *array_table;
extern Function *listFunction; // <-- DESCOMENTE SE FOR USAR FUNÇÕES


// --- Protótipos das Funções da Tabela de Símbolos ---

VARS *insr(VARS *l, char n[]);
VARS *srch(VARS *l, char n[]);

VARSI *insi(VARSI *l, char n[]);
VARSI *srchi(VARSI *l, char n[]);

VARST *inst(VARST *l, char n[]);
VARST *srcht(VARST *l, char n[]);

ARRAY_VAR *ins_array(ARRAY_VAR *l, char n[], VarType type, int size); // <-- AQUI: VarType
ARRAY_VAR *srch_array(ARRAY_VAR *l, char n[]);

bool varexiste(char v[]); // Verifica se uma variável/array já existe

// Protótipos para funções (DESCOMENTE SE FOR USAR FUNÇÕES)
Function *insfunction(Function *l, char n[], VarType type, Ast *body);
Function *srchfunction(Function *l, char n[]);


// Funções para liberar a memória da tabela de símbolos
void free_rvar(VARS *l);
void free_ivar(VARSI *l);
void free_tvar(VARST *l);
void free_array_table(ARRAY_VAR *l);
void free_functions(Function *l); // <-- DESCOMENTE SE FOR USAR FUNÇÕES

#endif // TAB_SIMB_H