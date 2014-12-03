#include<iostream>
#include<stdlib.h>
using namespace std;

class Node
{
public:
  virtual ~Node() {}  
}

class NumberNode : public Node
{
  double value;
public:
  NumberNode(double val) : value(val) {}
}

class VariableNode : public Node
{
  string Name;
public:
  VariableNode(string name) : Name(name) {}
}

class BinExprNode : public Node
{
public:
  BinExprNode(char op, Node* lhs, Node* rhs) {}
}

class funcCallNode : public Node
{
  string Callee;
  vector<Node> Args;
public:
  funcCallNode(const string &callee, vector<string> &args) : Callee(callee), Args(args) {}
}

class prototypeNode
{
  string Name;
  vector<string> Args;
public:
  prototypeNode(const string &name, vector<string> &args) : Name(name), Args(args) {}
}

class FunctionNode
{
public:
  FunctionNode(prototypeNode* proto, Node* body) {}
}
