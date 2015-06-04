#define DEBUG 0
#ifndef TREE_H
#define TREE_H
#include<iostream>
#include<map>
#include<vector>
#include<stdlib.h>
#include "llvm/Analysis/Passes.h"
#include "llvm/IR/Verifier.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
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
  virtual string getName() = 0;
  virtual Value *Codegen() = 0;
};

class identExprAST : public ExprAST
{
public:
  string Name;
  identExprAST(const string& name) : Name(name) {}
  virtual string getType() { return typeTab[Name]; }
  virtual string getName() { return Name; }
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
  virtual string getName() { return VarName; }
  virtual Value* Codegen();
};

class nullExprAST : public ExprAST
{
public:
  string Name;
  nullExprAST() : Name("null") {}
  virtual string getType() { return "null"; }
  virtual string getName() { return "null"; }
  virtual Value* Codegen() {}
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
  virtual string getName() { return Cond->getName(); }
  virtual Value *Codegen();
};

class IntExprAST : public ExprAST 
{
public:
  int Val;
  IntExprAST(double val) : Val(val) {}
  virtual string getType() { return "int"; }
  virtual string getName() { return "int"; }
  virtual Value *Codegen();
};

class ArrayIndexAST : public ExprAST
{
  ExprAST* Index;
  string VarName;
public:
  ArrayIndexAST(ExprAST* index, string varname) : Index(index), VarName(varname) {}
  virtual string getType() { return typeTab[VarName]; }
  virtual string getName() { return VarName; }
  virtual Value* Codegen();
};

class DoubleExprAST : public ExprAST 
{
public:
  double Val;
  DoubleExprAST(double val) : Val(val) {}
  virtual string getType() { return "double"; }
  virtual string getName() { return "double"; }
  virtual Value *Codegen();
};

class stringExprAST : public ExprAST
{
public:
  const char* Val;
  double Size;
  stringExprAST(const char* val, double size, bool isPString = false) : Val(val), Size(size) {}
  virtual string getType() { return "string"; }
  virtual string getName() { string str(Val); return str; }
  virtual Value* Codegen();
};

class CharExprAST : public ExprAST
{
public:
  char Val;
  CharExprAST(char val) : Val(val) {}
  virtual string getType() { return "char"; }
  virtual string getName() { return "char"; }
  virtual Value* Codegen();
};

class VariableExprAST : public ExprAST 
{
  string Name;
  string Type;
public:
  VariableExprAST(const string &name, const string &type) : Name(name), Type(type) {}
  virtual string getName() { return Name; }
  virtual string getType() { return typeTab[Name]; }
  virtual Value *Codegen();
};

class VarInitExprAST : public ExprAST
{
  string Name;
  string Type;
  ExprAST* Initd;
  ExprAST* arrayIdx;
public:
  VarInitExprAST(const string &name, const string &type, ExprAST* initd, ExprAST* ArrayIdx = new IntExprAST(1)) : Name(name), Type(type), Initd(initd), arrayIdx(ArrayIdx) {}
  virtual string getName() { return Name; }
  virtual string getType() { return Type; }
  virtual Value* Codegen();
};

class globalVarExprAST : public ExprAST
{
  string Name;
  string Type;
  ExprAST* Initd;
  ExprAST* arrayIdx;
public:
  globalVarExprAST(const string &name, const string &type, ExprAST* initd, ExprAST* ArrayIdx = new IntExprAST(1)) : Name(name), Type(type), Initd(initd), arrayIdx(ArrayIdx) {}
  virtual string getName() { return Name; }
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
  virtual string getName() { return varName; }
  virtual Value* Codegen();
};

class BinaryExprAST : public ExprAST 
{
  char Op;
  ExprAST *LHS, *RHS;
  Value* L, *R;
  string lty, rty;
  string Var;
public:
  BinaryExprAST(char op, ExprAST *lhs, ExprAST *rhs) : Op(op), LHS(lhs), RHS(rhs) {}
  BinaryExprAST(char op, const string& var, ExprAST *rhs) : Op(op), Var(var), RHS(rhs) {}
  virtual string getType() { return LHS->getType(); }
  string getLHSVar() { return dynamic_cast<VariableExprAST*>(LHS)->getName(); }
  string getRHSVar() { return dynamic_cast<VariableExprAST*>(RHS)->getName(); }
  virtual string getName() { return Var; }
  virtual Value *Codegen();
private:
  bool checkTypes();  //Checks if the types match
  void convertTypes();  //Will up/downconvert types as nessicary
  bool isPtrOp();   //Returns true if the operation involves pointers
  bool isArrayOp();
  Value* doOp();
  Value* doPtrOp();
  Value* doAssignmentOp();
};

class UnaryExprAST : public ExprAST
{
  char Op;
  ExprAST *RHS;
public:
  UnaryExprAST(char op, ExprAST *rhs) : Op(op), RHS(rhs) {}
  string getLHSVar() { return dynamic_cast<VariableExprAST*>(RHS)->getName(); }
  virtual string getType() { return RHS->getType(); }
  virtual string getName() { return RHS->getName(); }
  virtual Value* Codegen();
};

