#include <iostream>
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


void createfuncDef(FunctionAST* F)
{
  Function* func = F->Codegen();
  if(!func)
  {
    cerr << "\033[31m ERROR: \033[37m Error in reading function " << endl;
    exit(EXIT_FAILURE);
  }
  func->dump();
}

void createExtern(PrototypeAST* P)
{
  Function* func = P->Codegen();
  if(!func)
  {
    cerr << "\033[31m ERROR: \033[37m Error in declaring extern " << endl;
    exit(EXIT_FAILURE);
  }
  func->dump();
}

void createVarDef(VarInitExprAST* V)
{
  Value* F = V->Codegen();
  if (!F)
  {
    cerr << "\033[31m ERROR: \033[37m Error creating variable " << endl;
    exit(EXIT_FAILURE);
  }
  F->dump();
}

void createBinOp(BinaryExprAST* V)
{
  Value* F = V->Codegen();
  if (!F)
  {
    cerr << "\033[31m ERROR: \033[37m Error creating Binary Operation" << endl;
    exit(EXIT_FAILURE);
  }
  F->dump();
}
