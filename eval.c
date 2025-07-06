// eval.c
#include "ast.h"
#include "tab_simb.h" // Seus arquivos de tabela de símbolos
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // Para strcmp, strcpy, strdup
#include <math.h>   // Para sqrt, fabs

// Suas tabelas de símbolos globais (declaradas extern em tab_simb.h)
extern VARS *rvar;
extern VARSI *ivar;
extern VARST *tvar;
extern ARRAY_VAR *array_table;

// --- Função de avaliação principal da AST ---
// Retorna um double para a maioria das expressões numéricas.
// Para operações que não retornam valor (comandos), o valor retornado não é usado.
double eval(Ast *a) {
    double v = 0.0; // Valor de retorno para expressões numéricas
    int ival;       // Para valores inteiros intermediários
    char *sval;     // Para valores string intermediários

    if (!a) {
        // Isso pode acontecer para nós opcionais (e.g., else vazio, init vazio no for)
        // Não é necessariamente um erro fatal se o design permitir.
        return 0.0;
    }

    switch (a->nodetype) {
        // --- Literais ---
        case NODE_INT_LIT:
            return (double)a->ivalue;
        case NODE_REAL_LIT:
            return a->rvalue;
        case NODE_STRING_LIT:
            // String literais retornam 0.0 se eval é double.
            // Para strings, você precisará de uma função de print separada ou modificar eval.
            // Por enquanto, retorna 0.0, mas o print em NODE_WRITE usará a->svalue diretamente.
            return 0.0; // Strings não são avaliadas como números, mas podem ser impressas

        // --- Identificadores e Acessos ---
        case NODE_ID:; // Busca o valor de uma variável simples
            // A busca na tabela de símbolos deve ser feita aqui.
            // Retorna o valor numérico da variável.
            // Se for string, talvez retorne 0.0 ou exija um tratamento especial.
            // É melhor ter funções de busca que retornem o tipo e o valor.

            // Exemplo adaptado da sua busca existente:
            VARS *aux_r = srch(rvar, a->svalue);
            if (aux_r) return aux_r->v;

            VARSI *aux_i = srchi(ivar, a->svalue);
            if (aux_i) return (double)aux_i->v;

            VARST *aux_t = srcht(tvar, a->svalue);
            if (aux_t) {
                // Se a variável string contiver um número (ex: "123")
                return atof(aux_t->v);
            }
            fprintf(stderr, "Erro: Variavel '%s' nao declarada.\n", a->svalue);
            exit(1);

        case NODE_ARRAY_ACCESS:; // Acesso a elemento de array
            // a->l é o NODE_ID do array, a->r é a expressão do índice
            char *array_name = a->l->svalue;
            ARRAY_VAR *arr_e = srch_array(array_table, array_name);
            if (!arr_e) {
                fprintf(stderr, "Erro: Array '%s' nao declarado.\n", array_name);
                exit(1);
            }
            int index_e = (int)eval(a->r); // Avalia a expressão do índice
            if (index_e < 0 || index_e >= arr_e->size) {
                fprintf(stderr, "Erro: Indice de array fora dos limites para '%s' (indice %d, tamanho %d).\n", arr_e->name, index_e, arr_e->size);
                exit(1);
            }

            if (arr_e->type == TYPE_INT) {
                return (double)((int*)arr_e->data)[index_e];
            } else if (arr_e->type == TYPE_REAL) {
                return ((double*)arr_e->data)[index_e];
            } else if (arr_e->type == TYPE_STRING) {
                // Se a string contiver um número
                return atof(((char**)arr_e->data)[index_e]);
            }
            return 0.0; // Caso de erro ou tipo desconhecido

        // --- Operadores Unários ---
        case NODE_UMINUS:
            return -eval(a->l);
        // case NODE_NOT: // Se você tiver 'nao' lógico
        //    return (eval(a->l) == 0.0) ? 1.0 : 0.0;

        // --- Operadores Binários Aritméticos ---
        case NODE_ADD: return eval(a->l) + eval(a->r);
        case NODE_SUB: return eval(a->l) - eval(a->r);
        case NODE_MUL: return eval(a->l) * eval(a->r);
        case NODE_DIV:
            if (eval(a->r) == 0.0) {
                fprintf(stderr, "Erro: Divisao por zero.\n");
                exit(1);
            }
            return eval(a->l) / eval(a->r);

        // --- Operadores de Comparação (retornam 1.0 para verdadeiro, 0.0 para falso) ---
        case NODE_GT: return (eval(a->l) > eval(a->r)) ? 1.0 : 0.0;
        case NODE_LT: return (eval(a->l) < eval(a->r)) ? 1.0 : 0.0;
        case NODE_NE: return (eval(a->l) != eval(a->r)) ? 1.0 : 0.0;
        case NODE_EQ: return (eval(a->l) == eval(a->r)) ? 1.0 : 0.0;
        case NODE_GE: return (eval(a->l) >= eval(a->r)) ? 1.0 : 0.0;
        case NODE_LE: return (eval(a->l) <= eval(a->r)) ? 1.0 : 0.0;

        // --- Operadores Lógicos ---
        case NODE_OR: return (eval(a->l) != 0.0 || eval(a->r) != 0.0) ? 1.0 : 0.0;
        case NODE_AND: return (eval(a->l) != 0.0 && eval(a->r) != 0.0) ? 1.0 : 0.0;

        // --- Atribuição e Declaração ---
        case NODE_ASSIGN:; // Atribuição de variável simples (ID = EXPRESSAO)
            // a->l é o NODE_ID da variável, a->r é a expressão do valor
            double assigned_value = eval(a->r);
            char *var_name_assign = a->l->svalue;

            VARS *x_r = srch(rvar, var_name_assign);
            if (x_r) {
                x_r->v = assigned_value;
            } else {
                VARSI *x_i = srchi(ivar, var_name_assign);
                if (x_i) {
                    x_i->v = (int)assigned_value;
                } else {
                    VARST *x_t = srcht(tvar, var_name_assign);
                    if (x_t) {
                        // Atribuição de numérico para string: converter para string
                        // OU, se a->r é NODE_STRING_LIT, copiar a string.
                        if (a->r->nodetype == NODE_STRING_LIT) {
                            strncpy(x_t->v, a->r->svalue, MAX_STR_LEN - 1); // MAX_STR_LEN do seu tab_simb.h
                            x_t->v[MAX_STR_LEN - 1] = '\0';
                        } else {
                            snprintf(x_t->v, MAX_STR_LEN, "%.6g", assigned_value);
                        }
                    } else {
                        fprintf(stderr, "Erro: Variavel '%s' nao declarada para atribuicao.\n", var_name_assign);
                        exit(1);
                    }
                }
            }
            return assigned_value;

        case NODE_ARRAY_ASSIGN:; // Atribuição em array (ID[EXPRESSAO] = EXPRESSAO)
            // a->l é o NODE_ARRAY_ACCESS, a->r é a expressão do valor
            Ast *array_access_node = a->l; // O lado esquerdo é um nó de acesso a array
            char *arr_name_assign = array_access_node->l->svalue; // Nome do array

            ARRAY_VAR *arr_assign = srch_array(array_table, arr_name_assign);
            if (!arr_assign) {
                fprintf(stderr, "Erro: Array '%s' nao declarado para atribuicao.\n", arr_name_assign);
                exit(1);
            }
            int index_assign = (int)eval(array_access_node->r); // Avalia o índice
            if (index_assign < 0 || index_assign >= arr_assign->size) {
                fprintf(stderr, "Erro: Indice de array fora dos limites para '%s' (indice %d, tamanho %d).\n", arr_assign->name, index_assign, arr_assign->size);
                exit(1);
            }
            double val_assign = eval(a->r); // Avalia o valor a ser atribuído

            if (arr_assign->type == TYPE_INT) {
                ((int*)arr_assign->data)[index_assign] = (int)val_assign;
            } else if (arr_assign->type == TYPE_REAL) {
                ((double*)arr_assign->data)[index_assign] = val_assign;
            } else if (arr_assign->type == TYPE_STRING) {
                // Atribuição de string em array de strings
                char **str_array_data = (char**)arr_assign->data;
                if (str_array_data[index_assign] != NULL) {
                    free(str_array_data[index_assign]); // Libera a string antiga
                }
                if (a->r->nodetype == NODE_STRING_LIT) { // Se o valor é um literal de string
                    str_array_data[index_assign] = strdup(a->r->svalue);
                } else { // Se o valor é numérico, converte para string
                    char temp_str[256];
                    snprintf(temp_str, sizeof(temp_str), "%.6g", val_assign);
                    str_array_data[index_assign] = strdup(temp_str);
                }
            }
            return val_assign; // Retorna o valor atribuído

        case NODE_VAR_DECL:; // Declaração de variável (int/real/string ID [= valor])
            char *decl_id = a->l->svalue; // Nome da variável
            VarType decl_type = a->type;  // Tipo da variável (INT, REAL, STRING)

            if (varexiste(decl_id)) {
                fprintf(stderr, "Erro de redeclaracao: variavel '%s' ja existe.\n", decl_id);
                exit(1);
            }

            if (decl_type == TYPE_INT) {
                ivar = insi(ivar, decl_id);
                if (a->r != NULL) { // Se há valor inicial
                    srchi(ivar, decl_id)->v = (int)eval(a->r);
                }
            } else if (decl_type == TYPE_REAL) {
                rvar = insr(rvar, decl_id);
                if (a->r != NULL) { // Se há valor inicial
                    srch(rvar, decl_id)->v = eval(a->r);
                }
            } else if (decl_type == TYPE_STRING) {
                tvar = inst(tvar, decl_id);
                if (a->r != NULL) { // Se há valor inicial (espera-se NODE_STRING_LIT)
                    if (a->r->nodetype == NODE_STRING_LIT) {
                        strncpy(srcht(tvar, decl_id)->v, a->r->svalue, MAX_STR_LEN - 1);
                        srcht(tvar, decl_id)->v[MAX_STR_LEN - 1] = '\0';
                    } else {
                        fprintf(stderr, "Erro: Nao eh possivel atribuir valor nao-string a variavel string '%s'.\n", decl_id);
                        exit(1);
                    }
                }
            }
            return 0.0; // Declarações não retornam valor numérico

        case NODE_ARRAY_DECL:; // Declaração de array (array tipo ID [tamanho])
            char *array_decl_name = a->l->svalue; // Nome do array
            VarType array_decl_type = a->type;    // Tipo base do array (TYPE_INT, TYPE_REAL, TYPE_STRING)
            int array_decl_size = (int)eval(a->r); // Avalia a expressão do tamanho

            if (varexiste(array_decl_name)) { // Sua função 'varexiste' precisa verificar em todas as tabelas
                fprintf(stderr, "Erro de redeclaracao: Array '%s' ja existe.\n", array_decl_name);
                exit(1);
            }
            if (array_decl_size <= 0) {
                fprintf(stderr, "Erro: Tamanho de array '%s' deve ser positivo (tamanho %d).\n", array_decl_name, array_decl_size);
                exit(1);
            }
            array_table = ins_array(array_table, array_decl_name, array_decl_type, array_decl_size);
            return 0.0;

        // --- Comandos de E/S ---
        case NODE_READ:; // Comando LER
            // a->l é o NODE_ID ou NODE_ARRAY_ACCESS que será lido
            Ast *target_node = a->l;
            if (target_node->nodetype == NODE_ID) { // LER variavel simples
                char *read_var_name = target_node->svalue;
                VARS *xcr = srch(rvar, read_var_name);
                if (xcr) {
                    scanf("%lf", &xcr->v);
                } else {
                    VARSI *xci = srchi(ivar, read_var_name);
                    if (xci) {
                        scanf("%d", &xci->v);
                    } else {
                        VARST *xct = srcht(tvar, read_var_name);
                        if (xct) {
                            // Limpa o buffer de entrada antes de ler a string
                            int c; while ((c = getchar()) != '\n' && c != EOF);
                            if (fgets(xct->v, MAX_STR_LEN, stdin) != NULL) {
                                xct->v[strcspn(xct->v, "\n")] = '\0'; // Remove newline
                            } else {
                                fprintf(stderr, "Erro na leitura de string.\n");
                                exit(1);
                            }
                        } else {
                            fprintf(stderr, "Erro: Variavel '%s' invalida para leitura ou nao declarada.\n", read_var_name);
                            exit(1);
                        }
                    }
                }
            } else if (target_node->nodetype == NODE_ARRAY_ACCESS) { // LER elemento de array
                char *arr_name_read = target_node->l->svalue;
                ARRAY_VAR *arr_read = srch_array(array_table, arr_name_read);
                if (!arr_read) {
                    fprintf(stderr, "Erro: Array '%s' nao declarado para leitura.\n", arr_name_read);
                    exit(1);
                }
                int index_read = (int)eval(target_node->r);
                if (index_read < 0 || index_read >= arr_read->size) {
                    fprintf(stderr, "Erro: Indice de array fora dos limites para '%s' na leitura.\n", arr_read->name);
                    exit(1);
                }

                if (arr_read->type == TYPE_INT) {
                    scanf("%d", &((int*)arr_read->data)[index_read]);
                } else if (arr_read->type == TYPE_REAL) {
                    scanf("%lf", &((double*)arr_read->data)[index_read]);
                } else if (arr_read->type == TYPE_STRING) {
                    char **str_array_data = (char**)arr_read->data;
                    if (str_array_data[index_read] != NULL) {
                        free(str_array_data[index_read]);
                    }
                    char buffer[MAX_STR_LEN]; // Buffer temporário para leitura
                    int c; while ((c = getchar()) != '\n' && c != EOF); // Limpa o buffer
                    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
                        buffer[strcspn(buffer, "\n")] = '\0';
                        str_array_data[index_read] = strdup(buffer);
                    } else {
                        fprintf(stderr, "Erro na leitura de string para array.\n");
                        exit(1);
                    }
                }
            } else {
                fprintf(stderr, "Erro: Argumento invalido para LER.\n");
                exit(1);
            }
            return 0.0;

        case NODE_WRITE:; // Comando ESCREVER
            // O nó NODE_WRITE em si tem apenas um item de escrita em a->l
            // A lista de argumentos é construída por NODE_LIST no compilador.y
            // Se o write_statement é `ESCREVER write_args`, e write_args cria NODE_LIST,
            // então `eval(root)` em `main` vai chamar `eval(NODE_LIST)` que chama `eval` recursivamente.
            // Aqui, `a->l` é o `write_item`.

            Ast *item_to_print = a->l;
            if (!item_to_print) {
                // Isso pode acontecer se houver um `escrever` sem argumentos, embora a gramática não permita
                return 0.0;
            }

            if (item_to_print->nodetype == NODE_STRING_LIT) {
                fprintf(stdout, "%s", item_to_print->svalue);
            } else if (item_to_print->nodetype == NODE_ID) {
                char *print_var_name = item_to_print->svalue;
                VARS *auxn_r = srch(rvar, print_var_name);
                if (auxn_r) {
                    fprintf(stdout, "%.2f", auxn_r->v);
                } else {
                    VARSI *auxn_i = srchi(ivar, print_var_name);
                    if (auxn_i) {
                        fprintf(stdout, "%d", auxn_i->v);
                    } else {
                        VARST *auxn_t = srcht(tvar, print_var_name);
                        if (auxn_t) {
                            fprintf(stdout, "%s", auxn_t->v);
                        } else {
                            fprintf(stderr, "Erro: Variavel '%s' nao declarada para impressao.\n", print_var_name);
                            exit(1);
                        }
                    }
                }
            } else if (item_to_print->nodetype == NODE_ARRAY_ACCESS) {
                char *arr_name_print = item_to_print->l->svalue;
                ARRAY_VAR *arr_print = srch_array(array_table, arr_name_print);
                if (!arr_print) {
                    fprintf(stderr, "Erro: Array '%s' nao declarado para impressao.\n", arr_name_print);
                    exit(1);
                }
                int index_print = (int)eval(item_to_print->r);
                if (index_print < 0 || index_print >= arr_print->size) {
                    fprintf(stderr, "Erro: Indice de array fora dos limites para '%s' na impressao.\n", arr_print->name);
                    exit(1);
                }

                if (arr_print->type == TYPE_INT) {
                    fprintf(stdout, "%d", ((int*)arr_print->data)[index_print]);
                } else if (arr_print->type == TYPE_REAL) {
                    fprintf(stdout, "%.2f", ((double*)arr_print->data)[index_print]);
                } else if (arr_print->type == TYPE_STRING) {
                    fprintf(stdout, "%s", ((char**)arr_print->data)[index_print]);
                }
            } else { // Para expressões que resultam em número (int/real literal, ou operação)
                fprintf(stdout, "%.2f\n", eval(item_to_print)); // Avalia e imprime o resultado numérico
            }
            return 0.0;


        // --- Comandos de Controle de Fluxo ---
        case NODE_LIST: // Lista de instruções (statements_block ou write_args)
            eval(a->l); // Executa o primeiro item na lista
            v = eval(a->r); // Recursivamente avalia o resto da lista. O último eval retorna 0.0.
            return v;

        case NODE_IF:; // If / If-Else
            if (eval(a->l) != 0.0) { // a->l é a condição
                eval(a->r); // a->r é o corpo THEN
            } else {
                eval(a->m); // a->m é o corpo ELSE (pode ser NODE_EMPTY)
            }
            return 0.0; // Comandos não retornam valor útil

        case NODE_WHILE:; // While
            while (eval(a->l) != 0.0) { // a->l é a condição
                eval(a->r); // a->r é o corpo
            }
            return 0.0;

        case NODE_FOR:; // For
            // a->l = inicializacao, a->r = condicao, a->m = iteracao, a->e = corpo
            eval(a->l); // Inicialização
            while (eval(a->r) != 0.0) { // Condição
                eval(a->e); // Corpo do loop
                eval(a->m); // Iteração
            }
            return 0.0;

        // --- Funções Embutidas ---
        case NODE_SQRT: return sqrt(eval(a->l));
        case NODE_ABS:  return fabs(eval(a->l));

        case NODE_EMPTY: // Nó vazio, não faz nada
            return 0.0;

        default:
            fprintf(stderr, "Tipo de no AST desconhecido encontrado: %d\n", a->nodetype);
            exit(1);
    }
    return v;
}