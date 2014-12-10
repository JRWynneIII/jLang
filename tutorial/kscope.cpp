//#include "llvm/Analysis/Verifier.h"
#include "llvm/IR/Verifier.h"           //comment for other workstation
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>
#include <vector>
using namespace llvm;

enum Token
{
  tok_eof = -1,

  tok_def = -2, tok_extern = -3,

  tok_identifier = -4, tok_number = -5
};

static Module *theModule;
static IRBuilder<> Builder(getGlobalContext());
static std::map<std::string, Value*> NamedValues;

static std::string IdentifierStr;  // Filled in if tok_identifier
static double NumVal;              // Filled in if tok_number

static int gettok() 
{
  static int LastChar = ' ';

  while (isspace(LastChar))
    LastChar = getchar();

  if (isalpha(LastChar)) 
  {
    IdentifierStr = LastChar;
    while (isalnum((LastChar = getchar())))
      IdentifierStr += LastChar;

    if (IdentifierStr == "def") return tok_def;
    if (IdentifierStr == "extern") return tok_extern;
    return tok_identifier;
  }

  if (isdigit(LastChar) || LastChar == '.') 
  {   
    std::string NumStr;
    do 
    {
      NumStr += LastChar;
      LastChar = getchar();
    } while (isdigit(LastChar) || LastChar == '.');

    NumVal = strtod(NumStr.c_str(), 0);
    return tok_number;
  }

  if (LastChar == '#') 
  {
    // Comment until end of line.
    do LastChar = getchar();
    while (LastChar != EOF && LastChar != '\n' && LastChar != '\r');
    
    if (LastChar != EOF)
      return gettok();
  }
  
  // Check for end of file.  Don't eat the EOF.
  if (LastChar == EOF)
    return tok_eof;

  // Otherwise, just return the character as its ascii value.
  int ThisChar = LastChar;
  LastChar = getchar();
  return ThisChar;
}

namespace 
{
class ExprAST 
{
public:
  virtual ~ExprAST() {}
  virtual Value *Codegen() = 0;
};

class NumberExprAST : public ExprAST 
{
  double Val;
public:
  NumberExprAST(double val) : Val(val) {}
  virtual Value *Codegen();
};

class VariableExprAST : public ExprAST {
  std::string Name;
public:
  VariableExprAST(const std::string &name) : Name(name) {}
  virtual Value *Codegen();
};

class BinaryExprAST : public ExprAST 
{
  char Op;
  ExprAST *LHS, *RHS;
public:
  BinaryExprAST(char op, ExprAST *lhs, ExprAST *rhs) : Op(op), LHS(lhs), RHS(rhs) {}
  virtual Value *Codegen();
};

class CallExprAST : public ExprAST 
{
  std::string Callee;
  std::vector<ExprAST*> Args;
public:
  CallExprAST(const std::string &callee, std::vector<ExprAST*> &args)
    : Callee(callee), Args(args) {}
  virtual Value *Codegen();
};

class PrototypeAST 
{
  std::string Name;
  std::vector<std::string> Args;
public:
  PrototypeAST(const std::string &name, const std::vector<std::string> &args)
    : Name(name), Args(args) {}
  Function *Codegen();
  
};

/// FunctionAST - This class represents a function definition itself.
class FunctionAST 
{
  PrototypeAST *Proto;
  ExprAST* Body;
public:
  FunctionAST(PrototypeAST *proto, ExprAST *body) : Proto(proto), Body(body) {}
  Function *Codegen();
};
} // end anonymous namespace

ExprAST *Error(const char *Str) { fprintf(stderr, "Error: %s\n", Str);return 0;}
Value *ErrorV(const char *Str) { Error(Str); return 0; }
PrototypeAST *ErrorP(const char *Str) { Error(Str); return 0; }
FunctionAST *ErrorF(const char *Str) { Error(Str); return 0; }

Value* NumberExprAST::Codegen()
{
  return ConstantFP::get(getGlobalContext(),APFloat(Val));
}

Value* VariableExprAST::Codegen()
{
  Value *V = NamedValues[Name];
  return V ? V : ErrorV("Unknown Variable Name");
}

Value* BinaryExprAST::Codegen()
{
  Value *L = LHS->Codegen();
  Value *R = RHS->Codegen();
  if(L == 0 || R == 0) return 0;

  switch(Op)
  {
    case '+': return Builder.CreateFAdd(L, R, "addtmp");
    case '-': return Builder.CreateFSub(L, R, "subtmp");
    case '*': return Builder.CreateFMul(L, R, "multmp");
    case '<':
      L = Builder.CreateFCmpULT(L, R, "cmptmp");
      return Builder.CreateUIToFP(L, Type::getDoubleTy(getGlobalContext()), "booltmp");
    default: return ErrorV("Invalid Binary Operator");
  }
}

