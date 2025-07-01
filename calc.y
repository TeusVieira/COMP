%debug
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void yyerror(const char *s);
int yylex(void);

typedef struct Variavel {
    char *nome;
    int tipo; // 0=int, 1=float, 2=string
    int tamanho; // 1 para variável simples, >1 para arrays
    union {
        int *vi;
        float *vf;
        char **vs;
    } valor;
} Variavel;

#define MAX_VARS 100
Variavel tabela_vars[MAX_VARS];
int num_vars = 0;

int procura_var(char *nome) {
    for (int i=0; i<num_vars; i++) {
        if (strcmp(tabela_vars[i].nome, nome) == 0)
            return i;
    }
    return -1;
}

void adiciona_var(char *nome, int tipo, int tamanho) {
    if (procura_var(nome) != -1) {
        fprintf(stderr, "Erro: variável '%s' já declarada.\n", nome);
        exit(1);
    }
    if(num_vars == MAX_VARS) {
        fprintf(stderr, "Erro: número máximo de variáveis atingido.\n");
        exit(1);
    }
    Variavel *v = &tabela_vars[num_vars++];
    v->nome = strdup(nome);
    v->tipo = tipo;
    v->tamanho = tamanho;
    if (tipo == 0) { // int
        v->valor.vi = malloc(sizeof(int) * tamanho);
        for (int i=0; i<tamanho; i++) v->valor.vi[i] = 0;
    } else if (tipo == 1) { // float
        v->valor.vf = malloc(sizeof(float) * tamanho);
        for (int i=0; i<tamanho; i++) v->valor.vf[i] = 0.0;
    } else { // string
        v->valor.vs = malloc(sizeof(char*) * tamanho);
        for (int i=0; i<tamanho; i++) v->valor.vs[i] = NULL;
    }
}

Variavel* pega_var(char *nome) {
    int idx = procura_var(nome);
    if (idx == -1) {
        fprintf(stderr, "Erro: variável '%s' não declarada.\n", nome);
        exit(1);
    }
    return &tabela_vars[idx];
}

void imprime_var(Variavel *v, int pos) {
    if (pos < 0 || pos >= v->tamanho) {
        fprintf(stderr, "Erro: índice %d fora do limite do array %s\n", pos, v->nome);
        exit(1);
    }
    if (v->tipo == 0) printf("%d", v->valor.vi[pos]);
    else if (v->tipo == 1) printf("%f", v->valor.vf[pos]);
    else if (v->tipo == 2) {
        if (v->valor.vs[pos]) printf("%s", v->valor.vs[pos]);
        else printf("(null)");
    }
}

%}

%union {
    int ival;
    float fval;
    char *sval;
}

%token <ival> INT_CONST
%token <fval> FLOAT_CONST
%token <sval> ID STRING

%token INICIO FIM
%token KW_INT KW_FLOAT KW_STRING
%token RECEBA DEVOLVA
%token KW_IF KW_ELSE KW_WHILE

%token OP_IGUAL OP_DIFERENTE OP_MAIORIGUAL OP_MENORIGUAL OP_MAIOR OP_MENOR OP_ATRIBUICAO

%type <ival> expressao
%type <sval> lista_de_impressao item_impressao

%left OP_IGUAL OP_DIFERENTE
%left OP_MAIOR OP_MENOR OP_MAIORIGUAL OP_MENORIGUAL
%left '+' '-'
%left '*' '/'


%%

programa:
    INICIO bloco FIM { printf("Programa finalizado com sucesso.\n"); }
;

bloco:
    bloco linha
  | linha
;

linha:
    declaracao
  | comando
;

declaracao:
    KW_INT ID ';'          { adiciona_var($2, 0, 1); }
  | KW_FLOAT ID ';'        { adiciona_var($2, 1, 1); }
  | KW_STRING ID ';'       { adiciona_var($2, 2, 1); }
  | KW_INT ID '[' INT_CONST ']' ';' { adiciona_var($2, 0, $4); }
  | KW_FLOAT ID '[' INT_CONST ']' ';' { adiciona_var($2, 1, $4); }
  | KW_STRING ID '[' INT_CONST ']' ';' { adiciona_var($2, 2, $4); }
;