class CallExprAST : public ExprAST 
{
  string Callee;
  vector<ExprAST*> Args;
public:
  CallExprAST(const string &callee, vector<ExprAST*> &args) : Callee(callee), Args(args) {}
  virtual string getType() { return "None"; }
  virtual string getName() { return Callee; }
  virtual Value *Codegen();
};

class PrototypeAST : public ExprAST
{
  string Name;
  vector<VarInitExprAST*> Args;
  string Ty;
public:
  PrototypeAST(const string &name, const vector<VarInitExprAST*> &args, const string& type, bool isKernel = false) : Name(name), Args(args), Ty(type) {}
  virtual string getType() { return Ty; }
  virtual string getName() { return Name; }
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
  virtual string getName() { return Proto->getName(); }
  
  Function *Codegen();
};

template <typename T, typename U>
class SymbolTable
{
private:
  map<T,U> _NamedValues;
  map<T,U> _GlobalValues;
public:
  SymbolTable() {}
  ~SymbolTable() {}
  typedef typename std::map<T,U>::iterator iterator;
  typedef typename std::map<T,U>::const_iterator const_iterator;
  iterator begin() { return _NamedValues.begin(); }
  const_iterator begin() const { return _NamedValues.begin(); }
  iterator end() { return _GlobalValues.end(); }
  const_iterator end() const { return _GlobalValues.end(); }
  iterator find(T key) 
  {
    if (_NamedValues.find(key) == _NamedValues.end())
      return _GlobalValues.find(key);
    else
      return _NamedValues.find(key);
  }
  void clear() { _NamedValues.clear(); }
  void clearAll() { this->clear(); _GlobalValues.clear(); }
  void addGlobal(T key, U val) { _GlobalValues[key] = val; }
  U& operator[](T key) 
  { 
    if (_NamedValues[key])
      return _NamedValues[key];
    else
      return _GlobalValues[key];
  }
  U operator[] (T key) const
  { 
    if (_NamedValues[key])
      return _NamedValues[key];
    else
      return _GlobalValues[key];
  }
  void dump()
  {
    //THIS IS HACKY. For some reason clang won't allow map<T,U>::iterator here. Gives a missing semicolon error
    map<string,Value*>::iterator it;
    cout << "\nDumping vars: \n";
    for(it=_NamedValues.begin();it!=_NamedValues.end(); it++)
    {
      cout << it->first << ": " << it->second;
      cout << "\tType: " << typeTab[it->first] << endl;
    }
    cout << "\nDumping global vars: \n";
    for(it=_GlobalValues.begin();it!=_GlobalValues.end(); it++)
    {
      cout << it->first << ": " << it->second;
      cout << "\tType: " << typeTab[it->first] << endl;
    }
  }
};
class ObjectSymbolTable
{
private:
  SymbolTable<string,Value*> Vars;
  map<string,Value*> methodTable;
  map<string,string> types;
public:
  ObjectSymbolTable() {}
  ~ObjectSymbolTable() {}
  void clear() { Vars.clear(); }
  void clearAll() { Vars.clearAll();  methodTable.clear(); }
  void addMethod(string key, Value* method) { methodTable[key] = method; }
  void addVar(string key, Value* var) { Vars[key] = var; }
  void addObjectGlobal(string key, Value* var) { Vars.addGlobal(key,var); }
  string getType(string key) { return types[key]; }
  Type*  getLLVMType(string key);
  void setType(string key, string type) { types[key] = type; }
  Value* operator[](string key)
  {
    if(Vars[key])
      return Vars[key];
    else
      return methodTable[key];
  }
};

class ClassAST : public ExprAST
{
  string Name;
  FunctionAST* Init;
  vector<FunctionAST*> FunctionList;
  vector<ExprAST*> Vars;
  ObjectSymbolTable symbols;
public:
  ClassAST(string name, FunctionAST* init, vector<ExprAST*> vars, vector<FunctionAST*> functionlist) : Name(name), Init(init), Vars(vars), FunctionList(functionlist) {}
  ~ClassAST() {}
  virtual string getType() { return Name; }
  virtual string getName() { return Name; }
  virtual Value* Codegen();
};

class ObjectInitAST : public ExprAST
{
  string Name;
  string Object;
  ExprAST* Init;
public:
  ObjectInitAST(string object, string name) : Name(name), Object(object) {}
  ObjectInitAST(string object, string name, ExprAST* init) : Name(name), Object(object), Init(init) {}
  virtual string getType() { return Object; }
  virtual string getName() { return Name; }
  virtual Value* Codegen();
};

class KernelAST : public ExprAST
{
  PrototypeAST *Proto;
  vector<ExprAST*> Body;
public:
  KernelAST(PrototypeAST *proto, vector<ExprAST*> body) : Proto(proto), Body(body) {}
  virtual string getType() { return Proto->getType(); }
  virtual string getName() { return Proto->getName(); }
  
  Function *Codegen();
};

void createExtern(PrototypeAST* P);
void loadModule(const char* name);
void ERROR(string err);

#endif
