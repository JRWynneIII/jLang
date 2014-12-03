%{
#include<iostream>
#include<map>
#include<string>
#include<cstdlib>
using namespace std;
extern int yylex(void);
void yyerror(char const *s) { fprintf(stderr, "%s\n",s); }
map<string,double> vars;
%}

%union
{
  int int_val;
  double double_val;
  string* str_val;
}

%token NUMBER <double_val>
%token SEMICOLON <int_val>
%token PLUS <int_val>
%token MINUS<int_val>
%token DIV <int_val>
%token MUL <int_val>
%token DOT <int_val>
%token EQUAL <int_val>
%token VAR <str_val>
%token FUNC <str_val>
%token RARROW <int_val>
%token LBRACE <int_val>
%token RBRACE <int_val>
%token ID <str_val>

%type <double_val> variableDef NUMBER expression
%type <str_val> ID 
%type <double_val> binOp 

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
           | VAR ID EQUAL expression SEMICOLON { printf("Variable defined(var:%s is equal to %d\n",*$2,$4); }
          
funcDef: FUNC '(' expressions ')' SEMICOLON
       | FUNC '(' expressions ')' LBRACE expressions RBRACE

binOp: NUMBER PLUS NUMBER SEMICOLON { printf("%d\n", $1 + $3); }
     | NUMBER MINUS NUMBER SEMICOLON { printf("%d\n", $1 - $3); }
     | NUMBER DIV NUMBER SEMICOLON { printf("%d\n", $1 / $3); }
     | NUMBER MUL NUMBER SEMICOLON { printf("%d\n", $1*$3); }

