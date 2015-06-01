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

Value* ClassAST::Codegen()
{
  //Populate the Object's symbol table. This includes not only the variables, but methods as well
  vector<FunctionAST*>::iterator it;
  Value* Constructor = Init->Codegen();
  symbols.addMethod(Name,Constructor);
  for(it = FunctionList.begin(); it != FunctionList.end(); it++)
  {
    Value* func = *it->Codegen();
    if (!func)
      return 0;
    //May have to do some renameing of the function in here so it doesn't clash with other functions in IR
    symbols.addMethod((*it)->getName(), func);
  }
  vector<ExprAST*>::iterator vi;
  for(vi = Vars.begin(); vi != Vars.end(); vi++)
  {
    Value* var = (*it)->Codegen();
    if (!var)
      return 0;
    //May have to do some renameing of the variables in here so it doesn't clash with other vars in the IR
    symbols.addVar((*it)->getName(), var);
  }
}
