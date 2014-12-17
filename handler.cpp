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
#include "jcode.tab.h"
#include "tree.h"
using namespace std;
using namespace llvm;

stack<ExprAST*> parseStack;

static void createfuncDef(FunctionAST* F)
{
  if(Function* func != F->Codegen())
  {
    cerr << "\033[31m ERROR: \033[37m Error in reading function " << endl;
    exit(EXIT_FAILURE);
  }
  func->dump();
}

static void createExtern(PrototypeAST* P)
{
  if(Function* func != P->Codegen())
  {
    cerr << "\033[31m ERROR: \033[37m Error in declaring extern " << endl;
    exit(EXIT_FAILURE);
  }
  func->dump();
}

static void createTLE(FunctionAST* F)
{
  if(Function* func != F->Codegen())
  {
    cerr << "\033[31m ERROR: \033[37m Error in TLE " << endl;
    exit(EXIT_FAILURE);
  }
  func->dump(); 
}
