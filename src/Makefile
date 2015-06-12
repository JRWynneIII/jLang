SRC= classes.cpp handler.cpp jlang.tab.cpp lex.yy.c vars.cpp loops.cpp functions.cpp ops.cpp main.cpp 
all: jlangc

jlangc: ${SRC} 
	clang++ -pthread -w -g -std=c++11 -I${HOME}/lib/boost/boost_1_58_0/ ${SRC} `llvm-config --cppflags --ldflags --libs core mcjit native bitwriter --system-libs` -O0 -o jlangc

lex.yy.c: lexer.l
	flex lexer.l

jlang.tab.cpp: jlang.ypp
	bison -d jlang.ypp 
clean:
	rm -rf lex.yy.c jlangc jlang.tab.cpp jlang.tab.hpp module.tab.cpp module.tab.hpp
