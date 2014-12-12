%{
#include<stdio.h>
#include<iostream>
#include<map>
std::map<std::string,double> varTable;
std::map<std::string,std::string> varStringTbl;
extern int yylex(void);
extern void storeVar(char*,double);
extern void storeStringVar(char*,char*);
extern int lineNum;
void yyerror(char const *s) { fprintf(stderr,"ERROR: %s in line %d\n", s, lineNum); }
%}

%union
{
  int intVal;
  char charVal;
  double doubleVal;
  char* strVal;
}

%token NUMBER FLOAT
%token STRING ID
%token FUNC KERNEL FOR IF ELSE ELIF
%token INT DOUBLE CHAR STR
%token RARROW LARROW CAROT
%token EQUAL GT LT GTE LTE NEQUAL PLUS MINUS MUL DIV MOD EMARK QMARK AND OR LSBRACE RSBRACE LSPAREN RSPAREN LBRACE RBRACE AT DOT COMMA COLON SEMICOLON

%type <intVal> NUMBER
%type <doubleVal> DOUBLE
%type <str> CAROT
%type <str> AT
%type <str> CHAR
%type <str> STR 
%type <str> STRING
%type <str> SEMICOLON
%type <str> PLUS
%type <str> MINUS
%type <str> DIV
%type <str> MUL
%type <str> DOT
%type <str> EQUAL
%type <str> FUNC
%type <str> KERNEL
%type <str> RARROW
%type <str> LARROW
%type <str> LBRACE
%type <str> LPAREN
%type <str> RPAREN
%type <str> RBRACE
%type <str> ID

%start program

%%
program: expressions

expressions: expressions expression
           | expression


