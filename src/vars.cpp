#define DEBUG 1
#include <iostream>
#include <fstream>
#include "llvm/Analysis/Passes.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Support/raw_ostream.h"
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
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/User.h"
#include <boost/lexical_cast.hpp>
#include <cctype>
#include <cstdio>
#include <map>
#include <string>
#include <vector>
#include<stdio.h>
#include<stdlib.h>
#include "jlang.tab.hpp"
#include "tree.h"
using namespace std;
using namespace llvm;

extern FILE* yyin;
extern int lineNum;

extern Module *theModule;
static IRBuilder<> Builder(getGlobalContext());
map<string, AllocaInst*> NamedValues;
PointerType* intPtr32 = PointerType::get(Type::getInt32Ty(getGlobalContext()), 0);
PointerType* intPtr8 = PointerType::get(Type::getInt8Ty(getGlobalContext()), 0);
PointerType* doublePtr = PointerType::get(Type::getDoubleTy(getGlobalContext()), 0);

void dumpVars()
{
  map<string,AllocaInst*>::iterator it;
  cout << "\nDumping vars: \n";
  for(it=NamedValues.begin();it!=NamedValues.end(); it++)
  {
    cout << it->first << ": " << it->second;
    cout << "\tType: " << typeTab[it->first] << endl;
  }
}

static AllocaInst *CreateEntryBlockAlloca(const string &VarName, string type) 
{
  if (type == "double")
    return Builder.CreateAlloca(Type::getDoubleTy(getGlobalContext()), 0, VarName.c_str());
  else if (type == "doubles")
    return Builder.CreateAlloca(doublePtr, 0, VarName.c_str());
  else if (type == "int")
    return Builder.CreateAlloca(Type::getInt32Ty(getGlobalContext()), 0, VarName.c_str());
  else if (type == "ints")
    return Builder.CreateAlloca(intPtr32, 0, VarName.c_str());
  else if (type == "char")
    return Builder.CreateAlloca(Type::getInt8Ty(getGlobalContext()), 0, VarName.c_str());
  else if (type == "chars")
    return Builder.CreateAlloca(intPtr8, 0, VarName.c_str());
  return 0;
}

Value* IntExprAST::Codegen()
{
  return ConstantInt::get(Type::getInt32Ty(getGlobalContext()), Val);
}

Value* DoubleExprAST::Codegen()
{
  return ConstantFP::get(Type::getDoubleTy(getGlobalContext()), Val);
}

Value* CharExprAST::Codegen()
{
  return ConstantInt::get(Type::getInt8Ty(getGlobalContext()), Val);
}

Value* stringExprAST::Codegen()
{
  int i = 0;
  vector<uint8_t> v;
  while (i<Size)
  {
    v.push_back(Val[i]);
    i++;
  } 
  ArrayRef<uint8_t> chars(v);
  return ConstantDataArray::get(getGlobalContext(), chars);
}


Value* VariableExprAST::Codegen()
{
  //does var exist?
  Value* V = NamedValues[Name];
  if (V == 0)
  {
#ifdef DEBUG
    dumpVars();
#endif
    cerr << "\033[31m ERROR: \033[37m Unknown Variable Reference: " << Name << endl;
    exit(EXIT_FAILURE);
  }
  return Builder.CreateLoad(V, Name);
}

Value* VarInitExprAST::Codegen()
{
  if (NamedValues.find(Name) == NamedValues.end())
  {
    AllocaInst* Alloca;
    vector<AllocaInst* > oldBindings;

    Function* F = Builder.GetInsertBlock()->getParent();

    Value* Initial;
    if(Initd) //if initialized
      Initial = Initd->Codegen();
    else
    {
      if (Type == "double")
        Initial = ConstantFP::get(Type::getDoubleTy(getGlobalContext()),0.0);
      else if (Type == "doubles")
        Initial = ConstantFP::get(Type::getDoubleTy(getGlobalContext()),0.0);
      else if (Type == "int")
        Initial = ConstantInt::get(Type::getInt32Ty(getGlobalContext()), 0);
      else if (Type == "ints")
        Initial = ConstantInt::get(Type::getInt32Ty(getGlobalContext()), 0);
      else if (Type == "char")
        Initial = ConstantInt::get(Type::getInt8Ty(getGlobalContext()), 0);
      else if (Type == "chars")
        Initial = ConstantInt::get(Type::getInt8Ty(getGlobalContext()), 0);
      else if (Type == "string")
        Initial = ConstantDataArray::getString(getGlobalContext(), "");
    }
    Alloca = CreateEntryBlockAlloca(Name,Type);
    NamedValues[Name] = Alloca;
    return Builder.CreateStore(Initial,Alloca);
  }
  else
  {
#ifdef DEBUG
    dumpVars();
#endif
    cerr << "\033[31m ERROR: \033[37m Variable already exists: " << Name << endl;
    exit(EXIT_FAILURE);
  }
}


