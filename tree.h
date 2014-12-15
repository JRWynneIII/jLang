#include<iostream>
#include<map>
#include<vector>
#include<stdlib.h>
using namespace std;

class ExprAST 
{
public:
  virtual ~ExprAST() {}
  virtual Value *Codegen() = 0;
};

class ForExprAST : public ExprAST
{
  string VarName;
  ExprAST *Start, *End, *Step, *Body;
public:
  ForExprAST(const string &varname, ExprAST *start, ExprAST *end, ExprAST *step, ExprAST *body) : VarName(varname), Start(start), End(end), Body(body) {}
  virtual Value* Codegen();
};

class IfExprAST : public ExprAST
{
  ExprAST *Cond, *Then, *Else;
public:
  IfExprAST(ExprAST *cond, ExprAST *then, ExprAST *_else) : Cond(cond), Then(then), Else(_else) {}
  virtual Value *Codegen();
};


class IntExprAST : public ExprAST 
{
  int Val;
public:
  NumberExprAST(double val) : Val(val) {}
  virtual Value *Codegen();
};

class DoubleExprAST : public ExprAST 
{
  double Val;
public:
  NumberExprAST(double val) : Val(val) {}
  virtual Value *Codegen();
};

class VariableExprAST : public ExprAST 
{
  string Name;
  string type;
public:
  VariableExprAST(const string &name) : Name(name) {}
  virtual Value *Codegen();
};

class BinaryExprAST : public ExprAST 
{
  char Op;
  ExprAST *LHS, *RHS;
public:
  BinaryExprAST(char op, ExprAST *lhs, ExprAST *rhs) 
    : Op(op), LHS(lhs), RHS(rhs) {}
  virtual Value *Codegen();
};

class CallExprAST : public ExprAST 
{
  string Callee;
  vector<ExprAST*> Args;
public:
  CallExprAST(const string &callee, vector<ExprAST*> &args)
    : Callee(callee), Args(args) {}
  virtual Value *Codegen();
};

class PrototypeAST 
{
  string Name;
  vector<string> Args;
public:
  PrototypeAST(const string &name, const vector<std::string> &args)
    : Name(name), Args(args) {}
  
  Function *Codegen();
};

class FunctionAST 
{
  PrototypeAST *Proto;
  ExprAST *Body;
public:
  FunctionAST(PrototypeAST *proto, ExprAST *body)
    : Proto(proto), Body(body) {}
  
  Function *Codegen();
};
