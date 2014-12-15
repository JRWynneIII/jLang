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
#include "tree.h"
using namespace std;

static Module *theModule;
static IRBuilder<> Builder(getGlobalContext());
static map<string, AllocaInst*> NamedValues;
static FunctionPassManager *theFPM;

static AllocaInst *CreateEntryBlockAlloca(Function *TheFunction, const string &VarName, string type) 
{
  IRBuilder<> TmpB(&TheFunction->getEntryBlock(), TheFunction->getEntryBlock().begin());
  if (type == "double")
    return TmpB.CreateAlloca(Type::getDoubleTy(getGlobalContext()), 0, VarName.c_str());
  else if (type == "int")
    return TmpB.CreateAlloca(Type::getInt64Ty(getGlobalContext()), 0, VarName.c_str());
  else if (type == "char")
    return TmpB.CreateAlloca(Type::getInt8Ty(getGlobalContext()), 0, VarName.c_str());
}

Value* IntExprAST::Codegen()
{
  return ConstantInt::get(Type::getInt64Ty(getGlobalContext()), Val);
}

Value* DoubleExprAST::Codegen()
{
  return ConstantFP::get(Type::getDoubleTy(getGlobalContext()), Val);
}

Value* VariableExprAST::Codegen()
{
  //does var exist?
  Value* V = NamedValues[Name];
  if (V == 0)
  {
    cerr << "\033[31m ERROR: \033[37m Unknown Variable Reference: " << Name << endl;
    exit(EXIT_FAILURE);
  }

  return Builder.CreateLoad(V, Name.c_str());
}

Value* BinaryExprAST::Codegen()
{
  if(Op == '=')
  {
    VariableExprAST* LHSE = dynamic_cast<VariableExprAST*>(LHS);
    if (!LHSE)
    {
      cerr << "\033[31m ERROR: \033[37m lvalue must be a variable: " << endl;
      exit(EXIT_FAILURE);
    }
    Value* Val = RHS->Codegen();
    if(Val == 0)
      return 0;
    //Look up the name
    Value* Variable = NamedValues[LHSE->getName()];
    if (Variable == 0)
    {
      cerr << "\033[31m ERROR: \033[37m variable not declared!: " << LHSE->getName() << endl;
      exit(EXIT_FAILURE);
    }
    Builder.CreateStore(Val,Variable);
    return Val;
  }

  Value *L = LHS->Codegen();
  Value *R = RHS->Codegen();
  if (L == 0 || R == 0) return 0;
  
  switch (Op) 
  {
    case '+': 
      return Builder.CreateFAdd(L, R, "addtmp");
    case '-': 
      return Builder.CreateFSub(L, R, "subtmp");
    case '*': 
      return Builder.CreateFMul(L, R, "multmp");    
    case '<':
      L = Builder.CreateFCmpULT(L, R, "cmptmp");
      return Builder.CreateUIToFP(L, Type::getDoubleTy(getGlobalContext()), "booltmp");
    default: break;
  }
  
  Function *F = TheModule->getFunction(string("binary")+Op);
  assert(F && "binary operator not found!");
  
  Value *Ops[] = { L, R };
  return Builder.CreateCall(F, Ops, "binop"); 
}

Value* CallExprAST::Codegen()
{
  //Look up the name in the global module
  Function* CalleeF = theModule->getFunction(CalleeF);
  if (CalleeF == 0)
  {
    cerr << "\033[31m ERROR: \033[37m Unknown Function Reference" << endl;
    exit(EXIT_FAILURE);
  }
  if (CalleeF->arg_size() != Args.size())
  {
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

Value* IfExprAST::Codegen()
{
  Value* CondV = Cond->Codegen();
  if(CondV == 0)
    return 0;
  Builder.CreateFCmpONE(CondV, ConstantFP::get(getGlobalContext(), APFloat(0.0)), "ifcond");
  Function* theFunction = Builder.GetInsertBlock()->getParent();

  BasicBlock* ThenBB = BasicBlock::Create(getGlobalContext(), "then", theFunction);
  BasicBlock* ElseBB = BasicBlock::Create(getGlobalContext(), "else");
  BasicBlock* MergeBB = BasicBlock::Create(getGlobalContext(), "ifcont");

  Builder.CreateCondBr(CondV, ThenBB, ElseBB);
  Builder.SetInsertPoint(ThenBB);

  Value* ThenV = Then->Codegen();
  if (ThenV == 0)
    return 0;
  Builder.CreateBr(MergeBB);
  ThenBB = Builder.GetInsertBlock();
  
  theFunction->getBasicBlockList().push_back(ElseBB);
  Builder.SetInsertPoint(ElseBB);

  Value* ElseV = Else->Codegen();
  if (ElseV == 0)
    return 0;

  Builder.CreateBr(MergeBB);
  ElseBB = Builder.GetInsertBlock();

  theFunction->getBasicBlockList().push_back(MergeBB);
  Builder.SetInsertPoint(MergeBB);
  PHINode* PN = Builder.CreatePHI(Type::getDoubleTy(getGlobalContext()),2,"iftmp");
  PN->addIncoming(ThenV,ThenBB);
  PN->addIncoming(ElseV,ElseBB);
  return PN;
}

Function* PrototypeAST::Codegen()
{
  vector<Type*> Doubles(Args.size(), Type::getDoubleTy(getGlobalContext()));
  FunctionType* FT = FunctionType::get(Type::getDoubleTy(getGlobalContext()),Doubles,false);
  Function* F = Function::Create(FT, Function::ExternalLinkage, Name, theModule);

  if(F->getName() != Name)
  {
    F->eraseFromParent();
    F = theModule->getFunction(Name);

    if(!F->empty())
    {
      cerr << "\033[31m ERROR: \033[37m Redfinition of Function" << F->getName() << endl;
      exit(EXIT_FAILURE);
    }

    if(F->args_size() != Args.size())
    {
      cerr << "\033[31m ERROR: \033[37m Incorrect number of arguments for Function" << F->getName() << endl;
      exit(EXIT_FAILURE);
    }

    unsigned Idx = 0;
    for (Function::arg_iterator AI = F->arg_begin(); Idx != Args.size(); ++AI, ++Idx)
    {
      NamedValues[Args[Idx]] = AI;
    }
    return F;
}

Function* FunctionAST::Codegen()
{
  NamedValues.clear();
  Function* theFunction = Proto->Codegen();
  if(TheFunction == 0)
    return 0;

  BasicBlock* BB = BasicBlock::Create(getGlobalContext(),"entry",theFunction);
  Builder.SetEntryPoint(BB);

  if(Value* RetVal = Body->Codegen())
  {
    Builder.CreateRet(RetVal);
    verifyFunction(*theFunction);
    theFPM->run(*theFunction);
    return theFunction;
  }
  //If it gets here there's an error! erase the function
  theFunction->eraseFromParent();
  return 0;
}

