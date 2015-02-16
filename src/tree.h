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

extern map<string,string> typeTab;

class ExprAST 
{
public:
  virtual ~ExprAST() {}
  virtual string getType() = 0;
  virtual Value *Codegen() = 0;
};

class identExprAST : public ExprAST
{
public:
  string Name;
  identExprAST(const string& name) : Name(name) {}
  virtual string getType() { return typeTab[Name]; }
  virtual Value* Codegen() = 0;
};

class ForExprAST : public ExprAST
{
  string VarName;
  ExprAST *Start, *End, *Step;
  vector<ExprAST*> Body;
public:
  ForExprAST(const string &varname, ExprAST *start, ExprAST *end, ExprAST *step, vector<ExprAST*> body) : VarName(varname), Start(start), Step(step), End(end), Body(body) {}
  virtual string getType() { return typeTab[VarName]; }
  virtual Value* Codegen();
};

class IfExprAST : public ExprAST
{
  ExprAST *Cond;
  vector<ExprAST*> Else, Then;
  bool hasElse;
public:
  IfExprAST(ExprAST *cond, vector<ExprAST*> then, vector<ExprAST*> _else) : Cond(cond), Then(then), Else(_else) { hasElse = true; }
  IfExprAST(ExprAST *cond, vector<ExprAST*> then) : Cond(cond), Then(then) { hasElse = false; }
  virtual string getType() { return Cond->getType(); }
  virtual Value *Codegen();
};


class IntExprAST : public ExprAST 
{
public:
  int Val;
  IntExprAST(double val) : Val(val) {}
  virtual string getType() { return "int"; }
  virtual Value *Codegen();
};

class DoubleExprAST : public ExprAST 
{
public:
  double Val;
  DoubleExprAST(double val) : Val(val) {}
  virtual string getType() { return "double"; }
  virtual Value *Codegen();
};

class stringExprAST : public ExprAST
{
public:
  const char* Val;
  double Size;
  stringExprAST(const char* val, double size) : Val(val), Size(size) {}
  virtual string getType() { return "string"; }
  virtual Value* Codegen();
};

class CharExprAST : public ExprAST
{
public:
  char Val;
  CharExprAST(char val) : Val(val) {}
  virtual string getType() { return "char"; }
  virtual Value* Codegen();
};

class VariableExprAST : public ExprAST 
{
  string Name;
  string Type;
public:
  VariableExprAST(const string &name, const string &type) : Name(name), Type(type) {}
  const string &getName() const { return Name; }
  virtual string getType() { return typeTab[Name]; }
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
  virtual string getType() { return Type; }
  virtual Value* Codegen();
};

class TypeCastExprAST : public ExprAST
{
  string toType;
  string varName;
public:
  TypeCastExprAST(string name, string ty) : varName(name), toType(ty) {}
  virtual string getType() { return toType; }
  string getName() { return varName; }
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
  virtual string getType() { return LHS->getType(); }
  string getLHSVar() { return dynamic_cast<VariableExprAST*>(LHS)->getName(); }
  string getRHSVar() { return dynamic_cast<VariableExprAST*>(RHS)->getName(); }
  virtual Value *Codegen();
};

class UnaryExprAST : public ExprAST
{
  char Op;
  VariableExprAST *RHS;
public:
  UnaryExprAST(char op, VariableExprAST *rhs) : Op(op), RHS(rhs) {}
  string getLHSVar() { return RHS->getName(); }
  virtual string getType() { return RHS->getType(); }
  virtual Value* Codegen();
};

class CallExprAST : public ExprAST 
{
  string Callee;
  vector<ExprAST*> Args;
public:
  CallExprAST(const string &callee, vector<ExprAST*> &args) : Callee(callee), Args(args) {}
  virtual string getType() { return "None"; }
  virtual Value *Codegen();
};

class PrototypeAST : public ExprAST
{
  string Name;
  vector<VarInitExprAST*> Args;
  string Ty;
public:
  PrototypeAST(const string &name, const vector<VarInitExprAST*> &args, const string& type) : Name(name), Args(args), Ty(type) {}
  virtual string getType() { return Ty; }
  
  Function *Codegen();
  void CreateArgumentAllocas(Function *F);
};

class FunctionAST : public ExprAST
{
  PrototypeAST *Proto;
  vector<ExprAST*> Body;
public:
  FunctionAST(PrototypeAST *proto, vector<ExprAST*> body) : Proto(proto), Body(body) {}
  virtual string getType() { return Proto->getType(); }
  
  Function *Codegen();
};

void createExtern(PrototypeAST* P);
void loadModule(const char* name);
#endif