Value* CallExprAST::Codegen()
{
  Function *CalleeF = theModule->getFunction(Callee);
  if (CalleeF == 0)
    return ErrorV("Unknown function ref!");
  if (CalleeF->arg_size() != Args.size())
    return ErrorV("Incorrect Num of arguements passed!");
  
  std::vector<Value*> ArgsV;
  for(unsigned i = 0, e=Args.size(); i!=e; ++i)
  {
    ArgsV.push_back(Args[i]->Codegen());
    if(ArgsV.back() == 0) return 0;
  }
  return Builder.CreateCall(CalleeF, ArgsV, "calltmp");
}

Function* PrototypeAST::Codegen()
{
  std::vector<Type*> Doubles(Args.size(), Type::getDoubleTy(getGlobalContext()));
  FunctionType *FT = FunctionType::get(Type::getDoubleTy(getGlobalContext()), Doubles, false);
  Function *F = Function::Create(FT, Function::ExternalLinkage, Name, theModule);
  if (F->getName() !=Name)
  {
    F->eraseFromParent();
    F = theModule->getFunction(Name);
    if (!F->empty())
    {
      ErrorF("Redefinition of function");
      return 0;
    }

    if(F->arg_size()!=Args.size())
    {
      ErrorF("Redef of function with a different num of args");
      return 0;
    }
  }
  unsigned Idx = 0;
  for(Function::arg_iterator AI = F->arg_begin(); Idx != Args.size(); ++AI, ++Idx)
  {
    AI->setName(Args[Idx]);
    NamedValues[Args[Idx]] = AI;  //Add arguements to symbol table
  }

  return F;
}

Function* FunctionAST::Codegen()
{
  NamedValues.clear();
  Function* theFunction = Proto->Codegen();
  if (theFunction == 0)
    return 0;
  //create a new basic block to start insertion info
  BasicBlock* BB = BasicBlock::Create(getGlobalContext(), "entry", theFunction);
  Builder.SetInsertPoint(BB);
  if(Value* RetVal = Body->Codegen())
  {
    //Finish off the function
    Builder.CreateRet(RetVal);
    //validate generated code
    verifyFunction(*theFunction);
    return theFunction;
  }
  //There was an error reading the body! remove function
  theFunction->eraseFromParent();
  return 0;
}

static int CurTok;
static int getNextToken() 
{
  return CurTok = gettok();
}

static std::map<char, int> BinopPrecedence;

static int GetTokPrecedence() 
{
  if (!isascii(CurTok))
    return -1;
  
  // Make sure it's a declared binop.
  int TokPrec = BinopPrecedence[CurTok];
  if (TokPrec <= 0) return -1;
  return TokPrec;
}


static ExprAST *ParseExpression();

static ExprAST *ParseIdentifierExpr() 
{
  std::string IdName = IdentifierStr;
  
  getNextToken();  // eat identifier.
  
  if (CurTok != '(') // Simple variable ref.
    return new VariableExprAST(IdName);
  
  // Call.
  getNextToken();  // eat (
  std::vector<ExprAST*> Args;
  if (CurTok != ')') 
  {
    while (1) 
    {
      ExprAST *Arg = ParseExpression();
      if (!Arg) return 0;
      Args.push_back(Arg);

      if (CurTok == ')') break;

      if (CurTok != ',')
        return Error("Expected ')' or ',' in argument list");
      getNextToken();
    }
  }

  // Eat the ')'.
  getNextToken();
  
  return new CallExprAST(IdName, Args);
}

static ExprAST *ParseNumberExpr() 
{
  ExprAST *Result = new NumberExprAST(NumVal);
  getNextToken(); // consume the number
  return Result;
}

static ExprAST *ParseParenExpr() 
{
  getNextToken();  // eat (.
  ExprAST *V = ParseExpression();
  if (!V) return 0;
  
  if (CurTok != ')')
    return Error("expected ')'");
  getNextToken();  // eat ).
  return V;
}

static ExprAST *ParsePrimary() 
{
  switch (CurTok) 
  {
    default: return Error("unknown token when expecting an expression");
    case tok_identifier: return ParseIdentifierExpr();
    case tok_number:     return ParseNumberExpr();
    case '(':            return ParseParenExpr();
  }
}