comando:
    RECEBA ID ';' { 
        Variavel *v = pega_var($2);
        if (v->tipo == 0) {
            printf("Digite valor inteiro para %s: ", v->nome);
            scanf("%d", &v->valor.vi[0]);
        } else if (v->tipo == 1) {
            printf("Digite valor float para %s: ", v->nome);
            scanf("%f", &v->valor.vf[0]);
        } else {
            printf("Digite valor string para %s: ", v->nome);
            char buffer[100];
            getchar();
            fgets(buffer, sizeof(buffer), stdin);
            buffer[strcspn(buffer, "\n")] = 0;
            if(v->valor.vs[0]) free(v->valor.vs[0]);
            v->valor.vs[0] = strdup(buffer);
        }
    }
  | RECEBA ID '[' INT_CONST ']' ';' { 
        Variavel *v = pega_var($2);
        int pos = $4;
        if(pos < 0 || pos >= v->tamanho) {
            fprintf(stderr, "Erro: índice %d fora do limite do array %s\n", pos, v->nome);
            exit(1);
        }
        if (v->tipo == 0) {
            printf("Digite valor inteiro para %s[%d]: ", v->nome, pos);
            scanf("%d", &v->valor.vi[pos]);
        } else if (v->tipo == 1) {
            printf("Digite valor float para %s[%d]: ", v->nome, pos);
            scanf("%f", &v->valor.vf[pos]);
        } else {
            printf("Digite valor string para %s[%d]: ", v->nome, pos);
            char buffer[100];
            getchar();
            fgets(buffer, sizeof(buffer), stdin);
            buffer[strcspn(buffer, "\n")] = 0;
            if(v->valor.vs[pos]) free(v->valor.vs[pos]);
            v->valor.vs[pos] = strdup(buffer);
        }
    }
  | DEVOLVA lista_de_impressao ';' { printf("\n"); }
  | ID OP_ATRIBUICAO expressao ';' {
        Variavel *v = pega_var($1);
        if(v->tipo == 0) v->valor.vi[0] = $3;
        else if(v->tipo == 1) v->valor.vf[0] = (float)$3;
        else {
            fprintf(stderr, "Erro: atribuição direta não suportada para string sem uso de RECEBA.\n");
            exit(1);
        }
    }
  | ID '[' INT_CONST ']' OP_ATRIBUICAO expressao ';' {
        Variavel *v = pega_var($1);
        int pos = $3;
        if(pos < 0 || pos >= v->tamanho) {
            fprintf(stderr, "Erro: índice %d fora do limite do array %s\n", pos, v->nome);
            exit(1);
        }
        if(v->tipo == 0) v->valor.vi[pos] = $6;
        else if(v->tipo == 1) v->valor.vf[pos] = (float)$6;
        else {
            fprintf(stderr, "Erro: atribuição direta não suportada para string sem uso de RECEBA.\n");
            exit(1);
        }
    }
  | condicional
  | repeticao
;

lista_de_impressao:
      item_impressao
    | lista_de_impressao ',' item_impressao { printf(" "); }
;

item_impressao:
      STRING { printf("%s", $1); free($1); }
    | ID {
        Variavel *v = pega_var($1);
        imprime_var(v, 0);
        free($1);
      }
    | ID '[' expressao ']' {
    Variavel *v = pega_var($1);
    int pos = $3;
    if(pos < 0 || pos >= v->tamanho) {
        fprintf(stderr, "Erro: índice %d fora do limite do array %s\n", pos, v->nome);
        exit(1);
    }
    if(v->tipo == 0) {
        printf("%d", v->valor.vi[pos]);
    } else if(v->tipo == 1) {
        printf("%f", v->valor.vf[pos]);
    } else if(v->tipo == 2) {
        if(v->valor.vs[pos])
            printf("%s", v->valor.vs[pos]);
        else
            printf("(null)");
    }
    free($1);
}
;

condicional:
    KW_IF '(' expressao ')' bloco
  | KW_IF '(' expressao ')' bloco KW_ELSE bloco
  | KW_IF '(' expressao ')' bloco KW_ELSE bloco {
        if ($3) {
            // Executa o primeiro bloco
        } else {
            // Executa o bloco do else
        }
    }
;

repeticao:
    KW_WHILE '(' expressao ')' bloco {
        while ($3) {

            fprintf(stderr, "Erro: estrutura de repetição while ainda não suporta execução do bloco repetidamente.\n");
        }
    }
;

expressao:
      expressao '+' expressao { $$ = $1 + $3; }
    | expressao '-' expressao { $$ = $1 - $3; }
    | expressao '*' expressao { $$ = $1 * $3; }
    | expressao '/' expressao { 
        if ($3 == 0) {
            yyerror("Divisão por zero");
            $$ = 0;
        } else {
            $$ = $1 / $3;
        }
      }
    | '(' expressao ')' { $$ = $2; }
    | INT_CONST { $$ = $1; }
    | FLOAT_CONST { $$ = (int)$1; } // simplificação
    | ID { 
        Variavel *v = pega_var($1);
        if(v->tipo == 0) $$ = v->valor.vi[0];
        else if(v->tipo == 1) $$ = (int)v->valor.vf[0];
        else {
            fprintf(stderr, "Erro: variável string não pode ser usada em expressão numérica\n");
            exit(1);
        }
        free($1);
      }
    | ID '[' expressao ']' {
        Variavel *v = pega_var($1);
        int pos = $3;
        if(pos < 0 || pos >= v->tamanho) {
            fprintf(stderr, "Erro: índice %d fora do limite do array %s\n", pos, v->nome);
            exit(1);
        }
        if(v->tipo == 0) $$ = v->valor.vi[pos];
        else if(v->tipo == 1) $$ = (int)v->valor.vf[pos];
        else {
            fprintf(stderr, "Erro: variável string não pode ser usada em expressão numérica\n");
            exit(1);
        }
        free($1);
      }
    | expressao OP_IGUAL expressao       { $$ = ($1 == $3); }
    | expressao OP_DIFERENTE expressao   { $$ = ($1 != $3); }
    | expressao OP_MAIOR expressao       { $$ = ($1 > $3); }
    | expressao OP_MENOR expressao       { $$ = ($1 < $3); }
    | expressao OP_MAIORIGUAL expressao  { $$ = ($1 >= $3); }
    | expressao OP_MENORIGUAL expressao  { $$ = ($1 <= $3); }

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
    yyparse();
    fclose(f);
    return 0;
}
