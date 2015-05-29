#include <string.h>
#include <iostream>
#include <fstream>
#include "llvm/Analysis/Passes.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
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
extern IRBuilder<> Builder;
SymbolTable<string, Value*> NamedValues;
PointerType* intPtr32 = PointerType::get(Type::getInt32Ty(getGlobalContext()), 0);
PointerType* intPtr64 = PointerType::get(Type::getInt64Ty(getGlobalContext()), 0);
PointerType* intPtr8 = PointerType::get(Type::getInt8Ty(getGlobalContext()), 0);
PointerType* doublePtr = PointerType::get(Type::getDoubleTy(getGlobalContext()), 0);

void dumpVars()
{
  NamedValues.dump();
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
  else if (type == "string")
    return Builder.CreateAlloca(intPtr8, 0 , VarName.c_str());
  return 0;
}

static Value* CreateEntryBlockGlobal(const string &VarName, string type) 
{
  if (type == "double")
    return new GlobalVariable(Type::getDoubleTy(getGlobalContext()),false,GlobalValue::WeakAnyLinkage,nullptr,VarName);
  else if (type == "doubles")
    return new GlobalVariable(doublePtr,false,GlobalValue::WeakAnyLinkage,nullptr,VarName);
  else if (type == "int")
    return new GlobalVariable(Type::getInt32Ty(getGlobalContext()),false,GlobalValue::WeakAnyLinkage,nullptr,VarName);
  else if (type == "ints")
    return new GlobalVariable(intPtr32,false,GlobalValue::WeakAnyLinkage,nullptr,VarName);
  else if (type == "char")
    return new GlobalVariable(Type::getInt8Ty(getGlobalContext()),false,GlobalValue::WeakAnyLinkage,nullptr,VarName);
  else if (type == "chars")
    return new GlobalVariable(intPtr8,false,GlobalValue::WeakAnyLinkage,nullptr,VarName);
  else if (type == "string")
    return new GlobalVariable(intPtr8,false,GlobalValue::WeakAnyLinkage,nullptr,VarName);
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
  //int len = strlen(Val);
  //Val[len+1] = '\0';
  strcat((char*)Val, "\0");
  StringRef str(Val);
  return Builder.CreateGlobalStringPtr(str);
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
  cout << Type << endl;
  return Builder.CreateLoad(V, Name);
}

