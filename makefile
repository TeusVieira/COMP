all: compilador.l compilador.y
	@clear
	flex compilador.l
	bison -d -v compilador.y
	gcc -o cuida lex.yy.c compilador.tab.c tab_simb.c ast.c eval.c main.c -lm

