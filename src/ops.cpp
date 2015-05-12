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

extern FILE* yyin;
extern int lineNum;

extern Module *theModule;
extern IRBuilder<> Builder;
extern map<string, AllocaInst*> NamedValues;
extern PointerType* intPtr32;
extern PointerType* intPtr8;
extern PointerType* doublePtr;
extern void dumpVars();

Value* BinaryExprAST::Codegen()
{
  if(Op == '=')
  {
    bool isVar = false;
    ArrayIndexAST* LHSA = dynamic_cast<ArrayIndexAST*>(LHS);
    VariableExprAST* LHSE = dynamic_cast<VariableExprAST*>(LHS);

    if (!LHSA)
    {
        isVar = true;
    }
    if (!LHSE && !LHSA)
    {
#ifdef DEBUG
        dumpVars();
#endif
        cerr << "\033[31m ERROR: \033[37m lvalue must be a variable: " << endl;
        exit(EXIT_FAILURE);
    }

    //Codegen the right hand side.
    Value* Val = RHS->Codegen();
    if(Val == 0)
    {
#ifdef DEBUG
    dumpVars();
#endif
      cerr << "\033[31m ERROR: \033[37m Invalid Value " << endl;
      exit(EXIT_FAILURE);
    }
    //Look up the name
    Value* Variable;
    if (isVar)
      Variable = NamedValues[LHSE->getName()];
    else
      Variable = NamedValues[LHSA->getName()];
    if (!Variable)
    {
#ifdef DEBUG
    dumpVars();
#endif
      if (isVar)
        cerr << "\033[31m ERROR: \033[37m variable not declared!: " << LHSE->getName() << endl;
      else
        cerr << "\033[31m ERROR: \033[37m variable not declared!: " << LHSA->getName() << endl;

      exit(EXIT_FAILURE);
    }
    Value *L = LHS->Codegen();
    Value *R = RHS->Codegen();
    if (L == 0 || R == 0) return 0;

    string lty = LHS->getType();
    string rty = RHS->getType();
    if (lty == "int" && rty == "double")
      R = Builder.CreateFPToSI(R,Type::getInt32Ty(getGlobalContext()));
    else if (lty == "double" && rty == "int")
      R = Builder.CreateSIToFP(R,Type::getDoubleTy(getGlobalContext()));
    else if (lty == "intArray" || lty == "doubleArray" || lty == "charArray")
      return Builder.CreateStore(R,L);
    return Builder.CreateStore(R,Variable);
  }

  Value *L = LHS->Codegen();
  Value *R = RHS->Codegen();
  if (L == 0 || R == 0) return 0;

  string lty = LHS->getType();
  string rty = RHS->getType();

  if(rty == "ints" || lty == "ints" || lty == "doubles" || rty == "doubles" || rty == "chars" || lty == "chars")
  {
   
    bool ptrIsLeft = true;

    if(lty != "ints" && lty != "doubles" && lty != "chars")
    {
      ptrIsLeft = false;
    }
    if(!ptrIsLeft && (rty != "ints" && rty != "doubles" && rty != "chars"))
    {
#ifdef DEBUG
    dumpVars();
#endif
      cerr << "\033[31m ERROR: \033[37m Binary Operation can not be done!" << endl;
      exit(EXIT_FAILURE);
    }

    Value* iptr;
    switch(Op)
    {
      case '+':
        Value* afterAdd;
        if(ptrIsLeft)
        {
          iptr = Builder.CreatePtrToInt(L,Type::getInt32Ty(getGlobalContext()));
          Value* step;
          if(lty == "ints")
            step = Builder.CreateMul(R,ConstantInt::get(Type::getInt32Ty(getGlobalContext()),4));
          else if(lty == "doubles")
            step = Builder.CreateMul(R,ConstantInt::get(Type::getInt32Ty(getGlobalContext()),8));
          else if(lty == "chars")
            step = Builder.CreateMul(R,ConstantInt::get(Type::getInt32Ty(getGlobalContext()),1));
          afterAdd = Builder.CreateAdd(iptr,step,"ptrtmp");
          if(lty == "ints")
            return Builder.CreateIntToPtr(afterAdd,intPtr32);
          else if(lty == "doubles")
            return Builder.CreateIntToPtr(afterAdd,doublePtr);
          else if(lty == "chars")
            return Builder.CreateIntToPtr(afterAdd,intPtr8);
        }
        else
        {
          iptr = Builder.CreatePtrToInt(R,Type::getInt32Ty(getGlobalContext()));
          Value* step;
          if(rty == "ints")
            step = Builder.CreateMul(L,ConstantInt::get(Type::getInt32Ty(getGlobalContext()),4));
          else if(rty == "doubles")
            step = Builder.CreateMul(L,ConstantInt::get(Type::getInt32Ty(getGlobalContext()),8));
          else if(rty == "chars")
            step = Builder.CreateMul(L,ConstantInt::get(Type::getInt32Ty(getGlobalContext()),1));
          afterAdd = Builder.CreateAdd(iptr,L,"ptrtmp");
          if(rty == "ints")
            return Builder.CreateIntToPtr(afterAdd,intPtr32);
          else if(rty == "doubles")
            return Builder.CreateIntToPtr(afterAdd,doublePtr);
          else if(rty == "chars")
            return Builder.CreateIntToPtr(afterAdd,intPtr8);
        }
      case '-':
        Value* afterSub;
        if(ptrIsLeft)
        {
          iptr = Builder.CreatePtrToInt(L,Type::getInt32Ty(getGlobalContext()));
          Value* step;
          if(lty == "ints")
            step = Builder.CreateMul(R,ConstantInt::get(Type::getInt32Ty(getGlobalContext()),4));
          else if(lty == "doubles")
            step = Builder.CreateMul(R,ConstantInt::get(Type::getInt32Ty(getGlobalContext()),8));
          else if(lty == "chars")
            step = Builder.CreateMul(R,ConstantInt::get(Type::getInt32Ty(getGlobalContext()),1));
          afterSub = Builder.CreateSub(iptr,step,"ptrtmp");
          if(lty == "ints")
            return Builder.CreateIntToPtr(afterSub,intPtr32);
          else if(lty == "doubles")
            return Builder.CreateIntToPtr(afterSub,doublePtr);
          else if(lty == "chars")
            return Builder.CreateIntToPtr(afterSub,intPtr8);
        }
        else
        {
          iptr = Builder.CreatePtrToInt(R,Type::getInt32Ty(getGlobalContext()));
          Value* step;
          if(rty == "ints")
            step = Builder.CreateMul(L,ConstantInt::get(Type::getInt32Ty(getGlobalContext()),4));
          else if(rty == "doubles")
            step = Builder.CreateMul(L,ConstantInt::get(Type::getInt32Ty(getGlobalContext()),8));
          else if(rty == "chars")
            step = Builder.CreateMul(L,ConstantInt::get(Type::getInt32Ty(getGlobalContext()),1));
          afterSub = Builder.CreateSub(iptr,L,"ptrtmp");
          if(rty == "ints")
            return Builder.CreateIntToPtr(afterSub,intPtr32);
          else if(rty == "doubles")
            return Builder.CreateIntToPtr(afterSub,doublePtr);
          else if(rty == "chars")
            return Builder.CreateIntToPtr(afterSub,intPtr8);
        }
      case '*':
        Value* afterMul;
        if(ptrIsLeft)
        {
          iptr = Builder.CreatePtrToInt(L,Type::getInt32Ty(getGlobalContext()));
          Value* step;
          if(lty == "ints")
            step = Builder.CreateMul(R,ConstantInt::get(Type::getInt32Ty(getGlobalContext()),4));
          else if(lty == "doubles")
            step = Builder.CreateMul(R,ConstantInt::get(Type::getInt32Ty(getGlobalContext()),8));
          else if(lty == "chars")
            step = Builder.CreateMul(R,ConstantInt::get(Type::getInt32Ty(getGlobalContext()),1));
          afterMul = Builder.CreateMul(iptr,step,"ptrtmp");
          if(lty == "ints")
            return Builder.CreateIntToPtr(afterMul,intPtr32);
          else if(lty == "doubles")
            return Builder.CreateIntToPtr(afterMul,doublePtr);
          else if(lty == "chars")
            return Builder.CreateIntToPtr(afterMul,intPtr8);
        }
        else
        {
          iptr = Builder.CreatePtrToInt(R,Type::getInt32Ty(getGlobalContext()));
          Value* step;
          if(rty == "ints")
            step = Builder.CreateMul(L,ConstantInt::get(Type::getInt32Ty(getGlobalContext()),4));
          else if(rty == "doubles")
            step = Builder.CreateMul(L,ConstantInt::get(Type::getInt32Ty(getGlobalContext()),8));
          else if(rty == "chars")
            step = Builder.CreateMul(L,ConstantInt::get(Type::getInt32Ty(getGlobalContext()),1));
          afterMul = Builder.CreateMul(iptr,L,"ptrtmp");
          if(rty == "ints")
            return Builder.CreateIntToPtr(afterMul,intPtr32);
          else if(rty == "doubles")
            return Builder.CreateIntToPtr(afterMul,doublePtr);
          else if(rty == "chars")
            return Builder.CreateIntToPtr(afterMul,intPtr8);
        }
      case '/':
        Value* afterDiv;
        if(ptrIsLeft)
        {
          iptr = Builder.CreatePtrToInt(L,Type::getInt32Ty(getGlobalContext()));
          Value* step;
          if(lty == "ints")
            step = Builder.CreateMul(R,ConstantInt::get(Type::getInt32Ty(getGlobalContext()),4));
          else if(lty == "doubles")
            step = Builder.CreateMul(R,ConstantInt::get(Type::getInt32Ty(getGlobalContext()),8));
          else if(lty == "chars")
            step = Builder.CreateMul(R,ConstantInt::get(Type::getInt32Ty(getGlobalContext()),1));
          afterDiv = Builder.CreateUDiv(iptr,step,"ptrtmp");
          if(lty == "ints")
            return Builder.CreateIntToPtr(afterDiv,intPtr32);
          else if(lty == "doubles")
            return Builder.CreateIntToPtr(afterDiv,doublePtr);
          else if(lty == "chars")
            return Builder.CreateIntToPtr(afterDiv,intPtr8);
        }
        else
        {
          iptr = Builder.CreatePtrToInt(R,Type::getInt32Ty(getGlobalContext()));
          Value* step;
          if(rty == "ints")
            step = Builder.CreateMul(L,ConstantInt::get(Type::getInt32Ty(getGlobalContext()),4));
          else if(rty == "doubles")
            step = Builder.CreateMul(L,ConstantInt::get(Type::getInt32Ty(getGlobalContext()),8));
          else if(rty == "chars")
            step = Builder.CreateMul(L,ConstantInt::get(Type::getInt32Ty(getGlobalContext()),1));
          afterDiv = Builder.CreateUDiv(iptr,L,"ptrtmp");
          if(rty == "ints")
            return Builder.CreateIntToPtr(afterDiv,intPtr32);
          else if(rty == "doubles")
            return Builder.CreateIntToPtr(afterDiv,doublePtr);
          else if(rty == "chars")
            return Builder.CreateIntToPtr(afterDiv,intPtr8);
        }
      default: break;
    }
  }
  //Use the getValueID method on the R and L values to check for types/type clashes
  bool isInt = false;
  if(lty == rty)
  {
    if (lty == "int")
      isInt = true;
    else if (lty == "double")
      isInt = false;
  } 
  else
  {
    if (lty == "int" && rty == "double")
      L = Builder.CreateSIToFP(L,Type::getDoubleTy(getGlobalContext()));
    else if (lty == "double" && rty == "int")
      R = Builder.CreateSIToFP(R,Type::getDoubleTy(getGlobalContext()));
    else
    {
err:
#ifdef DEBUG
      dumpVars();
#endif
      cerr << "\033[31m ERROR: "<< lineNum <<": \033[37m Types do not match!" << endl;
      exit(EXIT_FAILURE);
    }
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
      cout << isInt << endl;
      if(!isInt) {
        L = Builder.CreateFCmpULT(L, R, "cmptmp");
        return Builder.CreateSIToFP(L, Type::getDoubleTy(getGlobalContext()), "booltmp");
      }
      else 
      {
        L = Builder.CreateICmpULT(L, R, "cmptmp");
        return Builder.CreateSExt(L, Type::getInt32Ty(getGlobalContext()), "booltmp");
      }
    case '>':
      if(!isInt) {
        L = Builder.CreateFCmpUGT(L, R, "cmptmp");
        return Builder.CreateSIToFP(L, Type::getDoubleTy(getGlobalContext()), "booltmp");
      }
      else 
      {
        L = Builder.CreateICmpUGT(L, R, "cmptmp");
        return Builder.CreateSExt(L, Type::getInt32Ty(getGlobalContext()), "booltmp");
      }
    case 'G':
      if(!isInt) {
        L = Builder.CreateFCmpUGE(L, R, "cmptmp");
        return Builder.CreateSIToFP(L, Type::getDoubleTy(getGlobalContext()), "booltmp");
      }
      else 
      {
        L = Builder.CreateICmpUGE(L, R, "cmptmp");
        return Builder.CreateSExt(L, Type::getInt32Ty(getGlobalContext()), "booltmp");
      }
    case 'L':
      if(!isInt) {
        L = Builder.CreateFCmpULE(L, R, "cmptmp");
        return Builder.CreateSIToFP(L, Type::getDoubleTy(getGlobalContext()), "booltmp");
      }
      else 
      {
        L = Builder.CreateICmpULE(L, R, "cmptmp");
        return Builder.CreateSExt(L, Type::getInt32Ty(getGlobalContext()), "booltmp");
      }
    case 'N':
      if(!isInt) {
        L = Builder.CreateFCmpUNE(L, R, "cmptmp");
        return Builder.CreateSIToFP(L, Type::getDoubleTy(getGlobalContext()), "booltmp");
      }
      else 
      {
        L = Builder.CreateICmpNE(L, R, "cmptmp");
        return Builder.CreateSExt(L, Type::getInt32Ty(getGlobalContext()), "booltmp");
      }
    case 'E':
      if(!isInt) {
        L = Builder.CreateFCmpUEQ(L, R, "cmptmp");
        return Builder.CreateSIToFP(L, Type::getDoubleTy(getGlobalContext()), "booltmp");
      }
      else 
      {
        L = Builder.CreateICmpEQ(L, R, "cmptmp");
        return Builder.CreateSExt(L, Type::getInt32Ty(getGlobalContext()), "booltmp");
      }
    default: break;
  }
  return 0;
}

