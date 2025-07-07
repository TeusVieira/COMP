

-----

# CUIDA: Uma Linguagem de Programação Simples e Direta

-----

CUIDA é uma linguagem de programação didática e direta, projetada para ser simples de entender e usar. Com uma sintaxe clara e intuitiva, CUIDA é ideal para quem está começando no mundo da programação, permitindo focar nos conceitos fundamentais sem a complexidade de linguagens mais robustas.

## Recursos Principais

  * **Tipos de Dados Básicos:** Suporte para **inteiros (`int`)**, **números reais (`real`)** e **texto (`string`)**.
  * **Declaração e Atribuição:** Sintaxe simples para declarar variáveis e atribuir valores a elas.
  * **Operações Aritméticas:** Realize cálculos básicos como adição, subtração, multiplicação e divisão.
  * **Operações Lógicas:** Suporte a comparações (`==`, `!=`, `<`, `>`, `<=`, `>=`) e operadores lógicos (`E` para AND, `OU` para OR) para criar condições complexas.
  * **Entrada e Saída de Dados:** Use `ler` para obter entrada do usuário e `escrever` para exibir informações no console.
  * **Estruturas Condicionais:** Implemente tomadas de decisão com a estrutura `se...entao...senao...fimse`, permitindo que seu programa execute diferentes blocos de código com base em condições.

## Exemplo de Código CUIDA

Aqui está um pequeno exemplo para você ter uma ideia da sintaxe de CUIDA:

```cui
// Declaração de variáveis
int idade = 30;
real altura = 1.75;
string nome = "Alice";

// Operações aritméticas
real preco_unitario = 15.50;
int quantidade = 3;
real total = preco_unitario * (quantidade + 2);

// Exibição de resultados
escrever "Olá, meu nome é "; escrever nome; escrever "!"; escrever "\n";
escrever "Sua compra deu: "; escrever total; escrever "\n";

// Tomada de decisão
se (total > 50.00) entao
    escrever "Você gastou bastante hoje!\n"
senao
    escrever "Sua compra foi econômica.\n"
fimse
```

## Como Usar (Compilar e Executar)

Para compilar e executar programas escritos em CUIDA, você precisará do compilador (`cuida`).

### Pré-requisitos

  * `flex` (ou `lex`)
  * `bison` (ou `yacc`)
  * Um compilador C (como `gcc`)

### Passos para Compilar

1.  **Limpe arquivos gerados anteriormente (se existirem):**
    ```bash
    rm -f lex.yy.c compilador.tab.c compilador.tab.h compilador.output cuida
    ```
2.  **Gere o analisador léxico com `flex`:**
    ```bash
    flex compilador.l
    ```
3.  **Gere o analisador sintático com `bison`:**
    ```bash
    bison -d compilador.y
    ```
    *(Certifique-se de que `compilador.y` e `compilador.l` estão no mesmo diretório.)*
4.  **Compile o compilador CUIDA:**
    ```bash
    gcc -o cuida lex.yy.c compilador.tab.c ast.c eval.c main.c -lfl -lm
    ```
    *(Assumindo que `ast.c`, `eval.c` e `main.c` são os arquivos-fonte do seu projeto. Ajuste conforme necessário.)*

### Como Executar um Programa CUIDA

Com o executável `cuida` compilado, você pode rodar seus arquivos `.cui` (ou `.ml`, como nos exemplos anteriores) assim:

```bash
./cuida seu_programa.cui
```

Substitua `seu_programa.cui` pelo nome do arquivo que contém seu código CUIDA.


CUIDA é um projeto educacional 

-----

