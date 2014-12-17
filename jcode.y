%{
#include<stdio.h>
#include <iostream>
#include <map>
#include "tree.h"
#include <stack>
std::map<std::string,Value*> varTable;
std::vector<ExprAST*> args;
extern stack<ExprAST*> parseStack;
extern int yylex(void);
extern void storeVar(char*,double);
extern void storeStringVar(char*,char*);
extern int lineNum;
using namespace std;
PrototypeAST* tempProto;
ExprAST* tail;
void link(ExprAST* next)
{
  tail->Next = next;
  tail = next;
}
void yyerror(char const *s) 
{ 
  cerr << "\033[31m ERROR: \033[37m" << s << " in line: " << lineNum << endl;
//  fprintf(stderr,"ERROR: %s in line %d\n", s, lineNum); 
}
%}

%union
{
  int intVal;
  char charVal;
  double doubleVal;
  char* strVal;
  ExprAST* node;
  std::vector<ExprAST*> nodes;
}

%require "3.0"
%error-verbose

%token NUMBER FLOAT
%token STRING ID
%token FUNC KERNEL EXTERN FOR IF ELSE ELIF MODULE
%token INT DOUBLE CHAR STR
%token RARROW LARROW CAROT
%token EQUAL GT LT GTE LTE NEQUAL PLUS MINUS MUL DIV MOD EMARK QMARK AND OR LSBRACE RSBRACE LPAREN RPAREN LBRACE RBRACE AT DOT COMMA COLON SEMICOLON

%left EQUAL;
%left LT GT LTE GTE;
%left PLUS MINUS;
%left MUL DIV;

%type <intVal> NUMBER
%type <doubleVal> DOUBLE
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
%type <strVal> LPAREN
%type <strVal> RPAREN
%type <strVal> ID
%type <strVal> GT LT GTE LTE

%type <node> expression expressions numLiteral identLiteral varDef funcDef kernelDef modImport extern funcCall binOp funcProto kernelProto
%type <nodes> paramDef paramDefs
%type <node> block
%type <str> dataType
%start program

%%
program: expressions

expressions: expressions expression
           | expression

expression: varDef
          | funcDef
          | kernelDef
          | modImport
          | extern
          | funcCall
          | binOp
          | numLiteral
          | identLiteral

numLiteral: NUMBER { $$ = new DoubleExprAST($1); }  

identLiteral: ID { $$ = new VariableExprAST($1,"string"); }

modImport: MODULE ID SEMICOLON { cout << "Module loaded: " << $2 << endl; }

funcCall: ID LPAREN paramDefs RPAREN { $$ = new CallExprAST($1,$3); cout << "Calling func " << $1 << endl; }
        | ID LPAREN RPAREN { $$ = new CallExprAST($1,NULL); cout << "Calling func " << $1 << endl; }

extern: EXTERN funcProto SEMICOLON  { $$ = new PrototypeAST($2); }
      | EXTERN kernelProto SEMICOLON { $$ = new PrototypeAST($2); }

dataType: INT { $$ = "int"; cout << "Datatype set to Int\n"; }
        | DOUBLE { $$ = "double"; cout << "Datatype set to Double\n"; }
        | CHAR { $$ = "char"; cout << "Datatype set to Char\n"; }
        | STR { $$ = "string"; cout << "Datatype set to String\n"; }

varDef: dataType ID { $$ = new VariableExprAST($2,$1); cout << "Variable Defined!\n"; }

paramDefs: paramDefs COMMA paramDef
         | paramDef 

paramDef: dataType ID { cout << "Parameter defined!" << $1 << "\n"; }

block: LBRACE expressions RBRACE { $$ = $2; }
     | LBRACE RBRACE { $$ = $$ }

/*forLoop: FOR ID EQUAL expression COMMA expression block
       | FOR ID EQUAL expression COMMA expression COMMA expression block */
/*
elseBranch: ELSE block
          | ELIF block elseBranch

ifBranch: IF expression block 
        | IF expression block elseBranch
*/
funcDef: funcProto block { link(new FunctionAST(tempProto, $2)); cout << "Function defined!\n"; }

kernelDef: kernelProto block { cout << "Kernel defined!\n"; }

kernelProto: KERNEL ID LPAREN paramDefs RPAREN LARROW NUMBER { $$ = new PrototypeAST($2,&args); args.clear(); }
           | KERNEL ID LPAREN           RPAREN LARROW NUMBER { $$ = new PrototypeAST($2,NULL); } 

funcProto: FUNC ID LPAREN paramDefs RPAREN RARROW dataType { $$ = new PrototypeAST($2, &args); args.clear(); }
         | FUNC ID LPAREN           RPAREN RARROW dataType { $$ = new PrototypeAST($2,NULL); }

binOp: ID EQUAL expression { $$ = new BinaryExprAST($2,$1,$3); }
     | expression PLUS expression { $$ = new BinaryExprAST($2,$1,$3); }
     | expression MINUS expression { $$ = new BinaryExprAST($2,$1,$3); }
     | expression DIV expression { $$ = new BinaryExprAST($2,$1,$3); }
     | expression MUL expression { $$ = new BinaryExprAST($2,$1,$3); }
     | expression LT expression { $$ = new BinaryExprAST($2,$1,$3); }
     | expression GT expression { $$ = new BinaryExprAST($2,$1,$3); }
     | expression LTE expression { $$ = new BinaryExprAST($2,$1,$3); }
     | expression GTE expression { $$ = new BinaryExprAST($2,$1,$3); }
