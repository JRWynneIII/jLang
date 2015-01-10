SRC= handler.cpp jlang.tab.cpp lex.yy.c ast.cpp 
all: jlangc

jlangc: ${SRC} 
	/usr/bin/clang++ -g ${SRC} `llvm-config --cppflags --ldflags --libs core jit native bitwriter` -O3 -o jlangc

lex.yy.c: lexer.l
	flex lexer.l

jlang.tab.cpp: jlang.ypp
	bison -d jlang.ypp
clean:
	rm -rf lex.yy.c jlangc jlang.tab.cpp jlang.tab.h
