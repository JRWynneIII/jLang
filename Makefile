SRC= handler.cpp jlang.tab.c lex.yy.c ast.cpp 
all: jlangc

jlangc: ${SRC} 
	clang++ -g ${SRC} `llvm-config --cppflags --ldflags --libs core jit native` -O3 -o jlangc

lex.yy.c: lexer.l
	flex lexer.l

jlang.tab.c: jlang.y
	bison -d jlang.y
clean:
	rm -rf lex.yy.c jlangc jlang.tab.c jlang.tab.h
