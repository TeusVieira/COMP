all: calc.l calc.y
	@clear
	bison -d calc.y
	flex -i calc.l
	gcc calc.tab.c lex.yy.c -o compilador -lfl -lm