static ExprAST *ParseBinOpRHS(int ExprPrec, ExprAST *LHS) 
{
  // If this is a binop, find its precedence.
  while (1) 
  {
    int TokPrec = GetTokPrecedence();
    
    // If this is a binop that binds at least as tightly as the current binop,
    // consume it, otherwise we are done.
    if (TokPrec < ExprPrec)
      return LHS;
    
    // Okay, we know this is a binop.
    int BinOp = CurTok;
    getNextToken();  // eat binop
    
    // Parse the primary expression after the binary operator.
    ExprAST *RHS = ParsePrimary();
    if (!RHS) return 0;
    
    // If BinOp binds less tightly with RHS than the operator after RHS, let
    // the pending operator take RHS as its LHS.
    int NextPrec = GetTokPrecedence();
    if (TokPrec < NextPrec) 
    {
      RHS = ParseBinOpRHS(TokPrec+1, RHS);
      if (RHS == 0) return 0;
    }
    
    // Merge LHS/RHS.
    LHS = new BinaryExprAST(BinOp, LHS, RHS);
  }
}

static ExprAST *ParseExpression() 
{
  ExprAST *LHS = ParsePrimary();
  if (!LHS) return 0;
  
  return ParseBinOpRHS(0, LHS);
}

static PrototypeAST *ParsePrototype() 
{
  if (CurTok != tok_identifier)
    return ErrorP("Expected function name in prototype");

  std::string FnName = IdentifierStr;
  getNextToken();
  
  if (CurTok != '(')
    return ErrorP("Expected '(' in prototype");
  
  std::vector<std::string> ArgNames;
  while (getNextToken() == tok_identifier)
    ArgNames.push_back(IdentifierStr);
  if (CurTok != ')')
    return ErrorP("Expected ')' in prototype");
  
  // success.
  getNextToken();  // eat ')'.
  
  return new PrototypeAST(FnName, ArgNames);
}

static FunctionAST *ParseDefinition() 
{
  getNextToken();  // eat def.
  PrototypeAST *Proto = ParsePrototype();
  if (Proto == 0) return 0;

  if (ExprAST *E = ParseExpression())
    return new FunctionAST(Proto, E);
  return 0;
}

static FunctionAST *ParseTopLevelExpr() 
{
  if (ExprAST *E = ParseExpression()) 
  {
    // Make an anonymous proto.
    PrototypeAST *Proto = new PrototypeAST("", std::vector<std::string>());
    return new FunctionAST(Proto, E);
  }
  return 0;
}

static PrototypeAST *ParseExtern() 
{
  getNextToken();  // eat extern.
  return ParsePrototype();
}

static void HandleDefinition() 
{
  if (FunctionAST *F = ParseDefinition()) 
  {
    if (Function *LF = F->Codegen())
    {
      fprintf(stderr, "Parsed a function definition.\n");
      LF->dump();
    }
  } 
  else
  {
    // Skip token for error recovery.
    getNextToken();
  }
}

static void HandleExtern()
{
  if (PrototypeAST* P = ParseExtern())
  {
    if (Function* F = P->Codegen())
    {
      fprintf(stderr, "Parsed an extern\n");
      F->dump();
    }
  }
  else
  {
    // Skip token for error recovery.
    getNextToken();
  }
}

static void HandleTopLevelExpression() 
{
  // Evaluate a top-level expression into an anonymous function.
  if (FunctionAST* F = ParseTopLevelExpr()) 
  {
    if (Function *LF = F->Codegen())
    {
      fprintf(stderr, "Parsed a top-level expr\n");
      LF->dump();
    }
  } 
  else
  {
    // Skip token for error recovery.
    getNextToken();
  }
}

/// top ::= definition | external | expression | ';'
static void MainLoop() 
{
  while (1)
 {
    fprintf(stderr, "ready> ");
    switch (CurTok)
    {
      case tok_eof:    return;
      case ';':        getNextToken(); break;  // ignore top-level semicolons.
      case tok_def:    HandleDefinition(); break;
      case tok_extern: HandleExtern(); break;
      default:         HandleTopLevelExpression(); break;
    }
  }
}

int main() 
{
  // Install standard binary operators.
  // 1 is lowest precedence.
  BinopPrecedence['<'] = 10;
  BinopPrecedence['+'] = 20;
  BinopPrecedence['-'] = 20;
  BinopPrecedence['*'] = 40;  // highest.

  // Prime the first token.
  fprintf(stderr, "ready> ");
  getNextToken();

  // Run the main "interpreter loop" now.
  MainLoop();

  return 0;
}

