#include<iostream>
#include<string>
#include<cstdlib>
#include<stack>
#include<vector>
#include<map>
#include<stdlib.h>
#include<string.h>
using namespace std;
extern FILE* yyin;

typedef struct symEntry
{
  string type;        //Holds string telling what type the value is

  char* valC = NULL;
  int valI;
  double valD;
  string valS;

  char scope = 'g';         //Tells scope of var. 'g': global 'f': function 'l': loop
  string originFunc;  //Provides context for scope
} tblEntry;

map<string,tblEntry> varTbl;

bool checkVar(string name)
{
  map<string,tblEntry>::iterator it;
  for(it = varTbl.begin(); it != varTbl.end(); it++)
  {
    if(it->first == name)
      return true;
  }
  return false;
}

void storeNewVar(string name, string type, string value)
{
  if (checkVar(name))
  {
    cout << "ERROR: Duplicate variable defined! " << name << endl;
    exit(EXIT_FAILURE);
  }
  if (strcmp(type.c_str(),"char") == 0)
  {
    tblEntry var;
    var.type = type;
    var.valC = const_cast<char *>(value.c_str());
    varTbl[name] = var;
  }
  else if (strcmp(type.c_str(),"int") == 0)
  {
    tblEntry var;
    var.type = type;
    var.valI = atoi(value.c_str());
    varTbl[name] = var;
  }
  else if (strcmp(type.c_str(),"double") == 0)
  {
    tblEntry var;
    var.type = type;
    var.valD = atof(value.c_str());
    varTbl[name] = var;
  }
  else if (strcmp(type.c_str(),"string") == 0)
  {
    tblEntry var;
    var.type = type;
    var.valS = value;
    varTbl[name] = var;
  }
}

void updateVar(string name, string value)
{
  if (checkVar(name))
  {
    cout << "ERROR: '" << name << "' not defined!\n";
    exit(EXIT_FAILURE);
  }
  tblEntry var = varTbl[name];
  string type = var.type;
  if (type == "int")
    var.valI = atoi(value.c_str());
  else if (type == "double")
    var.valD = atof(value.c_str());
  else if (type == "char")
    var.valC = const_cast<char *>(value.c_str());
  else if (type == "string")
    var.valS = value;
}

int parseStart()
{
  
}
