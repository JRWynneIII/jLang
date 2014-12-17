%require "3.0"
%{
#include<stdio.h>
#include<iostream>
#include<map>
std::map<std::string,Value*> varTable;
std::vector<string> args;
extern stack<ExprAST*> parseStack;
extern int yylex(void);
extern void storeVar(char*,double);
extern void storeStringVar(char*,char*);
extern int lineNum;
using namespace std;
PrototypeAST* tempProto;
void yyerror(char const *s) 
{ 
  cerr << "\033[31m ERROR: \033[37m" << s << " in line: " << lineNum << endl;
//  fprintf(stderr,"ERROR: %s in line %d\n", s, lineNum); 
}
%}

%error-verbose

%union
{
  int intVal;
  char charVal;
  double doubleVal;
  char* strVal;
}

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
%type <strVal> LBRACE
%type <strVal> LPAREN
%type <strVal> RPAREN
%type <strVal> RBRACE
%type <strVal> ID

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
          | forLoop
          | ifBranch
          | binOp
          | NUMBER
          | ID

modImport: MODULE ID SEMICOLON { cout << "Module loaded: " << $2 << endl; }

funcCall: ID LPAREN paramDefs RPAREN { cout << "Calling func " << $1 << endl; }
        | ID LPAREN RPAREN { cout << "Calling func " << $1 << endl; }

extern: EXTERN funcProto SEMICOLON
      | EXTERN kernelProto SEMICOLON

dataType: INT { cout << "Datatype set to Int\n"; }
        | DOUBLE { cout << "Datatype set to Double\n"; }
        | CHAR { cout << "Datatype set to Char\n"; }
        | STR { cout << "Datatype set to String\n"; }

varDef: dataType ID { cout << "Variable Defined!\n"; }

paramDefs: paramDefs COMMA paramDef
         | paramDef 

paramDef: dataType ID { args.push_back($1); cout << "Parameter defined!" << $1 << "\n"; }

block: LBRACE expressions RBRACE
     | LBRACE RBRACE

forLoop: FOR ID EQUAL expression COMMA expression block
       | FOR ID EQUAL expression COMMA expression COMMA expression block 

elseBranch: ELSE block
          | ELIF block elseBranch

ifBranch: IF expression block 
        | IF expression block elseBranch

funcDef: funcProto block { parseStack.push(new FunctionAST(tempProto, ); cout << "Function defined!\n"; }

kernelDef: kernelProto block { cout << "Kernel defined!\n"; }

kernelProto: KERNEL ID LPAREN paramDefs RPAREN LARROW NUMBER { tempProto = new PrototypeAST($2,&args); args.clear(); }
           | KERNEL ID LPAREN           RPAREN LARROW NUMBER { tempProto = new PrototypeAST($2,NULL); } 

funcProto: FUNC ID LPAREN paramDefs RPAREN RARROW dataType { tempProto = new PrototypeAST($2, &args); args.clear(); }
         | FUNC ID LPAREN           RPAREN RARROW dataType { tempProto = new PrototypeAST($2,NULL); }

binOp: ID EQUAL expression
     | expression PLUS expression
     | expression MINUS expression
     | expression DIV expression
     | expression MUL expression
     | expression LT expression
     | expression GT expression
     | expression LTE expression
     | expression GTE expression