Value* globalVarExprAST::Codegen()
{
  if (NamedValues.find(Name) == NamedValues.end())
  {
    Value* Alloca;
    Value* Initial;
    if (Type == "intArray" || Type == "doubleArray" || Type == "charArray")
    {
      //get the size of the array
      int arrSize = dynamic_cast<IntExprAST*>(arrayIdx)->Val;  
      //store the size into a an llvm int
      Value* size = ConstantInt::get(Type::getInt32Ty(getGlobalContext()),arrSize);
      //create teh type for an array
      ArrayType* ArrayTy = ArrayType::get(Type::getInt32Ty(getGlobalContext()), arrSize);
      //Allocate the array??
      Alloca = new GlobalVariable(ArrayTy,false,GlobalValue::WeakAnyLinkage,nullptr,Name);
      //store the alloca in the symbol table
      NamedValues[Name] = Alloca;
      //return the alloca
      return Alloca;
    }
    if(Initd) //if initialized
      Initial = Initd->Codegen();
    else
    {
      if (Type == "double")
        Initial = ConstantFP::get(Type::getDoubleTy(getGlobalContext()),0.0);
      else if (Type == "doubles")
        Initial = ConstantPointerNull::get(doublePtr);
      else if (Type == "int")
        Initial = ConstantInt::get(Type::getInt32Ty(getGlobalContext()), 0);
      else if (Type == "ints")
        Initial = ConstantPointerNull::get(intPtr32);
      else if (Type == "char")
        Initial = ConstantInt::get(Type::getInt8Ty(getGlobalContext()), 0);
      else if (Type == "chars")
        Initial = ConstantPointerNull::get(intPtr8);
      else if (Type == "string")
        Initial = ConstantInt::get(Type::getInt8Ty(getGlobalContext()), 0);
    }
    Alloca = CreateEntryBlockGlobal(Name,Type);
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

Value* VarInitExprAST::Codegen()
{
  if (NamedValues.find(Name) == NamedValues.end())
  {
    AllocaInst* Alloca;
    Function* F = Builder.GetInsertBlock()->getParent();
    Value* Initial;
    if (Type == "intArray" || Type == "doubleArray" || Type == "charArray")
    {
      //get the size of the array
      int arrSize = dynamic_cast<IntExprAST*>(arrayIdx)->Val;  
      //store the size into a an llvm int
      Value* size = ConstantInt::get(Type::getInt32Ty(getGlobalContext()),arrSize);
      //create teh type for an array
      ArrayType* ArrayTy = ArrayType::get(Type::getInt32Ty(getGlobalContext()), arrSize);
      //Allocate the array??
      Alloca = Builder.CreateAlloca(ArrayTy,0);  
      //store the alloca in the symbol table
      NamedValues[Name] = Alloca;
      Alloca->setAlignment(4);
      //return the alloca
      return Alloca;
    }
    if(Initd) //if initialized
      Initial = Initd->Codegen();
    else
    {
      if (Type == "double")
        Initial = ConstantFP::get(Type::getDoubleTy(getGlobalContext()),0.0);
      else if (Type == "doubles")
        Initial = ConstantPointerNull::get(doublePtr);
      else if (Type == "int")
        Initial = ConstantInt::get(Type::getInt32Ty(getGlobalContext()), 0);
      else if (Type == "ints")
        Initial = ConstantPointerNull::get(intPtr32);
      else if (Type == "char")
        Initial = ConstantInt::get(Type::getInt8Ty(getGlobalContext()), 0);
      else if (Type == "chars")
        Initial = ConstantPointerNull::get(intPtr8);
      else if (Type == "string")
        Initial = ConstantInt::get(Type::getInt8Ty(getGlobalContext()), 0);
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

Value* ArrayIndexAST::Codegen() 
{
  VariableExprAST* Var = new VariableExprAST(VarName,typeTab[VarName]);
  Value* RHS = Var->Codegen();
  string rtype = Var->getType();
  if (Index->getType() == "null")
  {
    Value* zero = ConstantInt::get(Type::getInt32Ty(getGlobalContext()),0);
    vector<Value*> idx;
    idx.push_back(zero);
    idx.push_back(zero);
    return Builder.CreateInBoundsGEP(NamedValues[VarName],idx);
  }
  if (rtype != "intArray" && rtype != "doubleArray" && rtype != "charArray" && rtype != "ints" && rtype != "doubles" && rtype != "chars")
  {
#ifdef DEBUG
    dumpVars();
#endif
    cerr << "\033[31m ERROR: \033[37m Variable is not an array: " << Var->getName() << endl;
    exit(EXIT_FAILURE);
  }
  else
  {
    Value* LHS = Index->Codegen();
    if (LHS == 0)
    {
      return 0;
    }
    Value* ptr = NamedValues[VarName];
    if (rtype == "ints" || rtype == "doubles" || rtype == "chars")
    {
      ptr = Builder.CreateLoad(ptr);
      Value* zero = ConstantInt::get(Type::getInt32Ty(getGlobalContext()),0);
      vector<Value*> idx;
      idx.push_back(LHS);
      return Builder.CreateInBoundsGEP(ptr,idx);
    }
    Value* zero = ConstantInt::get(Type::getInt32Ty(getGlobalContext()),0);
    vector<Value*> idx;
    idx.push_back(zero);
    idx.push_back(LHS);
    return Builder.CreateInBoundsGEP(ptr,idx);
  }
}
