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

extern Module* theModule;
extern SymbolTable<string,Value*> NamedValues;
extern PointerType* intPtr32;
extern PointerType* intPtr8;
extern PointerType* doublePtr;
extern IRBuilder<> Builder;
map<string,StructType*> classes;
Type* i32 = Type::getInt32Ty(getGlobalContext());
Type* i8 = Type::getInt8Ty(getGlobalContext());
Type* d64 = Type::getDoubleTy(getGlobalContext());

Type* ObjectSymbolTable::getLLVMType(string key)
{
  string type = types[key];
  if (type == "int")
    return i32;
  else if (type == "ints")
    return intPtr32;
  else if (type == "double")
    return d64;
  else if (type == "doubles")
    return doublePtr;
  else if (type == "char")
    return i8;
  else if (type == "chars")
    return intPtr8;
}

Value* ClassAST::Codegen()
{
  //Populate the Object's symbol table. This includes not only the variables, but methods as well
  vector<Type*> structTys;    //Holds the LLVM::Type's for all the elements in the struct/object
  vector<FunctionAST*>::iterator it;
  if(Init)
  {
    Value* Constructor = Init->Codegen();
    symbols.addMethod(Name,Constructor);
  }
  if(FunctionList.size() > 0)
  {
    for(it = FunctionList.begin(); it != FunctionList.end(); it++)
    {
      Value* func = (*it)->Codegen();
      if (!func)
        return 0;
      //May have to do some renameing of the function in here so it doesn't clash with other functions in IR
      symbols.addMethod((*it)->getName(), func);
    }
  }
  if(Vars.size() > 0)
  {
    for(auto vi : Vars)
    {
      string n = vi->getName();
      string t = vi->getType();
      symbols.setType(n,t);
      Value* var = vi->Codegen();
      if (!var)
        return 0;
      //May have to do some renameing of the variables in here so it doesn't clash with other vars in the IR
      symbols.addVar(n, var);
      structTys.push_back(symbols.getLLVMType(vi->getName()));
    }
  }

  //create the type for the struct
  StructType* structReg = StructType::create(getGlobalContext(), structTys,StringRef(Name));
  PointerType* structRegPtr = PointerType::get(structReg,0);
  classes[Name] = structReg;
  return Builder.CreateAlloca(structReg);//ConstantInt::get(Type::getInt32Ty(getGlobalContext()), 0);
  //WHY IS NOTHING HAPPENING HERE????????????????????
}

Value* ObjectInitAST::Codegen()
{
  Value* alloca = Builder.CreateAlloca(classes[Object],0,Name.c_str());
  vector<Value*> idxs;
  Value* zero = ConstantInt::get(i32,0);
  idxs.push_back(zero);
  idxs.push_back(zero);
  return Builder.CreateStructGEP(classes[Object],alloca,0);
}
