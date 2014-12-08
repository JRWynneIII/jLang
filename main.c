#include<iostream>
#include<map>
#include<stdio.h>
#include<stdlib.h>
#include "parse.tab.h"

extern FILE* yyin;
extern std::map<std::string, double> varTable;
extern std::map<std::string, std::string> varStringTbl;
void storeVar(char* id, double value)
{
  std::string temp = id;
  std::map<std::string, double>::iterator it;
  for (it = varTable.begin(); it != varTable.end(); it++)
  {
    if (it->first == temp)
    {
      std::cout << "Duplicate Variable!\n";
      exit(EXIT_FAILURE);
    }
  }
  varTable[temp] = value;
}

void storeStringVar(char* id, char* value)
{
  std::string temp = id;
  std::string tempVal = value;
  std::map<std::string, std::string>::iterator it;
  for (it = varStringTbl.begin(); it != varStringTbl.end(); it++)
  {
    if(it->first == temp)
    {
      std::cout << "Duplicate Variable\n";
      exit(EXIT_FAILURE);
    }
  }
  varStringTbl[temp] = tempVal;
}

int main(int argc, char*argv[])
{
  if (argc >1)
    yyin = fopen(argv[1],"r");
  yyparse();
  return EXIT_SUCCESS;
}
