#ifndef TREE_H
#define TREE_H
#include<iostream>
#include<map>
#include<vector>
#include<stdlib.h>
#include "llvm/Analysis/Passes.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/PassManager.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Transforms/Scalar.h"
using namespace std;
using namespace llvm;

class ExprAST 
{
public:
  virtual ~ExprAST() {}
  virtual Value *Codegen() = 0;
};

class identExprAST : public ExprAST
{
public:
  string Name;
  identExprAST(const string& name) : Name(name) {}
  virtual Value* Codegen() = 0;
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
public:
  int Val;
  IntExprAST(double val) : Val(val) {}
  virtual Value *Codegen();
};

class DoubleExprAST : public ExprAST 
{
public:
  double Val;
  DoubleExprAST(double val) : Val(val) {}
  virtual Value *Codegen();
};

class VariableExprAST : public ExprAST 
{
  string Name;
  string Type;
public:
  VariableExprAST(const string &name, const string &type) : Name(name), Type(type) {}
  const string &getName() const { return Name; }
  virtual Value *Codegen();
};

class VarInitExprAST : public ExprAST
{
  string Name;
  string Type;
  ExprAST* Initd;
public:
  VarInitExprAST(const string &name, const string &type, ExprAST* initd) : Name(name), Type(type), Initd(initd) {}
  const string &getName() const { return Name; }
  virtual Value* Codegen();
};

class BinaryExprAST : public ExprAST 
{
  char Op;
  ExprAST *LHS, *RHS;
  string Var;
public:
  BinaryExprAST(char op, ExprAST *lhs, ExprAST *rhs) : Op(op), LHS(lhs), RHS(rhs) {}
  BinaryExprAST(char op, const string& var, ExprAST *rhs) : Op(op), Var(var), RHS(rhs) {}
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
  vector<VarInitExprAST*> Args;
  string Ty;
public:
  PrototypeAST(const string &name, const vector<VarInitExprAST*> &args, const string& type)
    : Name(name), Args(args), Ty(type) {}
  string getType() { return Ty; }
  
  Function *Codegen();
  void CreateArgumentAllocas(Function *F);
};

class FunctionAST 
{
  PrototypeAST *Proto;
  vector<ExprAST*> Body;
public:
  FunctionAST(PrototypeAST *proto, vector<ExprAST*>& body)
    : Proto(proto), Body(body) {}
  
  Function *Codegen();
};

void createfuncDef(FunctionAST* F);
void createExtern(PrototypeAST* P);
void createTLE(FunctionAST* F);
void createVarDef(VarInitExprAST* V);
void createBinOp(BinaryExprAST* V);
#endif
