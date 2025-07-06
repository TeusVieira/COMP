all: calc.l calc.y
	@clear
	bison -d calc.y
	flex calc.l
	gcc -o calc lex.yy.c calc.tab.c ast.c -lfl


