#include<iostream>
#include<cctype>
#include<cstdlib>
#include<cstdio>
#include<map>
#include<vector>
#include<string>
using namespace std;

enum Token 
{
  tok_eof = -1,
  tok_def = -2,
  tok_extern = -3,
  tok_identifier = -4,
  tok_number = -5,
};

static string IdentifierStr; //has value if tok_ident
static double NumVal; //has value if tok_number

//get token!!
static int gettok()
{
  static int LastChar = ' ';
  //skip any whitespace
  while (isspace(LastChar))
    LastChar = getchar(); //gets char from stdin

  //look for idents
  if (isalpha(LastChar))
  {
    IdentifierStr = LastChar;
    while(isalnum((LastChar = getchar())))
      IdentifierStr += LastChar;

    //check for known idents
    if (IdentifierStr == "def")
      return tok_def;
    if (IdentifierStr == "extern")
      return tok_extern;

    return tok_identifier;
  }

  if (isdigit(LastChar))
  {
    string NumStr;
    do
    {
      NumStr += LastChar;
      LastChar = getchar();
    } while (isdigit(LastChar) || LastChar == '.');

    NumVal = strtod(NumStr.c_str(), 0); // TODO: SINCE IT INCORRECTLY READS 1.23.45 AS 1.23 CORRECT THIS FOR ONLY ONE DECIMAL PT
    return tok_number;
  }

  //Comment tiiiime
  //just skip to the end of the line if encounters a # 
  if (LastChar == '#')
  {
    do LastChar = getchar();
    while (LastChar != EOF && LastChar != '\n' && LastChar != '\r');
    if (LastChar != EOF)
    {
      return gettok();
    }
  }

  //check for end of the file
  if (LastChar == EOF)
  {
    return tok_eof;
  }

  //finally if the character isn't recognized etc
  int thisChar = LastChar;
  LastChar = getchar();
  return thisChar;
}

//Begin AST stuff
namespace
{
  class ExprAST 
  {
public:
    virtual ~ExprAST() {}
  };
  
  //Class for number literals
  
  class NumberExprAST : public ExprAST
  {
    double Val;
public:
    NumberExprAST(double val) : Val(val) {}
  };
  
  //Variable timmee
  class VariableExprAST : public ExprAST
  {
    string Name;
public:
    VariableExprAST(const string &name) : Name(name) {}
  };
  
  //Binary op 
  class BinaryExprAST : public ExprAST
  {
    char Op;
    ExprAST *LHS, *RHS;
public:
    BinaryExprAST(char op, ExprAST *lhs, ExprAST *rhs) : Op(op), LHS(lhs), RHS(rhs) {}
  };
  
  //Func calls
  class CallExprAST : public ExprAST
  {
    string Callee;
    vector<ExprAST*> Args;
public:
    CallExprAST(const string &callee, vector<ExprAST*> args) : Callee(callee), Args(args) {}
  };
  
  class PrototypeAST
  {
    string Name;
    vector<string> Args;
public:
    PrototypeAST(const string &name, const vector<string> args) : Name(name), Args(args) {}
  };
  
  class FunctionAST
  {
    PrototypeAST* Proto;
    ExprAST* Body;
public:
    FunctionAST(PrototypeAST* proto, ExprAST* body) : Proto(proto), Body(body) {}
  };
} //end namespace
//Parse timee
static ExprAST* ParseExpression();
static ExprAST* ParseIdentifierExpr();
static ExprAST* ParseBinOpRHS(int ExprPrec, ExprAST* LHS);

static int CurTok; //Current token we're parsing
static int getNextToken() //update curtok
{
  return CurTok = gettok();
}

//Error handling funcs
ExprAST *Error(const char *Str)
{
  fprintf(stderr, "Error: %s\n", Str);
  return 0;
}

PrototypeAST *ErrorP(const char *Str)
{
  Error(Str);
  return 0;
}

FunctionAST *ErrorF(const char *Str)
{
  Error(Str);
  return 0;
}

static ExprAST* ParseNumberExpr()
{
  ExprAST* Result = new NumberExprAST(NumVal);
  getNextToken();
  return Result;
}

static ExprAST* ParseParenExpr()
{
  getNextToken(); //eat the (
  ExprAST* V = ParseExpression();
  if (!V) return 0;
  if (CurTok != ')')
  {
    return Error("Expected )");
  }
  getNextToken(); //eat the )
  return V;
}

