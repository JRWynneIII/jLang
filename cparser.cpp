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
  string value;       //Holds the value of the var in a string format. Use atof() or atoi() to convert
  string type;        //Holds string telling what type the value is
  char scope;         //Tells scope of var. 'g': global 'f': function 'l': loop
  string originFunc;  //Provides context for scope
} tblEntry;

map<string,tblEntry> varTbl;

map<string,string> stringTable;
map<string,double> doubleTable;
map<string,int> intTable;
vector<string> args;
vector<string> funcNames;
map<string,vector<string> > funcTable;


void checkDupVar(string name, string type)
{
  if(strcmp(name.c_str(),"string"))
  {
    map<string,string>::iterator it;
    for (it = stringTable.begin(); it != stringTable.end(); it++)
      if(it->first == name)
      {
        cout<<"ERROR: Duplicate Variable " << name << endl;
        exit(EXIT_FAILURE);
      }
  }
  else if (strcmp(name.c_str(),"double"))
  {
    map<string,double>::iterator it;
    for (it = doubleTable.begin(); it != doubleTable.end(); it++)
      if(it->first == name)
      {
        cout<<"ERROR: Duplicate Variable " << name << endl;
        exit(EXIT_FAILURE);
      }
  }
  else if (strcmp(name.c_str(),"int"))
  {
    map<string,int>::iterator it;
    for (it = intTable.begin(); it != intTable.end(); it++)
      if(it->first == name)
      {
        cout<<"ERROR: Duplicate Variable " << name << endl;
        exit(EXIT_FAILURE);
      }
  }
  else if (strcmp(name.c_str(), "func"))
  {
    vector<string>::iterator it;
    for(it = funcNames.begin(); it!= funcNames.end(); it++)
    {
      if (*it == name)
      {
        cout <<"ERROR: Duplicate Function Definition: " << name << endl;
        exit(EXIT_FAILURE);
      } 
    }
  }  
}

void storeVar(string name, string type, string value)
{
  if(strcmp(name.c_str(),"string"))
     stringTable[name] = value;
  else if (strcmp(name.c_str(),"double"))
     doubleTable[name] = atof(value.c_str());
  else if (strcmp(name.c_str(),"int"))
     intTable[name] = atoi(value.c_str());
  else if (strcmp(name.c_str(), "func"))
  {
     funcTable[name] = args;
     args.clear();
  }
}

int parseStart()
{

}
