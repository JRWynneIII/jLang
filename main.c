#include<iostream>
#include<map>
#include<stdio.h>
#include<stdlib.h>
#include "parse.tab.h"

extern FILE* yyin;
extern std::map<std::string, double> varTable;
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
int main(int argc, char*argv[])
{
  if (argc >1)
    yyin = fopen(argv[1],"r");
  yyparse();
  return EXIT_SUCCESS;
}
