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
extern IRBuilder<> Builder;
extern map<string, AllocaInst*> NamedValues;
extern PointerType* intPtr32; 
extern PointerType* intPtr8; 
extern PointerType* doublePtr;

extern void dumpVars();

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

Value* CallExprAST::Codegen()
{
  //Look up the name in the global module
  Function* CalleeF = theModule->getFunction(Callee);
  if (CalleeF == 0)
  {
#ifdef DEBUG
    dumpVars();
#endif
    cerr << "\033[31m ERROR: \033[37m Unknown Function Reference" << endl;
    exit(EXIT_FAILURE);
  }
  if (CalleeF->arg_size() != Args.size())
  {
#ifdef DEBUG
    dumpVars();
#endif
    cerr << "\033[31m ERROR: \033[37m Incorrect number of arguements" << endl;
    exit(EXIT_FAILURE);
  }
  vector<Value*> ArgsV;
  for (unsigned i = 0, e = Args.size(); i != e; ++i)
  {
    ArgsV.push_back(Args[i]->Codegen());
    if (ArgsV.back() == 0)
      return 0;
  }
  return Builder.CreateCall(CalleeF,ArgsV,"calltmp");
}


static Type *typeOf(VarInitExprAST* type) 
{
  if (type->getType() == "int") 
  {
    return Type::getInt32Ty(getGlobalContext());
  }
  else if (type->getType() == "double") 
  {
    return Type::getDoubleTy(getGlobalContext());
  }
  else if (type->getType() == "char") 
  {
    return Type::getInt8Ty(getGlobalContext());
  }
  else if (type->getType() == "chars") 
  {
    return intPtr8;
  }
  else if (type->getType() == "ints") 
  {
    return intPtr32;
  }
  else if (type->getType() == "doubles") 
  {
    return doublePtr;
  }
  else if (type->getType() == "string")
  {
    return intPtr8;
  }
  return 0;
}

typedef std::vector<VarInitExprAST*> VariableList;

Function* PrototypeAST::Codegen()
{
  vector<Type*> argTypes;
  VariableList::const_iterator it;
  for (it = Args.begin(); it != Args.end(); it++)
  {
    argTypes.push_back(typeOf((*it)));
  }
  FunctionType* FT;
  Function* F;
  if (Args.empty())
  {
    if (Ty == "double")
      FT = FunctionType::get(Builder.getDoubleTy(),false);
    else if (Ty == "doubles")
      FT = FunctionType::get(doublePtr,false);
    else if (Ty == "int")
      FT = FunctionType::get(Builder.getInt32Ty(),false);
    else if (Ty == "ints")
      FT = FunctionType::get(intPtr32, false);
    else if (Ty == "char")
      FT = FunctionType::get(Builder.getInt8Ty(),false);
    else if (Ty == "chars")
      FT = FunctionType::get(intPtr8, false);
    else if (Ty == "string")
      FT = FunctionType::get(intPtr8, false);
  }
  else 
  {
    if (Ty == "double")
      FT = FunctionType::get(Builder.getDoubleTy(),makeArrayRef(argTypes),false);
    else if (Ty == "doubles")
      FT = FunctionType::get(doublePtr,makeArrayRef(argTypes),false);
    else if (Ty == "int")
      FT = FunctionType::get(Builder.getInt32Ty(),makeArrayRef(argTypes),false);
    else if (Ty == "ints")
      FT = FunctionType::get(intPtr32,makeArrayRef(argTypes),false);
    else if (Ty == "char")
      FT = FunctionType::get(Builder.getInt8Ty(),makeArrayRef(argTypes),false);
    else if (Ty == "chars")
      FT = FunctionType::get(intPtr8,makeArrayRef(argTypes),false);
    else if (Ty == "string")
      FT = FunctionType::get(intPtr8,makeArrayRef(argTypes),false);
  }
  F = Function::Create(FT, Function::ExternalLinkage, Name, theModule);
  if(F->getName() != Name)
  {
    F->eraseFromParent();
    F = theModule->getFunction(Name);

    if(!F->empty())
    {
#ifdef DEBUG
    dumpVars();
#endif
      cerr << "\033[31m ERROR: \033[37m Redfinition of Function" << endl;
      exit(EXIT_FAILURE);
    }

    if(F->arg_size() != Args.size())
    {
#ifdef DEBUG
    dumpVars();
#endif
      cerr << "\033[31m ERROR: \033[37m Incorrect number of arguments for Function" << endl;
      exit(EXIT_FAILURE);
    }
  }

  unsigned Idx = 0;
  if (!Args.empty())
  {
    for (Function::arg_iterator AI = F->arg_begin(); Idx != Args.size(); ++AI, ++Idx)
    {
      AI->setName(Args[Idx]->getName());
    }
  }
  return F;
}

void PrototypeAST::CreateArgumentAllocas(Function *F)
{
  Function::arg_iterator AI = F->arg_begin();
  for (unsigned Idx = 0, e = Args.size(); Idx != e; ++Idx, ++AI)
  {
    AllocaInst* Alloca = CreateEntryBlockAlloca(Args[Idx]->getName(), Args[Idx]->getType());
    Builder.CreateStore(AI,Alloca);
    NamedValues[Args[Idx]->getName()] = Alloca;
    typeTab[Args[Idx]->getName()] = Args[Idx]->getType();
  }
}

Function* FunctionAST::Codegen()
{
  Function* theFunction = Proto->Codegen();
  if(theFunction == 0)
    return 0;

  BasicBlock* BB = BasicBlock::Create(getGlobalContext(),"entry",theFunction);
  Builder.SetInsertPoint(BB);

  Proto->CreateArgumentAllocas(theFunction);

  vector<ExprAST*>::iterator it = Body.begin();
  Value* last;
  for(it = Body.begin(); it != Body.end(); ++it)
  {
    last = (*it)->Codegen();
    if (!last)
      break;
  }
  
  if(last)
  {
    Builder.CreateRet(last);
    verifyFunction(*theFunction);
    NamedValues.clear();
    return theFunction;
  }
  //If it gets here there's an error! erase the function
  theFunction->eraseFromParent();
  return 0;
}
