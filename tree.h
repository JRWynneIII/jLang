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
  ExprAST* Next = NULL;
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
  IntExprAST(double val) : Val(val) {}
  virtual Value *Codegen();
};

class DoubleExprAST : public ExprAST 
{
  double Val;
public:
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
public:
  VarInitExprAST(const string &name, const string &type) : Name(name), Type(type) {}
  const string &getName() const { return Name; }
  virtual Value* Codegen();
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
  void CreateArgumentAllocas(Function *F);
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

static void createfuncDef(FunctionAST* F);
static void createExtern(PrototypeAST* P);
static void createTLE(FunctionAST* F);
