%{
#define VTYPE(type, value) printf("%s(%s)\n",type,value); 
#define TYPE(type) printf("%s\n",type)
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "parse.tab.h"
using namespace std;
int lineNum = 1;
%}
NUMBER [0-9]+
%option noyywrap
%%
#.*$ {}
{NUMBER}(\.{NUMBER}|(\.{NUMBER})?[eE][+-]?{NUMBER}) { VTYPE("FLOAT", yytext); }
{NUMBER} { yylval.doubleVal = atof(yytext); return NUMBER; }
[\t ] {}
"func" { return FUNC; }
"var" { return VAR; }
[a-z_][a-zA-Z0-9_]* { yylval.str = strdup(yytext); return ID; }
"->" { return RARROW; }
"=" { return EQUAL; }
">" { TYPE("GT"); }
"<" { TYPE("LT"); }
">=" { TYPE("GTE"); }
"<=" { TYPE("LTE"); }
"!=" { TYPE("NEQUAL"); }
"+" { return PLUS; }
"-" { return MINUS; }
"*" { return MUL; }
"/" { return DIV; }
"%" { TYPE("MOD"); }
"!" { TYPE("EMARK"); }
"?" { TYPE("QMARK"); }
"&" { TYPE("AND"); }
"|" { TYPE("OR"); }
"[" { TYPE("LSBRACE"); }
"]" { TYPE("RSBRACE"); }
"(" { return LPAREN; }
")" { return RPAREN; }
"{" { return LBRACE; }
"}" { return RBRACE; }
"@" { TYPE("AT"); }
"." { TYPE("DOT"); }
"," { TYPE("COMMA"); }
":" { TYPE("COLON"); }
";" { return SEMICOLON; }
"println" { return PRINTLN; }
\"([^"]|\\.)*\" { VTYPE("STRING", yytext); }
\'([^']|\\.)*\' { VTYPE("STRING", yytext); }
[A-Z][a-zA-Z0-9_]* { VTYPE("CONSTANT", yytext); }

\n {lineNum++;}
. { fprintf(stderr, "Unknown token '%s'\n", yytext); }
