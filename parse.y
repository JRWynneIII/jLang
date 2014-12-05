%{
#include<stdio.h>
extern int yylex(void);
void yyerror(char const *s) { fprintf(stderr, "%s\n",s); }
%}

%union
{
  int integer;
  double doubleVal;
  char* str;
}

%token NUMBER 
%token SEMICOLON 
%token PLUS
%token MINUS
%token DIV 
%token MUL
%token DOT 
%token EQUAL
%token VAR 
%token FUNC 
%token RARROW
%token LBRACE 
%token RBRACE 
%token ID 

%type <doubleVal> NUMBER 
%type <str> SEMICOLON
%type <str> PLUS
%type <str> MINUS
%type <str> DIV
%type <str> MUL
%type <str> DOT
%type <str> EQUAL
%type <str> VAR
%type <str> FUNC
%type <str> RARROW
%type <str> LBRACE
%type <str> RBRACE
%type <str> ID

%start program

%%
program: expressions

expressions: expressions expression
           | expression

expression: binOp 
          | variableDef 
          | funcDef
          | NUMBER

variableDef: VAR ID SEMICOLON { printf("Variable defined\n"); }
           | VAR ID EQUAL NUMBER SEMICOLON { printf("Variable defined %s is equal to %lf\n", $2, $4); }
          
funcDef: FUNC '(' expressions ')' SEMICOLON
       | FUNC '(' expressions ')' LBRACE expressions RBRACE

binOp: NUMBER PLUS NUMBER SEMICOLON { printf("%lf\n", $1 + $3); }
     | NUMBER MINUS NUMBER SEMICOLON { printf("%lf\n", $1 - $3); }
     | NUMBER DIV NUMBER SEMICOLON { printf("%lf\n", $1 / $3); }
     | NUMBER MUL NUMBER SEMICOLON { printf("%lf\n", $1*$3); }

