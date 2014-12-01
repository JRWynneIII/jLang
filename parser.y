%{
#include<stdio.h>
extern int yylex(void);
void yyerror(char const *s) { fprintf(stderr, "%s\n",s); }
%}

%token tNUMBER
%start program

%%
program: expressions

expressions: expressions expression
           | expression

expression: tNumber { printf("Parsed(%d)\n", $1); }

