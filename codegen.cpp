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

extern FILE* yyin;

Module *theModule;
static IRBuilder<> Builder(getGlobalContext());
map<string, AllocaInst*> NamedValues;
FunctionPassManager *theFPM;
PointerType* intPtr32 = PointerType::get(Type::getInt32Ty(getGlobalContext()), 0);
PointerType* intPtr8 = PointerType::get(Type::getInt8Ty(getGlobalContext()), 0);
PointerType* doublePtr = PointerType::get(Type::getDoubleTy(getGlobalContext()), 0);

void dumpVars()
{
  map<string,AllocaInst*>::iterator it;
  cout << "\nDumping vars: \n";
  for(it=NamedValues.begin();it!=NamedValues.end(); it++)
  {
    cout << it-> first << ": " << it->second << endl;
  }
}

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

Value* IntExprAST::Codegen()
{
  return ConstantInt::get(Type::getInt32Ty(getGlobalContext()), Val);
}

Value* DoubleExprAST::Codegen()
{
  return ConstantFP::get(Type::getDoubleTy(getGlobalContext()), Val);
}

Value* CharExprAST::Codegen()
{
  return ConstantInt::get(Type::getInt8Ty(getGlobalContext()), Val);
}

Value* stringExprAST::Codegen()
{
  int i = 0;
  vector<uint8_t> v;
  while (i<Size)
  {
    v.push_back(Val[i]);
    i++;
  } 
  ArrayRef<uint8_t> chars(v);
  return ConstantDataArray::get(getGlobalContext(), chars);
}