Value* UnaryExprAST::Codegen()
{
  Value* R = RHS->Codegen();
  if (!R)
  {
#ifdef DEBUG
    dumpVars();
#endif
      cerr << "\033[31m ERROR: \033[37m Invalid rval!: " <<  endl;
      exit(EXIT_FAILURE);
  }
  
  switch(Op)
  {
    case '^':
      if (typeTab[dynamic_cast<VariableExprAST*>(RHS)->getName()] == "int")
      {
        AllocaInst* allocaPtr = NamedValues[dynamic_cast<VariableExprAST*>(RHS)->getName()];
        return Builder.CreateGEP(allocaPtr,ConstantInt::get(Type::getInt32Ty(getGlobalContext()),0)); 
      }
      else if (typeTab[dynamic_cast<VariableExprAST*>(RHS)->getName()] == "double")
      {
        AllocaInst* allocaPtr = NamedValues[dynamic_cast<VariableExprAST*>(RHS)->getName()];
        return Builder.CreateGEP(allocaPtr,ConstantInt::get(Type::getDoubleTy(getGlobalContext()),0.0)); 
      }
      else if (typeTab[dynamic_cast<VariableExprAST*>(RHS)->getName()] == "char")
      {
        AllocaInst* allocaPtr = NamedValues[dynamic_cast<VariableExprAST*>(RHS)->getName()];
        return Builder.CreateGEP(allocaPtr,ConstantInt::get(Type::getInt8Ty(getGlobalContext()),0)); 
      }
    case '@':
      {
        if (typeTab[RHS->getName()] != "string" && typeTab[RHS->getName()] != "ints" && typeTab[RHS->getName()] != "doubles" && typeTab[RHS->getName()] != "chars" && typeTab[RHS->getName()] != "intArray" && typeTab[RHS->getName()] != "doubleArray" && typeTab[RHS->getName()] != "charArray")
        {
#ifdef DEBUG
          dumpVars();
#endif
          cerr << "\033[31m ERROR: \033[37m Invalid rvalue (" << RHS->getName() << ") for dereference operator!" << endl;
          exit(EXIT_FAILURE);
        }
        if (typeTab[RHS->getName()] != "intArray" && typeTab[RHS->getName()] != "doubleArray" && typeTab[RHS->getName()] != "charArray")
        {
          Value* gep = RHS->Codegen();
          AllocaInst* allocaPtr = NamedValues[RHS->getName()];
          //Value* derefPtr = Builder.CreateLoad(,"derefPtr");
          return Builder.CreateLoad(gep,"derefVal");
        }
        else
        {
          //is an array
          Value* allocaPtr = RHS->Codegen();  //returns a GEP ptr
          return Builder.CreateLoad(allocaPtr,"derefPtr");
         // return Builder.CreateLoad(derefPtr,"derefVal");
        }

      }
    case '!':
      {
        Value* R = RHS->Codegen();
        if (RHS->getType() == "int")
        {
          Value* cmp =  Builder.CreateICmpEQ(R,ConstantInt::get(Type::getInt32Ty(getGlobalContext()),0));
          return Builder.CreateSExt(cmp, Type::getInt32Ty(getGlobalContext()));
        }
        else if (RHS->getType() == "double")
        {
          Value* cmp = Builder.CreateFCmpOEQ(R,ConstantInt::get(Type::getDoubleTy(getGlobalContext()),0));
          return Builder.CreateSIToFP(cmp,Type::getDoubleTy(getGlobalContext()));
        }
        else
          return 0;
      }
    default: break;
  }
  return 0;
}

//Value* TypeCastExprAST::Codegen() 
//{
//  cout << "\t" << toType << endl;
//  cout << "\t" << typeTab[varName] << endl;
//  if (toType == typeTab[varName])
//    return 0;
//  else if (toType == "int" && typeTab[varName] == "double")
//    return Builder.CreateFPToSI(NamedValues[varName],Type::getInt32Ty(getGlobalContext()));
//  else if (toType == "double" && typeTab[varName] == "int")
//    return Builder.CreateSIToFP(NamedValues[varName],Type::getDoubleTy(getGlobalContext()));
//  else
//  {
//#ifdef DEBUG
//      dumpVars();
//#endif
//      cerr << "\033[31m ERROR: \033[37m Invalid rvalue (" << toType << ") for casting operation!" << endl;
//      exit(EXIT_FAILURE);
//
//  }
//}
