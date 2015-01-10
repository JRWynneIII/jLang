%{
#define VTYPE(type, value) printf("%s(%s)\n",type,value); 
#define TYPE(type) printf("%s\n",type)
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "jlang.tab.hpp"
#include "tree.h"
using namespace std;
int lineNum = 1;
%}
NUMBER [0-9]+
%option noyywrap
%%
#.*$ {}
{NUMBER}(\.{NUMBER}|(\.{NUMBER})?[eE][+-]?{NUMBER}) { yylval.doubleVal = atof(yytext); return FLOAT; }
{NUMBER} { yylval.intVal = atof(yytext); return NUMBER; }
[\t ] {}
"module" { return MODULE; }
"func" { return FUNC; }
"extern" { return EXTERN; }
"int" { return INT; }
"double" { return DOUBLE; }
"char" { return CHAR; }
"string" { return STR; }
"kernel" { return KERNEL; }
"for" { return FOR; }
"if" { return IF; }
"else" { return ELSE; }
"elif" { return ELIF; }
'^' { return CAROT; }
"->" { return RARROW; }
"<-" { return LARROW; }
"=" { return EQUAL; }
">" { return GT; }
"<" { return LT; }
">=" { return GTE; }
"<=" { return LTE; }
"!=" { return NEQUAL; }
"+" { return PLUS; }
"-" { return MINUS; }
"*" { return MUL; }
"/" { return DIV; }
"%" { return MOD; }
"!" { return EMARK; }
"?" { return QMARK; }
"&" { return AND; }
"|" { return OR; }
"[" { return LSBRACE; }
"]" { return RSBRACE; }
"(" { return LPAREN; }
")" { return RPAREN; }
"{" { return LBRACE; }
"}" { return RBRACE; }
"@" { return AT; }
"." { return DOT; }
"," { return COMMA; }
":" { return COLON; }
";" { return SEMICOLON; }
\'[a-zA-Z]\' { yylval.charVal = *yytext; return CHARACTER; }
[a-z_][a-zA-Z0-9_]* { yylval.strVal = strdup(yytext); return ID; }
\"([^"]|\\.)*\" { yylval.strVal = strdup(yytext); return STRING; }

\n {lineNum++; /*return END;*/ }
. { fprintf(stderr, "Unknown token '%s'\n", yytext); }
