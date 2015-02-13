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
#include<fstream>
#include "jlang.tab.hpp"
#include "tree.h"
using namespace std;
using namespace llvm;


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

extern FILE* yyin;
extern vector<ExprAST*>* lines;

void loadModule(const char* name)
{
  string modName(name);
  modName += ".jl";
  FILE* yytmp = yyin;
  if(!(yyin = fopen(modName.c_str(),"r")))
  {
    cerr << "\033[31m ERROR: \033[37m Can not read module "<< name << "!" << endl;
    exit(EXIT_FAILURE);
  }
  yyparse();
  vector<ExprAST*>::iterator it;
  Value* cur;
  for(it = lines->begin(); it != lines->end(); it++)
  {
    cur = (*it)->Codegen();
    cout << cur;
    if(!cur)
    {
      cerr << "\033[31m INTERNAL ERROR: \033[37m Error in reading AST in module " << name << endl;
      exit(EXIT_FAILURE);
    }
  }
  lines->clear();
  yyin = yytmp;
}
