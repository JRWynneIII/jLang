#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
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
#include "llvm/IR/LegacyPassManager.h"
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
extern SymbolTable<string, Value*> NamedValues;
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

  theModule->setDataLayout(DataLayout("e-m:e-i64:64-n32:64"));
  theModule->setTargetTriple("powerpc64le-unknown-linux-gnu");
  legacy::FunctionPassManager opt(theModule);
  opt.add(createAggressiveDCEPass());
  opt.add(createBasicAliasAnalysisPass());
  opt.add(createPromoteMemoryToRegisterPass());
  opt.add(createInstructionCombiningPass());
  opt.add(createReassociatePass());
  opt.add(createGVNPass());
  opt.add(createCFGSimplificationPass());
  opt.add(createVerifierPass());
  opt.doInitialization();
  for ( Module::iterator it = theModule->begin(); it != theModule->end(); ++it)
    opt.run(*it);
  opt.doFinalization();
#ifdef DEBUG
  theModule->dump();
#endif

  string Errors, ErrorCatch;
  int fd = open("t.ll", O_RDWR | O_CREAT | O_TRUNC);
  if (fd <= 0)
  {
    cerr << "FILE NOT OPENED\nfd = " << fd << endl;
    exit(EXIT_FAILURE);
  }
  raw_fd_ostream bcFile(fd, false);
  WriteBitcodeToFile(theModule,bcFile);
  return EXIT_SUCCESS;
}
