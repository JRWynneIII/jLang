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
using namespace std;
%}

%verbose

%union
{
  int intVal;
  char charVal;
  double doubleVal;
  char* strVal;
}

%token NUMBER FLOAT
%token STRING ID
%token FUNC KERNEL EXTERN FOR IF ELSE ELIF
%token INT DOUBLE CHAR STR
%token RARROW LARROW CAROT
%token EQUAL GT LT GTE LTE NEQUAL PLUS MINUS MUL DIV MOD EMARK QMARK AND OR LSBRACE RSBRACE LPAREN RPAREN LBRACE RBRACE AT DOT COMMA COLON SEMICOLON

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
%type <str> EXTERN
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
       | funcDef 
       | extern

expressions: expressions expression
           | expression

expression: varDef
          | binOp
          | NUMBER
          | ID

extern: EXTERN funcProto SEMICOLON
      | EXTERN kernelProto SEMICOLON

dataType: INT { cout << "Datatype set to Int\n"; }
        | DOUBLE { cout << "Datatype set to Double\n"; }
        | CHAR { cout << "Datatype set to Char\n"; }
        | STR { cout << "Datatype set to String\n"; }

varDef: dataType ID SEMICOLON { cout << "Variable Defined!\n"; }


paramDefs: paramDefs COMMA paramDef
         | paramDef 

paramDef: dataType ID { cout << "Parameter defined!\n"; }

funcDef: funcProto LBRACE expressions RBRACE { cout << "Function defined!\n"; }
       | kernelProto LBRACE expressions RBRACE { cout << "Kernel defined!\n"; }

funcProto: FUNC ID LPAREN paramDefs RPAREN RARROW dataType 
         | FUNC ID LPAREN RPAREN RARROW dataType

kernelProto: KERNEL ID LPAREN paramDefs RPAREN LARROW NUMBER 
           | KERNEL ID LPAREN RPAREN LARROW NUMBER

%left EQUAL;
%left LT GT LTE GTE;
%left PLUS MINUS;
%left MUL DIV;

binOp: expression EQUAL expression
     | expression PLUS expression
     | expression MINUS expression
     | expression DIV expression
     | expression MUL expression
     | expression LT expression
     | expression GT expression
     | expression LTE expression
     | expression GTE expression

