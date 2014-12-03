%{
#define VTYPE(type, value) printf("%s(%s)\n",type,value); 
#define TYPE(type) printf("%s\n",type)
#include <iostream>
#include "parse.tab.h"
using namespace std;
%}
NUMBER [0-9]+
%option noyywrap
%%
#.*$ {}
{NUMBER}(\.{NUMBER}|(\.{NUMBER})?[eE][+-]?{NUMBER}) { VTYPE("FLOAT", yytext); }
{NUMBER} { yylval = atoi(yytext); return NUMBER; }
[\t ] {}
"func" { return FUNC; }
"var" { return VAR; }
[a-z_][a-zA-Z0-9_]* { yylval.str_val = new string(yytext); return ID; }
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
"(" { TYPE("LPAREN"); }
")" { TYPE("RPAREN"); }
"{" { return LBRACE; }
"}" { return RBRACE; }
"@" { TYPE("AT"); }
"." { TYPE("DOT"); }
"," { TYPE("COMMA"); }
":" { TYPE("COLON"); }
";" { return SEMICOLON; }
\"([^"]|\\.)*\" { VTYPE("STRING", yytext); }
\'([^']|\\.)*\' { VTYPE("STRING", yytext); }
[A-Z][a-zA-Z0-9_]* { VTYPE("CONSTANT", yytext); }

\n {}
. { fprintf(stderr, "Unknown token '%s'\n", yytext); }
