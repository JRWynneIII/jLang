SRC=main.c parse.tab.c lex.yy.c
all: comp

comp: ${SRC}
	cc -o comp ${SRC}

lex.yy.c: grammar.l
	flex grammar.l

parse.tab.c: parse.y
	bison -d parse.y
clean:
	rm -rf lex.yy.c comp parse.tab.c parse.tab.h
