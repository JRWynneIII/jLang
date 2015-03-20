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
YY_BUFFER_STATE getCurBuf() { return YY_CURRENT_BUFFER; }
int lineNum = 1;
%}
%option noyywrap
%option header-file="lex.h"
NUMBER [0-9]+
%%
#.*$ {}
{NUMBER}(\.{NUMBER}|(\.{NUMBER})?[eE][+-]?{NUMBER}) { yylval.doubleVal = atof(yytext); return FLOAT; }
{NUMBER} { yylval.intVal = atof(yytext); return NUMBER; }
[\t ] {}
"return" { return RETURN; }
"import" { return IMPORT; }
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
"^" { return CAROT; }
"->" { return RARROW; }
"<-" { return LARROW; }
"=" { return EQUAL; }
"==" { return COMPEQUAL; }
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
'\\n' { yylval.charVal = '\n'; return CHARACTER; }
'*' { yylval.charVal = '*'; return CHARACTER; }
\'[a-zA-Z]\' { yylval.charVal = yytext[1]; return CHARACTER; }
[a-z_][a-zA-Z0-9_]* { yylval.strVal = strdup(yytext); return ID; }
\"([^"]|\\.)*\" { 
  int len = strlen(yytext);
  char st[len-2];
  int counter = 0;
  for (int i = 0; i<len; i++)
  {
    if (yytext[i] != '"')
    { 
      st[counter] = yytext[i];
      counter++;
    }
  }
  yylval.strVal = strdup(st);
  return STRING; 
}

\n {lineNum++; /*return END;*/ }
. { fprintf(stderr, "Unknown token '%s'\n", yytext); }
