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

Value* ForExprAST::Codegen()
{
  Function *TheFunction = Builder.GetInsertBlock()->getParent();
  BasicBlock *PreheaderBB = Builder.GetInsertBlock();
  // Get alloca for the variable in the entry block.
  AllocaInst *Alloca = NamedValues[VarName];
#ifdef DEBUG
  dumpVars();
#endif
  Value *StartVal = dynamic_cast<IntExprAST*>(Start)->Codegen();
  if (!StartVal)
    return 0;
  // Store the value into the alloca.
  Builder.CreateStore(StartVal, Alloca);

  BasicBlock *LoopBB = BasicBlock::Create(getGlobalContext(), "loop", TheFunction);

  // Insert an explicit fall through from the current block to the LoopBB.
  Builder.CreateBr(LoopBB);
  Builder.SetInsertPoint(LoopBB);

  PHINode *Variable = Builder.CreatePHI(Type::getInt32Ty(getGlobalContext()), 2, VarName.c_str());
  Variable->addIncoming(StartVal, PreheaderBB);

  Value *EndCond = End->Codegen();
  if (EndCond == 0)
    return EndCond;


  Value *StepVal;
  if (Step) 
  {
    StepVal = Step->Codegen();
    if (StepVal == 0)
      return 0;
  } 
  else 
    StepVal = ConstantInt::get(Type::getInt32Ty(getGlobalContext()), 1);


  Value *CurVar = Builder.CreateLoad(Alloca, VarName.c_str());
  Value *NextVar = Builder.CreateAdd(CurVar, StepVal, "nextvar");
  Builder.CreateStore(NextVar, Alloca);

  EndCond = Builder.CreateICmpSLT(CurVar, EndCond, "loopcond");
  vector<ExprAST*>::iterator it = Body.begin();
  for(it = Body.begin(); it != Body.end(); it++)
  {
    if((*it)->Codegen() == 0)
      return 0;
  }


  BasicBlock *LoopEnd = Builder.GetInsertBlock();
  BasicBlock *AfterBB = BasicBlock::Create(getGlobalContext(), "afterloop", TheFunction);
  Builder.CreateCondBr(EndCond, LoopBB, AfterBB);
  Builder.SetInsertPoint(AfterBB);
  Variable->addIncoming(NextVar,LoopEnd);

  return Constant::getNullValue(Type::getDoubleTy(getGlobalContext()));
}

Value* IfExprAST::Codegen()
{
  Value* CondV = Cond->Codegen();
  if(CondV == 0)
    return 0;
  string ty = Cond->getType();
  if (ty == "double")
    Builder.CreateFCmpONE(CondV, ConstantFP::get(getGlobalContext(), APFloat(0.0)), "ifcond");
  else if (ty == "int")
    Builder.CreateICmpNE(CondV, ConstantInt::get(Type::getInt32Ty(getGlobalContext()), 0), "ifcond");
  else
    return 0;
  Function* theFunction = Builder.GetInsertBlock()->getParent();
  BasicBlock* Entry = Builder.GetInsertBlock();

  BasicBlock* ThenBB = BasicBlock::Create(getGlobalContext(), "then", theFunction);
  BasicBlock* ElseBB = BasicBlock::Create(getGlobalContext(), "else");
  BasicBlock* MergeBB = BasicBlock::Create(getGlobalContext(), "ifcont");

  if (ty == "double")
    CondV = Builder.CreateFPToSI(CondV, Type::getInt32Ty(getGlobalContext()));
  Value* sCond = Builder.CreateSExtOrTrunc(CondV, Type::getInt1Ty(getGlobalContext()));
  if(hasElse)
    Builder.CreateCondBr(sCond, ThenBB, ElseBB);
  else
    Builder.CreateCondBr(sCond, ThenBB, MergeBB);
  //Create Then block
  Builder.SetInsertPoint(ThenBB);

  vector<ExprAST*>::iterator it;
  Value* Tlast; 
  for (it = Then.begin(); it != Then.end(); it++)
  {
    Tlast = (*it)->Codegen();
  }

  Builder.CreateBr(MergeBB);
  ThenBB = Builder.GetInsertBlock();
  //Create Else Block
  Value* Elast;
  if(hasElse) 
  {
    theFunction->getBasicBlockList().push_back(ElseBB);
    Builder.SetInsertPoint(ElseBB);

    vector<ExprAST*>::iterator it;
    for (it = Else.begin(); it != Else.end(); it++)
      Elast = (*it)->Codegen();

    Builder.CreateBr(MergeBB);
    ElseBB = Builder.GetInsertBlock();
  }

  //Set up merge block
  theFunction->getBasicBlockList().push_back(MergeBB);
  Builder.SetInsertPoint(MergeBB);

  return Constant::getNullValue(Type::getDoubleTy(getGlobalContext()));
}
