SRC= jcode.tab.c lex.yy.c ast.cpp 
all: jlangc

jlangc: ${SRC} 
	clang++ -g ${SRC} `llvm-config --cppflags --ldflags --libs core jit native` -O3 -o jlangc

lex.yy.c: lexer.l
	flex lexer.l

jcode.tab.c: jcode.y
	bison -d jcode.y
clean:
	rm -rf lex.yy.c jlangc jcode.tab.c jcode.tab.h
