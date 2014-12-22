%{
#include<stdio.h>
#include<iostream>
#include<map>
#include<vector>
#include "tree.h"
std::map<std::string,double> varTable;
std::map<std::string,std::string> varStringTbl;
std::vector<VarInitExprAST*> args;
extern int yylex(void);
extern void storeVar(char*,double);
extern void storeStringVar(char*,char*);
extern int lineNum;
using namespace std;
void yyerror(char const *s) 
{ 
  cerr << "\033[31m ERROR: \033[37m" << s << " in line: " << lineNum << endl;
//  fprintf(stderr,"ERROR: %s in line %d\n", s, lineNum); 
}
%}

%require "3.0"
%error-verbose

%union
{
  struct
  {
    int intVal;
    char charVal;
    double doubleVal;
    char* strVal;
  };
};

%token NUMBER FLOAT
%token STRING ID END
%token FUNC KERNEL EXTERN FOR IF ELSE ELIF MODULE
%token INT DOUBLE CHAR STR
%token RARROW LARROW CAROT
%token EQUAL GT LT GTE LTE NEQUAL PLUS MINUS MUL DIV MOD EMARK QMARK AND OR LSBRACE RSBRACE LPAREN RPAREN LBRACE RBRACE AT DOT COMMA COLON SEMICOLON

%left EQUAL;
%left LT GT LTE GTE;
%left PLUS MINUS;
%left MUL DIV;

%type <intVal> NUMBER
%type <strVal> INT
%type <strVal> DOUBLE
%type <strVal> CAROT
%type <strVal> AT
%type <strVal> CHAR
%type <strVal> STR 
%type <strVal> STRING
%type <strVal> SEMICOLON
%type <strVal> PLUS
%type <strVal> MINUS
%type <strVal> DIV
%type <strVal> MUL
%type <strVal> DOT
%type <strVal> EQUAL
%type <strVal> FUNC
%type <strVal> MODULE
%type <strVal> EXTERN
%type <strVal> KERNEL
%type <strVal> RARROW
%type <strVal> LARROW
%type <strVal> LBRACE
%type <strVal> LPAREN
%type <strVal> RPAREN
%type <strVal> RBRACE
%type <strVal> ID

%type <strVal> dataType 

%start program

%%
program: expressions

expressions: expressions expression
           | expression

expression: varDef END
          | funcDef END
          | kernelDef END
          | modImport END
          | extern END
          | funcCall END
          | forLoop END
          | ifBranch END
          | binOp END
          | NUMBER END
          | ID END

modImport: MODULE ID SEMICOLON { cout << "Module loaded: " << $2 << endl; }

funcCall: ID LPAREN paramDefs RPAREN { cout << "Calling func " << $1 << endl; }
        | ID LPAREN RPAREN { cout << "Calling func " << $1 << endl; }

extern: EXTERN funcProto SEMICOLON
      | EXTERN kernelProto SEMICOLON

dataType: INT { $$ = "int"; }
        | DOUBLE { $$ = "double"; }
        | CHAR { $$ = "char"; }
        | STR { $$ = "string"; }

varDef: dataType ID { createVarDef(new VarInitExprAST($2,$1)); cout << "Variable Defined!\n"; }

paramDefs: paramDefs COMMA paramDef
         | paramDef 

paramDef: dataType ID { VarInitExprAST* temp = new VarInitExprAST($2,$1); /*args.push_back(temp);*/ createVarDef(temp); cout << "Parameter defined!\n"; }

block: LBRACE expressions RBRACE
     | LBRACE RBRACE

forLoop: FOR ID EQUAL expression COMMA expression block
       | FOR ID EQUAL expression COMMA expression COMMA expression block 

elseBranch: ELSE block
          | ELIF block elseBranch

ifBranch: IF expression block 
        | IF expression block elseBranch

funcDef: funcProto block { cout << "Function defined!\n"; }

kernelDef: kernelProto block { cout << "Kernel defined!\n"; }

kernelProto: KERNEL ID LPAREN paramDefs RPAREN LARROW NUMBER 
           | KERNEL ID LPAREN           RPAREN LARROW NUMBER

funcProto: FUNC ID LPAREN paramDefs RPAREN RARROW dataType 
         | FUNC ID LPAREN           RPAREN RARROW dataType

binOp: ID EQUAL expression
     | expression PLUS expression
     | expression MINUS expression
     | expression DIV expression
     | expression MUL expression
     | expression LT expression
     | expression GT expression
     | expression LTE expression
     | expression GTE expression
