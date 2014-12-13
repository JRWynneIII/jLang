SRC=main.c jcode.tab.c lex.yy.c
all: jlangc

jlangc: ${SRC}
	g++ -o jlangc ${SRC}

lex.yy.c: lexer.l
	flex lexer.l

jcode.tab.c: jcode.y
	bison -d jcode.y
clean:
	rm -rf lex.yy.c jlangc jcode.tab.c jcode.tab.h
