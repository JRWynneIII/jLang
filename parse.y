%{
#include<stdio.h>
#include<iostream>
#include<map>
std::map<std::string,double> varTable;
extern int yylex(void);
extern void storeVar(char*,double);
extern int lineNum;
void yyerror(char const *s) { fprintf(stderr,"ERROR: %s in line %d\n", s, lineNum); }
%}

%union
{
  int integer;
  double doubleVal;
  char* str;
}

%token NUMBER 
%token PRINTLN
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
%token RPAREN
%token LPAREN
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
%type <str> LPAREN
%type <str> RPAREN
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
          | printFunc
          | NUMBER

binOps: binOps binOp
      | binOp

variableDefs: variableDefs variableDef
            | variableDef

block: binOps
     | variableDefs


variableDef: VAR ID SEMICOLON { printf("Variable defined\n"); storeVar($2,0.0);}
           | VAR ID EQUAL NUMBER SEMICOLON { printf("Variable defined %s is equal to %lf\n", $2, $4); storeVar($2,$4);}
           | VAR ID EQUAL binOp SEMICOLON 
          
printFunc: PRINTLN ID SEMICOLON { std::string temp = $2; std::cout << varTable[temp] << std::endl; }
funcDef: FUNC ID '(' expressions ')' RARROW ID SEMICOLON
       | FUNC ID '(' ')' RARROW ID SEMICOLON
       | FUNC ID '(' expressions ')' RARROW ID SEMICOLON LBRACE block RBRACE
       | FUNC ID '(' ')' RARROW ID SEMICOLON LBRACE block RBRACE

binOp: NUMBER PLUS NUMBER SEMICOLON { printf("%lf\n", $1 + $3); }
     | NUMBER MINUS NUMBER SEMICOLON { printf("%lf\n", $1 - $3); }
     | NUMBER DIV NUMBER SEMICOLON { printf("%lf\n", $1 / $3); }
     | NUMBER MUL NUMBER SEMICOLON { printf("%lf\n", $1*$3); }