//handle variable refs and func calls
static ExprAST* ParseIdentifierExpr()
{
  string IdName = IdentifierStr;
  getNextToken(); //Eat ident. Must be a hungry compiler
  if (CurTok != '(') //Simple variable reference
    return new VariableExprAST(IdName);
  //if get here then function
  getNextToken(); //Eat the ( 
  vector<ExprAST*> Args;
  if (CurTok != ')')
  {
    while(1)
    {
      ExprAST* Arg = ParseExpression();
      if (!Arg)
        return 0;
      Args.push_back(Arg);
      if (CurTok == ')')
        break;
      if (CurTok != ',')
        return Error("Expected ')' or ',' in arguement list!");
      getNextToken();
    }
  }
  getNextToken(); //Eat the )
  return new CallExprAST(IdName,Args);
}

//Parser entry point
static ExprAST* ParsePrimary()
{
  switch(CurTok)
  {
    default: 
      return Error("Unknown Token when expecting an expression!");
    case tok_identifier:
      return ParseIdentifierExpr();
    case tok_number:
      return ParseNumberExpr();
    case '(':
      return ParseParenExpr();
  }
}

//Binary Expression Parsing
static map<char,int> BinopPrecedence;

static int GetTokPrecedence()
{
  if (!isascii(CurTok))
    return -1;

  int TokPrec = BinopPrecedence[CurTok];
  if (TokPrec <= 0)
    return -1;
  return TokPrec;
}


static ExprAST* ParseExpression()
{
  ExprAST* LHS = ParsePrimary();
  if (!LHS)
    return 0;
  return ParseBinOpRHS(0,LHS);
}

static ExprAST* ParseBinOpRHS(int ExprPrec, ExprAST* LHS)
{
  while (1)
  {
    int TokPrec = GetTokPrecedence();
    if (TokPrec < ExprPrec)
      return LHS;
    int BinOp = CurTok;
    getNextToken(); //eat the operator
    ExprAST* RHS = ParsePrimary(); //parse the expr after the binop
    if (!RHS)
      return 0;
    int NextPrec = GetTokPrecedence();
    if (TokPrec < NextPrec)
    {
      RHS = ParseBinOpRHS(TokPrec+1, RHS);
      if (RHS == 0)
        return 0;
    }
    LHS = new BinaryExprAST(BinOp,LHS,RHS);
  }
}

static PrototypeAST* ParsePrototype()
{
  if (CurTok != tok_identifier)
    return ErrorP("Expected function name in prototype");
  string FnName = IdentifierStr;
  getNextToken();
  if (CurTok != '(')
    return ErrorP("Expected '(' in protoype");
  //read arguements
  vector<string> ArgNames;
  while (getNextToken() == tok_identifier)
    ArgNames.push_back(IdentifierStr);
  if (CurTok != ')')
    return ErrorP("Expected ')' in prototype");
  getNextToken(); //eat the )
  return new PrototypeAST(FnName, ArgNames);
}

static FunctionAST* ParseDefinition()
{
  getNextToken(); //eat the def
  PrototypeAST* Proto = ParsePrototype();
  if (Proto ==0)
    return 0;
  if (ExprAST* E = ParseExpression())
    return new FunctionAST(Proto,E);
  return 0;
}

static PrototypeAST* ParseExtern()
{
  getNextToken(); //Eat the extern
  return ParsePrototype();
}

static FunctionAST* ParseTopLevelExpr()
{
  if (ExprAST* E = ParseExpression())
  {
    PrototypeAST* Proto = new PrototypeAST("",vector<string>());
    return new FunctionAST(Proto, E);
  }
  return 0;
}

static void HandleDefinition()
{
  if(ParseDefinition())
    fprintf(stderr,"Parsed a function definition.\n");
  else
    getNextToken(); //skip token for errorrecovery
}

static void HandleExtern()
{
  if(ParseExtern())
    fprintf(stderr,"Parsed a extern function definition.\n");
  else
    getNextToken(); //skip token for errorrecovery
}

static void HandleTopLevelExpression()
{
  if(ParseTopLevelExpr())
    fprintf(stderr,"Parsed a top level expression.\n");
  else
    getNextToken(); //skip token for errorrecovery
}

static void MainLoop()
{
  while(1)
  {
    fprintf(stderr,"ready> ");
    switch(CurTok)
    {
      case tok_eof:
        return;
      case ';': 
        getNextToken();
        break;
      case tok_def:
        HandleDefinition();
        break;
      case tok_extern:
        HandleExtern();
        break;
      default:
        HandleTopLevelExpression();
        break;
    }
  }
}

int main(void)
{
  BinopPrecedence['<'] = 10;
  BinopPrecedence['+'] = 20;
  BinopPrecedence['-'] = 30;
  BinopPrecedence['*'] = 40;
  fprintf(stderr,"ready> ");
  getNextToken();
  MainLoop();
  return 0;
}
