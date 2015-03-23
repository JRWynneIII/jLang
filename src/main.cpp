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

IRBuilder<> Builder(getGlobalContext());
extern FILE* yyin;
extern vector<ExprAST*>* lines;
Module *theModule;
extern map<string, AllocaInst*> NamedValues;
extern FunctionPassManager *theFPM;

int main(int argc, char*argv[])
{
  LLVMContext &Context = getGlobalContext();
  theModule = new Module("jlangc", Context);

  
  if (argc >1)
    yyin = fopen(argv[1],"r");
  yyparse();

  //Codegen all the functions
  vector<ExprAST*>::iterator it;
  Value* cur;
  for(it = lines->begin(); it != lines->end(); it++)
  {
    cur = (*it)->Codegen();
    if(!cur)
    {
      cerr << "\033[31m INTERNAL ERROR: \033[37m Error in reading AST " << endl;
      exit(EXIT_FAILURE);
    }
  }

  FunctionPassManager opt(theModule);
  opt.add(createBasicAliasAnalysisPass());
  opt.add(new DataLayout("e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"));
  opt.add(createPromoteMemoryToRegisterPass());
  opt.add(createInstructionCombiningPass());
  opt.add(createReassociatePass());
  opt.add(createGVNPass());
  opt.add(createCFGSimplificationPass());
  opt.doInitialization();
#ifdef DEBUG
  theModule->dump();
#endif

  string Errors, ErrorCatch;
  raw_fd_ostream bcFile("t.ll", Errors, sys::fs::F_Binary);
  WriteBitcodeToFile(theModule,bcFile);
  return EXIT_SUCCESS;
}
