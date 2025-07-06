#include "ast.h"         
#include "tab_simb.h"    
#include "compilador.tab.h" 
#include <stdio.h>       
#include <stdlib.h>      
#include <string.h>      

// --- Variáveis e funções externas ---
extern FILE *yyin;        // Ponteiro para o arquivo de entrada do Flex
extern int yyparse();     // Função principal do parser, gerada pelo Bison
extern int yylineno;      // Variável global do Flex para o número da linha atual
extern Ast *root;         



// Funções da tabela de símbolos (declaradas em tab_simb.h, definidas em tab_simb.c)
extern VARS *rvar;
extern VARSI *ivar;
extern VARST *tvar;
extern ARRAY_VAR *array_table;
// extern Function *listFunction; // Se você tem funções

// Protótipos das funções de liberação de memória (declarados em tab_simb.h)
extern void free_rvar(VARS *l);
extern void free_ivar(VARSI *l);
extern void free_tvar(VARST *l);
extern void free_array_table(ARRAY_VAR *l);
// extern void free_functions(Function *l); // Se você tem funções

// Protótipo da função de avaliação da AST (definida em eval.c)
extern double eval(Ast *a); // Supondo que eval retorna um double

extern void yyerror(char *s);

int yydebug = 1;
// --- Função principal do compilador ---
int main(int argc, char **argv) {
    // 1. Verifica se o arquivo de entrada foi fornecido
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <arquivo_fonte.ml>\n", argv[0]);
        return 1; // Retorna 1 indicando erro de uso
    }

    // 2. Abre o arquivo de entrada para o lexer
    yyin = fopen(argv[1], "r");
    if (!yyin) {
        // Usa perror para imprimir uma mensagem de erro mais detalhada do sistema
        perror("Erro ao abrir o arquivo de entrada");
        return 1; // Retorna 1 indicando erro de arquivo
    }

    // Inicializa o contador de linha do lexer (essencial para mensagens de erro precisas)
    yylineno = 1;

    printf("Iniciando análise sintática para '%s'...\n", argv[1]);
    
   
    int parse_result = yyparse();

    fclose(yyin);

    if (parse_result == 0) {
        printf("Análise sintática concluída com sucesso.\n");

        // Se a raiz da AST foi criada, procede com a avaliação
        if (root) {
            printf("\n--- Iniciando a execução do programa ---\n");
            eval(root); // Chama a função de avaliação da AST
            printf("--- Execução do programa concluída ---\n");
        } else {
            fprintf(stderr, "Nenhum código para executar (AST vazia ou erro interno).\n");
        }
    } else {
        printf("Análise sintática falhou. Não foi possível executar o programa.\n");
    }
    

    if (root) {
        treefree(root); // Libera a AST
        root = NULL; // Evita double free
        printf("Memória da AST liberada.\n");
    }

    // Libera as tabelas de símbolos
    free_rvar(rvar);
    rvar = NULL;
    free_ivar(ivar);
    ivar = NULL;
    free_tvar(tvar);
    tvar = NULL;
    free_array_table(array_table);
    array_table = NULL;
    // if (listFunction) {
    //     free_functions(listFunction);
    //     listFunction = NULL;
    // }
    printf("Memória das tabelas de símbolos liberada.\n");

    // Retorna o resultado do parsing. Um retorno 0 indica sucesso geral.
    return parse_result;
}