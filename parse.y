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
  int integer;
  double doubleVal;
  char* str;
}

%token NUMBER STRING
%token PRINTLN
%token SEMICOLON 
%token PLUS MINUS DIV MUL EQUAL
%token VAR FUNC
%token RARROW DOT
%token LBRACE RBRACE
%token RPAREN LPAREN
%token ID 

%type <doubleVal> NUMBER 
%type <str> STRING
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
%type <str> PRINTLN

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
          | funcPrototype

binOps: binOps binOp
      | binOp

variableDefs: variableDefs variableDef
            | variableDef

block: binOps
     | variableDefs


variableDef: VAR ID SEMICOLON { printf("Variable defined\n"); storeVar($2,0.0);}
           | VAR ID EQUAL NUMBER SEMICOLON { printf("Variable defined %s is equal to %lf\n", $2, $4); storeVar($2,$4);}
           | VAR ID EQUAL binOp SEMICOLON 
           | VAR ID EQUAL STRING  SEMICOLON { storeStringVar($2,$4); } 

funcPrototype: FUNC ID LPAREN expressions RPAREN RARROW ID SEMICOLON { std::cout << "Function prototype with arguments!\n"; }
             | FUNC ID LPAREN RPAREN RARROW ID SEMICOLON { std::cout << "Functoin proto without arguments\n"; }

funcDef: FUNC ID LPAREN expressions RPAREN RARROW ID LBRACE block RBRACE
       | FUNC ID LPAREN RPAREN RARROW ID LBRACE block RBRACE

printFunc: PRINTLN ID SEMICOLON  { std::string temp = $2; std::cout << varTable[temp] << std::endl; }


binOp: NUMBER PLUS NUMBER SEMICOLON { printf("%lf\n", $1 + $3); }
     | NUMBER MINUS NUMBER SEMICOLON { printf("%lf\n", $1 - $3); }
     | NUMBER DIV NUMBER SEMICOLON { printf("%lf\n", $1 / $3); }
     | NUMBER MUL NUMBER SEMICOLON { printf("%lf\n", $1*$3); }