Value* ForExprAST::Codegen()
{
  Function *TheFunction = Builder.GetInsertBlock()->getParent();
  BasicBlock *PreheaderBB = Builder.GetInsertBlock();
  // Get alloca for the variable in the entry block.
  AllocaInst *Alloca = NamedValues[VarName];
  dumpVars();
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
  {
    StepVal = ConstantInt::get(Type::getInt32Ty(getGlobalContext()), 1);
  }


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

Value* VariableExprAST::Codegen()
{
  //does var exist?
  Value* V = NamedValues[Name];
  if (V == 0)
  {
    cerr << "\033[31m ERROR: \033[37m Unknown Variable Reference: " << Name << endl;
    exit(EXIT_FAILURE);
  }
  return Builder.CreateLoad(V, Name);
}

Value* VarInitExprAST::Codegen()
{
  cout << "Codegen'ing new variable...\n";
  if (NamedValues.find(Name) == NamedValues.end())
  {
    AllocaInst* Alloca;
    vector<AllocaInst* > oldBindings;

    Function* F = Builder.GetInsertBlock()->getParent();

    Value* Initial;
    if(Initd) //if initialized
      Initial = Initd->Codegen();
    else
    {
      if (Type == "double")
        Initial = ConstantFP::get(Type::getDoubleTy(getGlobalContext()),0.0);
      else if (Type == "doubles")
        Initial = ConstantFP::get(Type::getDoubleTy(getGlobalContext()),0.0);
      else if (Type == "int")
        Initial = ConstantInt::get(Type::getInt32Ty(getGlobalContext()), 0);
      else if (Type == "ints")
        Initial = ConstantInt::get(Type::getInt32Ty(getGlobalContext()), 0);
      else if (Type == "char")
        Initial = ConstantInt::get(Type::getInt8Ty(getGlobalContext()), 0);
      else if (Type == "chars")
        Initial = ConstantInt::get(Type::getInt8Ty(getGlobalContext()), 0);
      else if (Type == "string")
        Initial = ConstantDataArray::getString(getGlobalContext(), "");
    }
    Alloca = CreateEntryBlockAlloca(Name,Type);
    NamedValues[Name] = Alloca;
    dumpVars();
    return Builder.CreateStore(Initial,Alloca);
  }
  else
  {
    cerr << "\033[31m ERROR: \033[37m Variable already exists: " << Name << endl;
    exit(EXIT_FAILURE);
  }
}

Value* BinaryExprAST::Codegen()
{
  if(Op == '=')
  {
    cout << "Codegen'ing binary op....\n";
    VariableExprAST* LHSE = dynamic_cast<VariableExprAST*>(LHS);
    if (!LHSE)
    {
      cerr << "\033[31m ERROR: \033[37m lvalue must be a variable: " << endl;
      exit(EXIT_FAILURE);
    }

    //Codegen the right hand side.
    Value* Val = RHS->Codegen();

    if(Val == 0)
    {
      cerr << "\033[31m ERROR: \033[37m Invalid Value " << endl;
      exit(EXIT_FAILURE);
    }
    //Look up the name
    Value* Variable = NamedValues[LHSE->getName()];
    if (!Variable)
    {
      cerr << "\033[31m ERROR: \033[37m variable not declared!: " << LHSE->getName() << endl;
      exit(EXIT_FAILURE);
    }
    return Builder.CreateStore(Val,Variable);
  }

  Value *L = LHS->Codegen();
  Value *R = RHS->Codegen();
  if (L == 0 || R == 0) return 0;

  //Use the getValueID method on the R and L values to check for types/type clashes
  bool isInt = false;
  cout << L->getValueID() << "\t" << R->getValueID() << endl;
  if (L->getValueID() == 11 && R->getValueID() == 11)
  {
    isInt = true;
  } 
  else if (L->getValueID() == 12 && R->getValueID() == 12)
  {
    isInt = false;
  } 
  else if (L->getValueID() == 49 && R->getValueID() == 49)
  {
    if (dynamic_cast<VariableExprAST*>(LHS)->getType() == "int" && dynamic_cast<VariableExprAST*>(RHS)->getType() == "int")
      isInt = true;
    else if (dynamic_cast<VariableExprAST*>(LHS)->getType() == "double" && dynamic_cast<VariableExprAST*>(RHS)->getType() == "double")
      isInt = false;
    else
      goto err;
  } 
  else
  {
err:
    cerr << "\033[31m ERROR: \033[37m Types do not match!" << endl;
    exit(EXIT_FAILURE);
  }
  
  switch (Op) 
  {
    case '+': 
      if(!isInt)
        return Builder.CreateFAdd(L, R, "addtmp");
      else
        return Builder.CreateAdd(L, R, "addtmp");
    case '-': 
      if(!isInt)
        return Builder.CreateFSub(L, R, "subtmp");
      else
        return Builder.CreateSub(L, R, "addtmp");
    case '*': 
      if(!isInt)
        return Builder.CreateFMul(L, R, "multmp");    
      else
        return Builder.CreateMul(L, R, "addtmp");
    case '/':
      if(!isInt)
        return Builder.CreateFDiv(L, R, "divtmp");
      else
        return Builder.CreateUDiv(L, R, "addtmp");
    case '<':
      if(!isInt) {
        L = Builder.CreateFCmpULT(L, R, "cmptmp");
        return Builder.CreateUIToFP(L, Type::getDoubleTy(getGlobalContext()), "booltmp");
      }
      else 
      {
        L = Builder.CreateICmpULT(L, R, "cmptmp");
        return Builder.CreateSExt(L, Type::getInt32Ty(getGlobalContext()), "booltmp");
      }
    case '>':
      if(!isInt) {
        L = Builder.CreateFCmpUGT(L, R, "cmptmp");
        return Builder.CreateUIToFP(L, Type::getDoubleTy(getGlobalContext()), "booltmp");
      }
      else 
      {
        L = Builder.CreateICmpUGT(L, R, "cmptmp");
        return Builder.CreateSExt(L, Type::getInt32Ty(getGlobalContext()), "booltmp");
      }
    case 'G':
      if(!isInt) {
        L = Builder.CreateFCmpUGE(L, R, "cmptmp");
        return Builder.CreateUIToFP(L, Type::getDoubleTy(getGlobalContext()), "booltmp");
      }
      else 
      {
        L = Builder.CreateICmpUGE(L, R, "cmptmp");
        return Builder.CreateSExt(L, Type::getInt32Ty(getGlobalContext()), "booltmp");
      }
    case 'L':
      if(!isInt) {
        L = Builder.CreateFCmpULE(L, R, "cmptmp");
        return Builder.CreateUIToFP(L, Type::getDoubleTy(getGlobalContext()), "booltmp");
      }
      else 
      {
        L = Builder.CreateICmpULE(L, R, "cmptmp");
        return Builder.CreateSExt(L, Type::getInt32Ty(getGlobalContext()), "booltmp");
      }
    case 'N':
      if(!isInt) {
        L = Builder.CreateFCmpUNE(L, R, "cmptmp");
        return Builder.CreateUIToFP(L, Type::getDoubleTy(getGlobalContext()), "booltmp");
      }
      else 
      {
        L = Builder.CreateICmpNE(L, R, "cmptmp");
        return Builder.CreateSExt(L, Type::getInt32Ty(getGlobalContext()), "booltmp");
      }
    case 'E':
      if(!isInt) {
        L = Builder.CreateFCmpUEQ(L, R, "cmptmp");
        return Builder.CreateUIToFP(L, Type::getDoubleTy(getGlobalContext()), "booltmp");
      }
      else 
      {
        L = Builder.CreateICmpEQ(L, R, "cmptmp");
        return Builder.CreateSExt(L, Type::getInt32Ty(getGlobalContext()), "booltmp");
      }
    default: break;
  }
  
  //if theres a custom binary op defined, get it.
  Function *F = theModule->getFunction(string("binary")+Op);
  assert(F && "binary operator not found!");
  
  Value *Ops[] = { L, R };
  return Builder.CreateCall(F, Ops, "binop"); 
}

Value* UnaryExprAST::Codegen()
{
  Value* R = RHS->Codegen();
  if (!R)
  {
      cerr << "\033[31m ERROR: \033[37m Invalid rval!: " <<  endl;
      exit(EXIT_FAILURE);
  }
  
  switch(Op)
  {
    case '^':
      if (typeTab[RHS->getName()] == "int")
      {
        AllocaInst* allocaPtr = NamedValues[RHS->getName()];
        return Builder.CreateGEP(allocaPtr,ConstantInt::get(Type::getInt32Ty(getGlobalContext()),0)); 
      }
      else if (typeTab[RHS->getName()] == "double")
      {
        AllocaInst* allocaPtr = NamedValues[RHS->getName()];
        return Builder.CreateGEP(allocaPtr,ConstantInt::get(Type::getDoubleTy(getGlobalContext()),0.0)); 
      }
      else if (typeTab[RHS->getName()] == "char")
      {
        AllocaInst* allocaPtr = NamedValues[RHS->getName()];
        return Builder.CreateGEP(allocaPtr,ConstantInt::get(Type::getInt8Ty(getGlobalContext()),0)); 
      }
    default: break;
  }

  Function *F = theModule->getFunction(string("unary")+Op);
  assert(F && "unary operator not found!");
  Value* ident = R;
  return Builder.CreateCall(F,R,"unop");
}

Value* CallExprAST::Codegen()
{
  //Look up the name in the global module
  Function* CalleeF = theModule->getFunction(Callee);
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
  string ty = Cond->getType();
  string condVar = dynamic_cast<BinaryExprAST*>(Cond)->getLHSVar();
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
    Tlast = (*it)->Codegen();

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
  PHINode *PN = Builder.CreatePHI(Type::getLabelTy(getGlobalContext()), 2, "iftmp");
  PN->addIncoming(ElseBB, Entry);
  PN->addIncoming(ThenBB, Entry);

  return Constant::getNullValue(Type::getDoubleTy(getGlobalContext()));
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
    return Type::getInt8PtrTy(getGlobalContext());
  }
  else if (type->getType() == "ints") 
  {
    return Type::getInt32PtrTy(getGlobalContext());
  }
  else if (type->getType() == "doubles") 
  {
    return Type::getDoublePtrTy(getGlobalContext());
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
      FT = FunctionType::get(Type::getDoublePtrTy(getGlobalContext()),false);
    else if (Ty == "int")
      FT = FunctionType::get(Builder.getInt32Ty(),false);
    else if (Ty == "ints")
      FT = FunctionType::get(Type::getInt32PtrTy(getGlobalContext()),false);
    else if (Ty == "char")
      FT = FunctionType::get(Builder.getInt8Ty(),false);
    else if (Ty == "chars")
      FT = FunctionType::get(Builder.getInt8PtrTy(),false);
  }
  else 
  {
    if (Ty == "double")
      FT = FunctionType::get(Builder.getDoubleTy(),makeArrayRef(argTypes),false);
    else if (Ty == "doubles")
      FT = FunctionType::get(Type::getDoublePtrTy(getGlobalContext()),makeArrayRef(argTypes),false);
    else if (Ty == "int")
      FT = FunctionType::get(Builder.getInt32Ty(),makeArrayRef(argTypes),false);
    else if (Ty == "ints")
      FT = FunctionType::get(Type::getInt32PtrTy(getGlobalContext()),makeArrayRef(argTypes),false);
    else if (Ty == "char")
      FT = FunctionType::get(Builder.getInt8Ty(),makeArrayRef(argTypes),false);
    else if (Ty == "chars")
      FT = FunctionType::get(Builder.getInt8PtrTy(),makeArrayRef(argTypes),false);
  }
  F = Function::Create(FT, Function::ExternalLinkage, Name, theModule);
  if(F->getName() != Name)
  {
    F->eraseFromParent();
    F = theModule->getFunction(Name);

    if(!F->empty())
    {
      cerr << "\033[31m ERROR: \033[37m Redfinition of Function" << endl;
      exit(EXIT_FAILURE);
    }

    if(F->arg_size() != Args.size())
    {
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
  //TODO: There needs to be a check of types for the arguments!
  Function::arg_iterator AI = F->arg_begin();
  for (unsigned Idx = 0, e = Args.size(); Idx != e; ++Idx, ++AI)
  {
    AllocaInst* Alloca = CreateEntryBlockAlloca(Args[Idx]->getName(), Args[Idx]->getType());
    Builder.CreateStore(AI,Alloca);
    NamedValues[Args[Idx]->getName()] = Alloca;
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
    return theFunction;
  }
  //If it gets here there's an error! erase the function
  theFunction->eraseFromParent();
  return 0;
}
