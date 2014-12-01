%{
#define VTYPE(type, value) printf("%s(%s)\n",type,value)
#define TYPE(type) printf("%s\n",type)
#include <stdlib.h>
#include <stdio.h>
#include "parse.tab.h"
%}
NUMBER [0-9]+
%option noyywrap
%%
#.*$ {}
{NUMBER}(\.{NUMBER}|(\.{NUMBER})?[eE][+-]?{NUMBER}) { VTYPE("FLOAT", yytext); }
{NUMBER} { VTYPE("NUMBER",yytext); }
[a-z_][a-zA-Z0-9_]* { VTYPE("ID", yytext); }
[\t ] {}
"=" { TYPE("EQUAL"); }
">" { TYPE("GT"); }
"<" { TYPE("LT"); }
">=" { TYPE("GTE"); }
"<=" { TYPE("LTE"); }
"!=" { TYPE("NEQUAL"); }
"+" { TYPE("PLUS"); }
"-" { TYPE("MINUS"); }
"*" { TYPE("MULT"); }
"/" { TYPE("DIV"); }
"%" { TYPE("MOD"); }
"!" { TYPE("EMARK"); }
"?" { TYPE("QMARK"); }
"&" { TYPE("AND"); }
"|" { TYPE("OR"); }
"[" { TYPE("LSBRACE"); }
"]" { TYPE("RSBRACE"); }
"(" { TYPE("LPAREN"); }
")" { TYPE("RPAREN"); }
"{" { TYPE("LBRACE"); }
"}" { TYPE("RBRACE"); }
"@" { TYPE("AT"); }
"." { TYPE("DOT"); }
"," { TYPE("COMMA"); }
":" { TYPE("COLON"); }
";" { TYPE("SEMICOLON"); }
\"([^"]|\\.)*\" { VTYPE("STRING", yytext); }
\'([^']|\\.)*\' { VTYPE("STRING", yytext); }
[A-Z][a-zA-Z0-9_]* { VTYPE("CONSTANT", yytext); }

\n {}
. { fprintf(stderr, "Unknown token '%s'\n", yytext); }
%%

int main(int argc, char* argv[])
{
  if (argc > 1)
    yyin = fopen(argv[1], "r");
  yyparse();
  return EXIT_SUCCESS;
}
